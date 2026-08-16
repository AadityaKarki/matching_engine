#pragma once
#include <string_view>

enum class CancelResult {
    Ok,
    NotFound,
    AlreadyFilled,
};

enum class ModifyResult {
    Ok,
    NotFound,
    AlreadyFilled,
};

constexpr std::string_view to_string(CancelResult r) noexcept {
    switch (r) {
        case CancelResult::Ok:           return "Ok";
        case CancelResult::NotFound:     return "NotFound";
        case CancelResult::AlreadyFilled:return "AlreadyFilled";
    }
    return "Unknown";
}

constexpr std::string_view to_string(ModifyResult r) noexcept {
    switch (r) {
        case ModifyResult::Ok:           return "Ok";
        case ModifyResult::NotFound:     return "NotFound";
        case ModifyResult::AlreadyFilled:return "AlreadyFilled";
    }
    return "Unknown";
}
