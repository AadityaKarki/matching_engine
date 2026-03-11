#pragma once
#include <ostream>
enum class Side{
Buy,
Sell
};

inline std::ostream& operator<<(std::ostream& os, Side side) {
    return os << (side == Side::Buy ? "Buy" : "Sell");
}
constexpr bool is_buy(Side side) {
    return side == Side::Buy;
}
