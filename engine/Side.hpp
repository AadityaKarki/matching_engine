#pragma once

enum class Side{
Buy,
Sell
}

constexpr bool is_buy(Side side) {
    return side == Side::Buy;
}
