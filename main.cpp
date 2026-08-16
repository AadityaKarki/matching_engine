#include <iostream>
#include "Order_Book.hpp"

int main() {
    Order_Book book;


    (void)book.addLimitOrder(Order_Id(1), Side::Buy, Price(100), Quantity(10));
    std::cout << "Added buy order 1\n";

    auto r = book.cancelOrder(Order_Id(1));
    std::cout << "Cancel order 1: " << to_string(r) << "\n";

    (void)book.addLimitOrder(Order_Id(2), Side::Buy,  Price(100), Quantity(5));
    std::cout << "Added buy order 2\n";

    auto trades = book.addLimitOrder(Order_Id(3), Side::Sell, Price(100), Quantity(5));
    std::cout << "Added sell order 3, trades: " << trades.size() << "\n";
    for (auto& t : trades) {
        std::cout << "  Trade: buyer="  << t.getBuyerId().value()
                  << " seller="         << t.getSellerId().value()
                  << " price="          << t.getPrice().value()
                  << " qty="            << t.getQuantity().value() << "\n";
    }

    (void)book.addLimitOrder(Order_Id(4), Side::Sell, Price(105), Quantity(3));
    (void)book.cancelOrder(Order_Id(4));

    auto r2 = book.cancelOrder(Order_Id(4));
    std::cout << "Second cancel order 4: " << to_string(r2) << "\n"; 


    (void)book.addLimitOrder(Order_Id(5), Side::Sell, Price(99), Quantity(7));
    auto mTrades = book.addMarketOrder(Order_Id(6), Side::Buy, Quantity(4));
    std::cout << "Market buy order 6, trades: " << mTrades.size() << "\n";
    for (auto& t : mTrades) {
        std::cout << "  Trade: buyer="  << t.getBuyerId().value()
                  << " seller="         << t.getSellerId().value()
                  << " price="          << t.getPrice().value()
                  << " qty="            << t.getQuantity().value() << "\n";
    }


    (void)book.addLimitOrder(Order_Id(7), Side::Buy, Price(90), Quantity(10));
    auto mr = book.modifyOrder(Order_Id(7), Quantity(6));
    std::cout << "Modify order 7 to qty=6: " << to_string(mr) << "\n";

    return 0;
}
