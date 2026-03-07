#pragma once
#include <cstdint>

class Price {
public:
    explicit constexpr Price(int32_t price) : price_(price) {}

    constexpr int32_t value() const { return price_; }

    friend constexpr bool operator< (Price a, Price b) { return a.price_ <  b.price_; }
    friend constexpr bool operator> (Price a, Price b) { return a.price_ >  b.price_; }
    friend constexpr bool operator<=(Price a, Price b) { return a.price_ <= b.price_; }
    friend constexpr bool operator>=(Price a, Price b) { return a.price_ >= b.price_; }
    friend constexpr bool operator==(Price a, Price b) { return a.price_ == b.price_; }
    friend constexpr bool operator!=(Price a, Price b) { return a.price_ != b.price_; }

private:
    int32_t price_;
};