#pragma once
#include "Order_Id.hpp"
#include "Price.hpp"
#include "Quantity.hpp"
#include <vector>

class Trade {
public:
    Trade(Order_Id buyer, Order_Id seller, Price price, Quantity quantity)
        : buyer_(buyer), seller_(seller), price_(price), quantity_(quantity) {}

    Order_Id getBuyerId()   const { return buyer_; }
    Order_Id getSellerId()  const { return seller_; }
    Price    getPrice()     const { return price_; }
    Quantity getQuantity()  const { return quantity_; }

private:
    Order_Id buyer_;
    Order_Id seller_;
    Price    price_;
    Quantity quantity_;
};

using Trades = std::vector<Trade>;