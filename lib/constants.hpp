#pragma once

#include <cstdint>

// N V _ B D I Z C

namespace flags {
constexpr uint8_t CARRY = (1 << 0);
constexpr uint8_t ZERO = (1 << 1);
constexpr uint8_t INTERRUPT_DISABLE = (1 << 2);
constexpr uint8_t DECIMAL_MODE = (1 << 3);
constexpr uint8_t BREAK = (1 << 4);
constexpr uint8_t OVERFLOW = (1 << 6);
constexpr uint8_t NEGATIVE = (1 << 7);
} // namespace flags
