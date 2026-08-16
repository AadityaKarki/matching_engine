# Matching Engine

A lightweight, header-only C++20 order book and matching engine.

## Overview

This project implements a **price-time priority** (FIFO) central limit order book (CLOB). Orders are matched using standard exchange rules:

- **Bids** are sorted highest price first.
- **Asks** are sorted lowest price first.
- A trade occurs when the best bid price ≥ the best ask price.
- Partial fills are supported; an order remains in the book until fully filled or cancelled.
- **Trade price** is always the resting (passive) side's price — i.e. the ask price.

## Project Structure

```
matching_engine/
├── engine/
│   ├── Order_Book.hpp   # Core order book: add, cancel, modify, match
│   ├── Order.hpp        # Order class (limit & market), fill logic
│   ├── Result.hpp       # CancelResult / ModifyResult enums
│   ├── Order_Id.hpp     # Strong type wrapper for order identifiers
│   ├── Order_Type.hpp   # Enum: Limit / Market
│   ├── Price.hpp        # Strong type wrapper for price
│   ├── Quantity.hpp     # Strong type wrapper for quantity
│   ├── Side.hpp         # Enum: Buy / Sell
│   └── Trade.hpp        # Trade record (buyer, seller, price, qty)
├── tests/
│   └── test_order_book.cpp   # Catch2 unit tests (13 test cases, 38 assertions)
├── main.cpp             # Demo / smoke tests
└── CMakeLists.txt
```

## API

```cpp
// Add a limit order; returns trades produced by matching.
[[nodiscard]] Trades addLimitOrder(Order_Id, Side, Price, Quantity);

// Add a market order; matches immediately, never rests in the book.
[[nodiscard]] Trades addMarketOrder(Order_Id, Side, Quantity);

// Cancel an open order. Returns CancelResult::Ok / NotFound / AlreadyFilled.
[[nodiscard]] CancelResult cancelOrder(Order_Id);

// Reduce qty in-place (preserves queue position).
// Increase qty cancels and re-adds (loses queue priority).
[[nodiscard]] ModifyResult modifyOrder(Order_Id, Quantity new_qty);
```

No exceptions are thrown for expected failure cases — callers check the returned enum.

## Data Structures

| Component | Structure | Complexity |
|-----------|-----------|------------|
| Bid levels | `std::map<Price, std::list<Order*>, std::greater<Price>>` | O(log P) insertion/lookup |
| Ask levels | `std::map<Price, std::list<Order*>, std::less<Price>>` | O(log P) insertion/lookup |
| Order lookup | `std::unordered_map<Order_Id, unique_ptr<Order>>` | O(1) average |
| Queue within level | `std::list<Order*>` | O(1) insert & O(1) cancel |

Each `Order` stores a `std::list` iterator (`location_`) so cancellation is O(1) — no linear search required. The `Order_Book` owns all orders via `std::unique_ptr`; callers pass parameters, not `Order` objects.

## Build & Test

Requires **CMake ≥ 3.14** and a **C++20** compiler. Catch2 is fetched automatically on first configure.

```bash
# Configure (downloads Catch2 on first run)
cmake -S . -B build

# Build
cmake --build build

# Run demo
./build/main.exe          # Windows
./build/main              # Linux/macOS

# Run unit tests
cd build && ctest --output-on-failure
```
