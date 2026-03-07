#pragma once
#include <cstdint>
#include <functional>  

class Order_Id {
public:
    explicit constexpr Order_Id(uint64_t id) : id_(id) {}

    constexpr uint64_t value() const { return id_; }

    friend constexpr bool operator==(Order_Id a, Order_Id b) {
        return a.id_ == b.id_;
    }

    // Needed for unordered_map key
    struct Hash {
        std::size_t operator()(Order_Id oid) const noexcept {
            return std::hash<uint64_t>{}(oid.id_);
        }
    };

private:
    uint64_t id_;
};
