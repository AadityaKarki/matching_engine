#pragma once
#include "Side.hpp"
#include "Order_Id.hpp"
#include "Price.hpp"
#include "Quantity.hpp"
#include "Order_Type.hpp"
#include <stdexcept>
#include <list>
#include <memory>

class Order {
public:
    // Limit order
    Order(Order_Id id, Side side, Price price, Quantity quantity) noexcept
        : id_(id), type_(Order_Type::Limit), side_(side), price_(price),
          initial_quantity_(quantity), remaining_quantity_(quantity) {}

    // Market order
    Order(Order_Id id, Side side, Quantity quantity) noexcept
        : id_(id), type_(Order_Type::Market), side_(side), price_(),
          initial_quantity_(quantity), remaining_quantity_(quantity) {}

    // Non-copyable, movable
    Order(const Order&)            = delete;
    Order& operator=(const Order&) = delete;
    Order(Order&&)                 = default;
    Order& operator=(Order&&)      = default;

    [[nodiscard]] Order_Id   getOrderId()            const noexcept { return id_; }
    [[nodiscard]] Order_Type getType()               const noexcept { return type_; }
    [[nodiscard]] Side       getSide()               const noexcept { return side_; }
    [[nodiscard]] Price      getPrice()              const noexcept { return price_; }
    [[nodiscard]] Quantity   getInitialQuantity()    const noexcept { return initial_quantity_; }
    [[nodiscard]] Quantity   getRemainingQuantity()  const noexcept { return remaining_quantity_; }
    [[nodiscard]] bool       isFilled()              const noexcept { return remaining_quantity_ == Quantity{0}; }

    void Fill(Quantity quantity) {
        if (quantity > remaining_quantity_)
            throw std::logic_error("Fill quantity exceeds remaining quantity");
        remaining_quantity_ -= quantity;
    }

    // Non-owning position handle in the price-level list (set by Order_Book)
    using ListIt = std::list<Order*>::iterator;
    ListIt location_;

private:
    Order_Id   id_;
    Order_Type type_;
    Side       side_;
    Price      price_;
    Quantity   initial_quantity_;
    Quantity   remaining_quantity_;
};

using Orderptr    = std::unique_ptr<Order>;   // owning handle
using OrderRawPtr = Order*;                   // non-owning observer