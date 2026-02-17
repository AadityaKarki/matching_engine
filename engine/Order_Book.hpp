#pragma once

#include "Order.hpp"
#include <map>
#include <list>
#include <vector>


class Order_Book{
    public:

    private:
    map<Price, std::list<Orderptr>, std::greater<Price>>bidList_;
    map<Price, std::list<Orderptr>, std::less<Price>>askList_;
    unordered_map<Order_Id, Orderptr> orderMap_;
    Price bestBidPrice_;
    Price bestAskPrice_;
    Price lastTradePrice_;
    void addOrder(Order& order){
        orderMap_.insert({order.getOrderId(), &order});
        if(order.getSide() == Side::Buy){
            bidList_.insert({order.getPrice(), &order});
        }
        else{
            askList_.insert({order.getPrice(), &order});
        }
    }
    void cancelOrder(Order_Id order_id){
        auto it = orderMap_.find(order_id);
        if(it != orderMap_.end()){
            Orderptr order = it->second;
            if(order->isFilled()){
                throw std::logic_error("Order is already filled");
            }
            else{
                if(order->getSide() == Side::Buy){
                    bidList_.erase(order->getPrice());
                }
                else{
                    askList_.erase(order->getPrice());
                }
                orderMap_.erase(order_id);
               
            }
        }
    }
}