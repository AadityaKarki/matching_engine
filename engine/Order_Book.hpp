#pragma once

#include "Order.hpp"
#include <map>
#include <list>
#include <vector>


class Order_Book{
    public:

    private:
    vector<Order> orders_;
    map<Price, std::list<Orderptr>, std::greater<Price>>bidList_;
    map<Price, std::list<Orderptr>, std::less<Price>>askList_;
    unordered_map<Order_Id, Orderptr> orderMap_;
    void addOrder(Order& order){
        orders_.push_back(order);
        orderMap_.insert({order.getOrderId(), &orders_.back()});
        if(order.getSide() == Side::Buy){
            bidList_.insert({order.getPrice(), &orders_.back()});
        }
        else{
            askList_.insert({order.getPrice(), &orders_.back()});
        }
    }
}