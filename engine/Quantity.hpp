#pragma once

class Quantity{
public:
    explicit constexpr Quantity(uint32_t quantity): quantity_(quantity){};
    uint32_t constexpr value(){return quantity_;}
    constexpr Quantity& operator-=(Quantity other) {
        assert(quantity_ >= other.quantity_);
        quantity_ -= other.quantity_;
        return *this;
    }
    friend constexpr bool operator>(Quantity a, Quantity b) {
        return a.quantity_ > b.quantity_;
    }
    
private:
    uint32_t quantity_;
}
