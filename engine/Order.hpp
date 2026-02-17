#pragma once
#include "Side.hpp"
#include "Order_Id.hpp"
#include "Price.hpp"
#include "Quantity.hpp"
#include <memory>
#include <vector>
#include <iostream>
class Order{
public:
    Order(Order_Id id, Side side, Price price, Quantity quantity):
    id_(id), side_(side), price_(price), intial_quantity_(quantity), remaining_quantity_(quantity){}
    void displayOrder(){  
        std::cout<<"Order ID: "<<id_.value()<<" Side: "<<side_<<" Price: "<<price.value()<<" Quantity: "<<intial_quantity.value()<<" Remaining Quantity: "<<remaining_quantity.value()<<std::endl;
    }
    Order_Id getOrderId(){return id_.value();}
    Side getSide(){return side_;}
    Price getPrice(){return price.value();}
    Quantity getInitialQuantity(){return intial_quantity.value();}
    Quantity getRemainingQuantity(){return remaining_quantity.value();}
    void Fill(Quantity quantity){
        if(quantity > remaining_quantity)
            throw std::logic_error("Quantity to fill is greater than remaining quantity");
        remaining_quantity -= quantity;
    }
    bool isFilled(){return remaining_quantity == 0;}    
private:
    Order_Id id_;
    Side side_;
    Price price_;
    Quantity intial_quantity_;
    Quantity remaining_quantity_;
}   

using Orderptr = Order*;