#pragma once

#include "cpu/cpu.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>

struct OpCode {
  std::string mneomic;
  uint8_t code;
  AddressingMode mode;
  int8_t num_bytes;
};

inline const std::vector<OpCode> &GetOpCodes() {
  static const std::vector<OpCode> opcodes = {
      // load operations
      {"LDA", 0xa9, AddressingMode::Immediate, 2},
      {"LDA", 0xa5, AddressingMode::ZeroPage, 2},
      {"LDA", 0xb5, AddressingMode::ZeroPage_X, 2},
      {"LDA", 0xad, AddressingMode::Absolute, 3},
      {"LDA", 0xbd, AddressingMode::Absolute_X, 3},
      {"LDA", 0xb9, AddressingMode::Absolute_Y, 3},
      {"LDX", 0xa2, AddressingMode::Immediate, 2},
      {"LDX", 0xa6, AddressingMode::ZeroPage, 2},
      {"LDX", 0xb6, AddressingMode::ZeroPage_Y, 2},
      {"LDX", 0xae, AddressingMode::Absolute, 3},
      {"LDX", 0xbe, AddressingMode::Absolute_Y, 3},
      {"LDY", 0xa0, AddressingMode::Immediate, 2},
      {"LDY", 0xa4, AddressingMode::ZeroPage, 2},
      {"LDY", 0xb4, AddressingMode::ZeroPage_X, 2},
      {"LDY", 0xac, AddressingMode::Absolute, 3},
      {"LDY", 0xbc, AddressingMode::Absolute_X, 3},
      {"LDA", 0xa1, AddressingMode::Indirect_X, 2},
      {"LDA", 0xb1, AddressingMode::Indirect_Y, 2},
      // store operations
      {"STA", 0x85, AddressingMode::ZeroPage, 2},
      {"STA", 0x95, AddressingMode::ZeroPage_X, 2},
      {"STA", 0x8d, AddressingMode::Absolute, 3},
      {"STA", 0x9d, AddressingMode::Absolute_X, 3},
      {"STA", 0x99, AddressingMode::Absolute_Y, 3},
      {"STA", 0x81, AddressingMode::Indirect_X, 2},
      {"STA", 0x91, AddressingMode::Indirect_Y, 2},
      {"STX", 0x86, AddressingMode::ZeroPage, 2},
      {"STX", 0x96, AddressingMode::ZeroPage_Y, 2},
      {"STX", 0x96, AddressingMode::Absolute, 3},
      {"STY", 0x84, AddressingMode::ZeroPage, 2},
      {"STY", 0x94, AddressingMode::ZeroPage_X, 2},
      {"STY", 0x8c, AddressingMode::Absolute, 3},
      // register transfers
      {"TAX", 0xaa, AddressingMode::Implied, 1},
      {"TAY", 0xa8, AddressingMode::Implied, 1},
      {"TXA", 0x8a, AddressingMode::Implied, 1},
      {"TYA", 0x98, AddressingMode::Implied, 1},
      // stack operations
      {"TSX", 0xba, AddressingMode::Implied, 1},
      {"TXS", 0x9a, AddressingMode::Implied, 1},
      {"PHA", 0x48, AddressingMode::Implied, 1},
      {"PHP", 0x08, AddressingMode::Implied, 1},
      {"PLA", 0x68, AddressingMode::Implied, 1},
      {"PLP", 0x28, AddressingMode::Implied, 1},

      {"INX", 0xe8, AddressingMode::Implied, 1},
      {"BRK", 0x00, AddressingMode::Implied, 1},
  };

  return opcodes;
}

inline const std::unordered_map<uint8_t, OpCode> &GetOpCodeMap() {
  static const std::unordered_map<uint8_t, OpCode> map = [] {
    std::unordered_map<uint8_t, OpCode> m;

    for (const auto &op : GetOpCodes()) {
      m.emplace(op.code, op);
    }

    return m;
  }();

  return map;
}
