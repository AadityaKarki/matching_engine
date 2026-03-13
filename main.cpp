#include <iostream>
#include "Order_Book.hpp"

int main() {
    Order_Book book;

    // --- Test cancel ---
    Order o1(Order_Id(1), Side::Buy,  Price(100), Quantity(10));
    book.addOrder(o1);
    std::cout << "Added buy order 1\n";

    book.cancelOrder(Order_Id(1));
    std::cout << "Cancelled order 1\n";

    // --- Test matching ---
    Order o2(Order_Id(2), Side::Buy,  Price(100), Quantity(5));
    Order o3(Order_Id(3), Side::Sell, Price(100), Quantity(5));

    auto trades = book.addOrder(o2);
    std::cout << "Added buy order 2, trades: " << trades.size() << "\n";  // 0

    trades = book.addOrder(o3);
    std::cout << "Added sell order 3, trades: " << trades.size() << "\n"; // 1

    for (auto& t : trades) {
        std::cout << "Trade: buyer=" << t.getBuyerId().value()
                  << " seller="     << t.getSellerId().value()
                  << " price="      << t.getPrice().value()
                  << " qty="        << t.getQuantity().value() << "\n";
    }

    // --- Test double-cancel throws ---
    Order o4(Order_Id(4), Side::Sell, Price(105), Quantity(3));
    book.addOrder(o4);
    book.cancelOrder(Order_Id(4));
    try {
        book.cancelOrder(Order_Id(4));  // should throw
    } catch (const std::logic_error& e) {
        std::cout << "Caught expected error: " << e.what() << "\n";
    }

    return 0;
}
