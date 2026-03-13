#pragma once
#include <ostream>
enum class Order_Type {
    Limit,
    Market
};
inline std::ostream& operator<<(std::ostream& os, Order_Type type) {
    return os << (type == Order_Type::Limit ? "Limit" : "Market");
}