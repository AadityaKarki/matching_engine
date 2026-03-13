#pragma once
#include <cstdint>

class Price {
public:
    explicit constexpr Price(int32_t price) : price_(price) {}
    constexpr Price() : price_(-1) {}
    constexpr int32_t value() const { return price_; }
    constexpr bool isValid() const { return price_ >= 0; }
    friend constexpr bool operator< (Price a, Price b) { return a.price_ <  b.price_; }
    friend constexpr bool operator> (Price a, Price b) { return a.price_ >  b.price_; }
    friend constexpr bool operator<=(Price a, Price b) { return a.price_ <= b.price_; }
    friend constexpr bool operator>=(Price a, Price b) { return a.price_ >= b.price_; }
    friend constexpr bool operator==(Price a, Price b) { return a.price_ == b.price_; }
    friend constexpr bool operator!=(Price a, Price b) { return a.price_ != b.price_; }
    friend std::ostream& operator<<(std::ostream& os, const Price& p) {
    return os << p.price_;
}
private:
    int32_t price_;
};