#pragma once

#include "Order.hpp"
#include "Trade.hpp"
#include <map>
#include <list>
#include <vector>
#include <unordered_map>
#include <stdexcept>

class Order_Book {
public:
    
    Trades addOrder(Order& order) {
        insertOrder(order);
        return matchOrders();
    }

    void cancelOrder(Order_Id order_id) {
        auto it = orderMap_.find(order_id);
        if (it == orderMap_.end())
            throw std::logic_error("Order not found");

        Orderptr order = it->second;

        if (order->isFilled())
            throw std::logic_error("Order is already filled");

    
        if (order->getSide() == Side::Buy) {
            auto& orders = bidList_.at(order->getPrice());
            orders.erase(order->location_);         
            if (orders.empty())
                bidList_.erase(order->getPrice());
        } else {
            auto& orders = askList_.at(order->getPrice());
            orders.erase(order->location_);       
            if (orders.empty())
                askList_.erase(order->getPrice());
        }

        orderMap_.erase(it);
    }

private:
    std::map<Price, std::list<Orderptr>, std::greater<Price>> bidList_;
    std::map<Price, std::list<Orderptr>, std::less<Price>>    askList_;
    std::unordered_map<Order_Id, Orderptr, Order_Id::Hash> orderMap_;

    void insertOrder(Order& order) {
        Orderptr ptr = &order;
        orderMap_.insert({order.getOrderId(), ptr});

        if (order.getSide() == Side::Buy) {
            auto& lst = bidList_[order.getPrice()];   
            lst.push_back(ptr);
            order.location_ = std::prev(lst.end());   
        } else {
            auto& lst = askList_[order.getPrice()];
            lst.push_back(ptr);
            order.location_ = std::prev(lst.end());
        }
    }

    bool canMatch(Side side, Price price) const {
        if (side == Side::Buy) {
            if (askList_.empty()) return false;
            const auto& [bestAsk, _] = *askList_.begin();
            return bestAsk <= price;
        } else {
            if (bidList_.empty()) return false;
            const auto& [bestBid, _] = *bidList_.begin();
            return bestBid >= price;
        }
    }

    Trades matchOrders() {
        Trades trades;

        while (!bidList_.empty() && !askList_.empty()) {
            auto& [bestBid, bids] = *bidList_.begin();
            auto& [bestAsk, asks] = *askList_.begin();

            if (bestBid < bestAsk) break;

            while (!bids.empty() && !asks.empty()) {
                Orderptr bid = bids.front();
                Orderptr ask = asks.front();

                Quantity qty = std::min(bid->getRemainingQuantity(),
                                        ask->getRemainingQuantity());
                bid->Fill(qty);
                ask->Fill(qty);

                
                trades.push_back(Trade{bid->getOrderId(), ask->getOrderId(), bestAsk, qty});

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
};