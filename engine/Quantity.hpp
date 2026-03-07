#pragma once
#include <cstdint>
#include <cassert>
#include <algorithm>  // for std::min

class Quantity {
public:
    explicit constexpr Quantity(uint32_t quantity) : quantity_(quantity) {}

    constexpr uint32_t value() const { return quantity_; }

    constexpr Quantity& operator-=(Quantity other) {
        assert(quantity_ >= other.quantity_);
        quantity_ -= other.quantity_;
        return *this;
    }

    friend constexpr bool operator==(Quantity a, Quantity b) { return a.quantity_ == b.quantity_; }
    friend constexpr bool operator!=(Quantity a, Quantity b) { return a.quantity_ != b.quantity_; }
    friend constexpr bool operator< (Quantity a, Quantity b) { return a.quantity_ <  b.quantity_; }
    friend constexpr bool operator> (Quantity a, Quantity b) { return a.quantity_ >  b.quantity_; }

    // std::min needs this
    friend constexpr Quantity min(Quantity a, Quantity b) {
        return a < b ? a : b;
    }

private:
    uint32_t quantity_;
};