#pragma once

#include "Order.hpp"
#include "Trade.hpp"
#include "Result.hpp"
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <memory>

class Order_Book {
public:
    explicit Order_Book(std::size_t expected_capacity = 1024) {
        orderMap_.reserve(expected_capacity);
    }

    // Add a limit order. Returns trades produced by matching.
    [[nodiscard]] Trades addLimitOrder(Order_Id id, Side side, Price price, Quantity quantity) {
        auto order = std::make_unique<Order>(id, side, price, quantity);
        OrderRawPtr raw = order.get();
        insertIntoBook(std::move(order));
        return matchOrders(raw);
    }

    // Add a market order. Matches immediately at best available price; never rests in the book.
    [[nodiscard]] Trades addMarketOrder(Order_Id id, Side side, Quantity quantity) {
        auto order = std::make_unique<Order>(id, side, quantity);
        return matchMarketOrder(std::move(order));
    }

    [[nodiscard]] CancelResult cancelOrder(Order_Id order_id) noexcept {
        auto it = orderMap_.find(order_id);
        if (it == orderMap_.end())
            return CancelResult::NotFound;

        OrderRawPtr order = it->second.get();

        if (order->isFilled())
            return CancelResult::AlreadyFilled;

        removeFromBook(order);
        orderMap_.erase(it);
        return CancelResult::Ok;
    }

    // Reduce quantity in-place (preserves queue position).
    // Increase quantity cancels and re-adds the order (loses priority).
    [[nodiscard]] ModifyResult modifyOrder(Order_Id order_id, Quantity new_qty) {
        auto it = orderMap_.find(order_id);
        if (it == orderMap_.end())
            return ModifyResult::NotFound;

        OrderRawPtr order = it->second.get();
        if (order->isFilled())
            return ModifyResult::AlreadyFilled;

        if (new_qty < order->getRemainingQuantity()) {
            // Reduce in-place — preserve queue position
            Quantity delta = Quantity{order->getRemainingQuantity().value() - new_qty.value()};
            order->Fill(delta);
            if (order->isFilled()) {
                removeFromBook(order);
                orderMap_.erase(it);
            }
        } else {
            // Increase — must re-queue at back (loses priority)
            Side  side  = order->getSide();
            Price price = order->getPrice();
            removeFromBook(order);
            orderMap_.erase(it);
            (void)addLimitOrder(order_id, side, price, new_qty);
        }
        return ModifyResult::Ok;
    }

    [[nodiscard]] bool empty() const noexcept {
        return orderMap_.empty();
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return orderMap_.size();
    }

private:
    // bids: highest price first; asks: lowest price first
    std::map<Price, std::list<OrderRawPtr>, std::greater<Price>> bidList_;
    std::map<Price, std::list<OrderRawPtr>, std::less<Price>>    askList_;
    std::unordered_map<Order_Id, Orderptr, Order_Id::Hash>       orderMap_;

    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    void insertIntoBook(Orderptr order) {
        OrderRawPtr raw = order.get();
        orderMap_.emplace(raw->getOrderId(), std::move(order));

        if (raw->getSide() == Side::Buy) {
            auto& lst = bidList_[raw->getPrice()];
            lst.push_back(raw);
            raw->location_ = std::prev(lst.end());
        } else {
            auto& lst = askList_[raw->getPrice()];
            lst.push_back(raw);
            raw->location_ = std::prev(lst.end());
        }
    }

    void removeFromBook(OrderRawPtr order) noexcept {
        if (order->getSide() == Side::Buy) {
            auto& lst = bidList_.at(order->getPrice());
            lst.erase(order->location_);
            if (lst.empty()) bidList_.erase(order->getPrice());
        } else {
            auto& lst = askList_.at(order->getPrice());
            lst.erase(order->location_);
            if (lst.empty()) askList_.erase(order->getPrice());
        }
    }

    // Returns true if any opposite-side liquidity can match the given side/price
    [[nodiscard]] bool canMatch(Side side, Price price) const noexcept {
        if (side == Side::Buy) {
            if (askList_.empty()) return false;
            return askList_.begin()->first <= price;
        } else {
            if (bidList_.empty()) return false;
            return bidList_.begin()->first >= price;
        }
    }

    // Match the just-added limit order against resting opposite side
    Trades matchOrders(OrderRawPtr /*hint — unused, kept for future optimisations*/) {
        Trades trades;

        while (!bidList_.empty() && !askList_.empty()) {
            auto& [bestBid, bids] = *bidList_.begin();
            auto& [bestAsk, asks] = *askList_.begin();

            if (bestBid < bestAsk) break;

            while (!bids.empty() && !asks.empty()) {
                OrderRawPtr bid = bids.front();
                OrderRawPtr ask = asks.front();

                Quantity qty = std::min(bid->getRemainingQuantity(),
                                       ask->getRemainingQuantity());
                bid->Fill(qty);
                ask->Fill(qty);

                // Trade price = passive (ask) side
                trades.emplace_back(bid->getOrderId(), ask->getOrderId(), bestAsk, qty);

                if (bid->isFilled()) {
                    bids.pop_front();
                    orderMap_.erase(bid->getOrderId());
                }
                if (ask->isFilled()) {
                    asks.pop_front();
                    orderMap_.erase(ask->getOrderId());
                }
            }

            if (bids.empty()) bidList_.erase(bestBid);
            if (asks.empty()) askList_.erase(bestAsk);
        }

        return trades;
    }

    // Market order: match immediately at best available price, never rest
    Trades matchMarketOrder(Orderptr order) {
        Trades trades;
        OrderRawPtr raw  = order.get();
        Side        side = raw->getSide();

        // Market order sweeps the opposite side
        // Must use if/else — bidList_ and askList_ have different comparator types
        auto matchAgainstSide = [&](auto& opposite) {
        while (!opposite.empty() && !raw->isFilled()) {
            auto& [bestPrice, levelOrders] = *opposite.begin();

            while (!levelOrders.empty() && !raw->isFilled()) {
                OrderRawPtr resting = levelOrders.front();
                Quantity qty = std::min(raw->getRemainingQuantity(),
                                       resting->getRemainingQuantity());
                raw->Fill(qty);
                resting->Fill(qty);

                Order_Id buyerId  = (side == Side::Buy)  ? raw->getOrderId() : resting->getOrderId();
                Order_Id sellerId = (side == Side::Sell) ? raw->getOrderId() : resting->getOrderId();
                trades.emplace_back(buyerId, sellerId, bestPrice, qty);

                if (resting->isFilled()) {
                    levelOrders.pop_front();
                    orderMap_.erase(resting->getOrderId());
                }
            }

            if (levelOrders.empty())
                opposite.erase(bestPrice);
        }
        };
        if (side == Side::Buy)  matchAgainstSide(askList_);
        else                    matchAgainstSide(bidList_);

        // Market order is never inserted into the book; memory released here.
        return trades;
    }
};