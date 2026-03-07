#pragma once
#include "Side.hpp"
#include "Order_Id.hpp"
#include "Price.hpp"
#include "Quantity.hpp"
#include <stdexcept>
#include <iostream>

class Order {
public:
    Order(Order_Id id, Side side, Price price, Quantity quantity)
        : id_(id), side_(side), price_(price), initial_quantity_(quantity), remaining_quantity_(quantity) {}

    void displayOrder() const {
        std::cout << "Order ID: " << id_ 
                  << " Side: " << side_ 
                  << " Price: " << price_ 
                  << " Initial Qty: " << initial_quantity_ 
                  << " Remaining Qty: " << remaining_quantity_ << "\n";
    }

    Order_Id  getOrderId()          const { return id_; }
    Side      getSide()             const { return side_; }
    Price     getPrice()            const { return price_; }
    Quantity  getInitialQuantity()  const { return initial_quantity_; }
    Quantity  getRemainingQuantity()const { return remaining_quantity_; }

    void Fill(Quantity quantity) {
        if (quantity > remaining_quantity_)
            throw std::logic_error("Fill quantity exceeds remaining quantity");
        remaining_quantity_ -= quantity;
    }

    bool isFilled() const { return remaining_quantity_ == Quantity{0}; }

  
    using ListIt = std::list<Order*>::iterator;
    ListIt location_;          

private:
    Order_Id id_;
    Side     side_;
    Price    price_;
    Quantity initial_quantity_;
    Quantity remaining_quantity_;
};

using Orderptr = Order*;