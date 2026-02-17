#pragma once

class Order_Id{
public:
    explicit constexpr Order_Id(uint64_t id): id_(id){};
    uint64_t constexpr value(){return id_;}    
private:
    uint64_t id_;
}