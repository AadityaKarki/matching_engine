#include <catch2/catch_test_macros.hpp>
#include "Order_Book.hpp"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static Order_Id id(uint64_t v)  { return Order_Id(v); }
static Price    px(int32_t v)   { return Price(v); }
static Quantity qty(uint32_t v) { return Quantity(v); }

// ---------------------------------------------------------------------------
// Add & Cancel
// ---------------------------------------------------------------------------
TEST_CASE("Add a limit order and cancel it", "[cancel]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Buy, px(100), qty(10));

    REQUIRE(book.size() == 1);
    REQUIRE(book.cancelOrder(id(1)) == CancelResult::Ok);
    REQUIRE(book.empty());
}

TEST_CASE("Cancel non-existent order returns NotFound", "[cancel]") {
    Order_Book book;
    REQUIRE(book.cancelOrder(id(99)) == CancelResult::NotFound);
}

TEST_CASE("Cancel already-filled order returns AlreadyFilled", "[cancel]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Buy,  px(100), qty(5));
    book.addLimitOrder(id(2), Side::Sell, px(100), qty(5));
    // Both orders are fully matched and removed from the map,
    // so subsequent cancel returns NotFound (already erased).
    REQUIRE(book.cancelOrder(id(1)) == CancelResult::NotFound);
}

// ---------------------------------------------------------------------------
// Exact fill
// ---------------------------------------------------------------------------
TEST_CASE("Matching buy and sell at same price produces one trade", "[match]") {
    Order_Book book;
    auto t1 = book.addLimitOrder(id(1), Side::Buy,  px(100), qty(5));
    REQUIRE(t1.empty());

    auto t2 = book.addLimitOrder(id(2), Side::Sell, px(100), qty(5));
    REQUIRE(t2.size() == 1);

    const Trade& tr = t2[0];
    REQUIRE(tr.getBuyerId()  == id(1));
    REQUIRE(tr.getSellerId() == id(2));
    REQUIRE(tr.getPrice()    == px(100));
    REQUIRE(tr.getQuantity() == qty(5));

    REQUIRE(book.empty());
}

// ---------------------------------------------------------------------------
// Partial fill
// ---------------------------------------------------------------------------
TEST_CASE("Partial fill leaves remainder in book", "[match]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Buy,  px(100), qty(10));
    auto trades = book.addLimitOrder(id(2), Side::Sell, px(100), qty(6));

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getQuantity() == qty(6));

    // Order 1 has 4 remaining; order 2 is gone
    REQUIRE(book.size() == 1);
    REQUIRE(book.cancelOrder(id(1)) == CancelResult::Ok);
    REQUIRE(book.cancelOrder(id(2)) == CancelResult::NotFound);
}

// ---------------------------------------------------------------------------
// Price-time priority
// ---------------------------------------------------------------------------
TEST_CASE("Price priority: higher bid matches before lower bid", "[priority]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Buy, px(99),  qty(5));  // lower
    book.addLimitOrder(id(2), Side::Buy, px(101), qty(5));  // higher → traded first

    auto trades = book.addLimitOrder(id(3), Side::Sell, px(99), qty(5));
    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getBuyerId() == id(2));  // price-priority
}

TEST_CASE("Time priority: earlier order at same price matches first", "[priority]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Buy, px(100), qty(5));  // arrived first
    book.addLimitOrder(id(2), Side::Buy, px(100), qty(5));  // arrived second

    auto trades = book.addLimitOrder(id(3), Side::Sell, px(100), qty(5));
    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getBuyerId() == id(1));  // time-priority
}

// ---------------------------------------------------------------------------
// Market orders
// ---------------------------------------------------------------------------
TEST_CASE("Market buy order matches against resting asks", "[market]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Sell, px(98), qty(10));

    auto trades = book.addMarketOrder(id(2), Side::Buy, qty(4));
    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getPrice()    == px(98));
    REQUIRE(trades[0].getQuantity() == qty(4));
    REQUIRE(book.size() == 1);  // 6 still resting
}

TEST_CASE("Market sell order matches against resting bids", "[market]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Buy, px(105), qty(8));

    auto trades = book.addMarketOrder(id(2), Side::Sell, qty(8));
    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getPrice()    == px(105));
    REQUIRE(trades[0].getQuantity() == qty(8));
    REQUIRE(book.empty());
}

TEST_CASE("Market order with no liquidity produces no trades", "[market]") {
    Order_Book book;
    auto trades = book.addMarketOrder(id(1), Side::Buy, qty(5));
    REQUIRE(trades.empty());
    REQUIRE(book.empty());
}

// ---------------------------------------------------------------------------
// Modify order
// ---------------------------------------------------------------------------
TEST_CASE("Modify reduces quantity in-place and preserves position", "[modify]") {
    Order_Book book;
    book.addLimitOrder(id(1), Side::Buy, px(100), qty(10));
    book.addLimitOrder(id(2), Side::Buy, px(100), qty(5));

    // Reduce id(1) from 10 → 6; it should still be ahead of id(2) in queue
    auto mr = book.modifyOrder(id(1), qty(6));
    REQUIRE(mr == ModifyResult::Ok);

    // Sell 6 → should match id(1) at front (price-time, id(1) still first)
    auto trades = book.addLimitOrder(id(3), Side::Sell, px(100), qty(6));
    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getBuyerId() == id(1));
    REQUIRE(trades[0].getQuantity() == qty(6));
}

TEST_CASE("Modify non-existent order returns NotFound", "[modify]") {
    Order_Book book;
    REQUIRE(book.modifyOrder(id(99), qty(5)) == ModifyResult::NotFound);
}

// ---------------------------------------------------------------------------
// Trade price convention
// ---------------------------------------------------------------------------
TEST_CASE("Trade executes at passive (ask) price", "[convention]") {
    Order_Book book;
    // Aggressive buyer bids 105, resting ask is 100 → trade at 100
    book.addLimitOrder(id(1), Side::Sell, px(100), qty(5));
    auto trades = book.addLimitOrder(id(2), Side::Buy, px(105), qty(5));

    REQUIRE(trades.size() == 1);
    REQUIRE(trades[0].getPrice() == px(100));  // passive (ask) price
}
