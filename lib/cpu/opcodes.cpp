#include "cpu.hpp"

const std::array<CPU::OpCode, 256> &CPU::GetOpTable() {
  static const std::array<OpCode, 256> table = [] {
    std::array<OpCode, 256> t = {};

    // load instructions
    // -- LDA
    t[0xa9] = {&CPU::op_lda, AddressingMode::Immediate, 2};
    t[0xa5] = {&CPU::op_lda, AddressingMode::ZeroPage, 2};
    t[0xb5] = {&CPU::op_lda, AddressingMode::ZeroPage_X, 2};
    t[0xad] = {&CPU::op_lda, AddressingMode::Absolute, 3};
    t[0xbd] = {&CPU::op_lda, AddressingMode::Absolute_X, 3};
    t[0xb9] = {&CPU::op_lda, AddressingMode::Absolute_Y, 3};
    t[0xa1] = {&CPU::op_lda, AddressingMode::Indirect_X, 2};
    t[0xb1] = {&CPU::op_lda, AddressingMode::Indirect_Y, 2};
    // -- LDX
    t[0xa2] = {&CPU::op_ldx, AddressingMode::Immediate, 2};
    t[0xa6] = {&CPU::op_ldx, AddressingMode::ZeroPage, 2};
    t[0xb6] = {&CPU::op_ldx, AddressingMode::ZeroPage_Y, 2};
    t[0xae] = {&CPU::op_ldx, AddressingMode::Absolute, 3};
    t[0xbe] = {&CPU::op_ldx, AddressingMode::Absolute_Y, 3};
    // -- LDY
    t[0xa0] = {&CPU::op_ldy, AddressingMode::Immediate, 2};
    t[0xa4] = {&CPU::op_ldy, AddressingMode::ZeroPage, 2};
    t[0xb4] = {&CPU::op_ldy, AddressingMode::ZeroPage_X, 2};
    t[0xac] = {&CPU::op_ldy, AddressingMode::Absolute, 3};
    t[0xbc] = {&CPU::op_ldy, AddressingMode::Absolute_X, 3};

    // store instructions
    // -- STA
    t[0x85] = {&CPU::op_sta, AddressingMode::ZeroPage, 2};
    t[0x95] = {&CPU::op_sta, AddressingMode::ZeroPage_X, 2};
    t[0x8d] = {&CPU::op_sta, AddressingMode::Absolute, 3};
    t[0x9d] = {&CPU::op_sta, AddressingMode::Absolute_X, 3};
    t[0x99] = {&CPU::op_sta, AddressingMode::Absolute_Y, 3};
    t[0x81] = {&CPU::op_sta, AddressingMode::Indirect_X, 2};
    t[0x91] = {&CPU::op_sta, AddressingMode::Indirect_Y, 2};
    // -- STX
    t[0x86] = {&CPU::op_stx, AddressingMode::ZeroPage, 2};
    t[0x96] = {&CPU::op_stx, AddressingMode::ZeroPage_Y, 2};
    t[0x8e] = {&CPU::op_stx, AddressingMode::Absolute, 3};
    // -- STY
    t[0x84] = {&CPU::op_sty, AddressingMode::ZeroPage, 2};
    t[0x94] = {&CPU::op_sty, AddressingMode::ZeroPage_X, 2};
    t[0x8c] = {&CPU::op_sty, AddressingMode::Absolute, 3};

    // register transfer instructions
    // -- TAX
    t[0xaa] = {&CPU::op_tax, AddressingMode::Implied, 1};
    // -- TAY
    t[0xa8] = {&CPU::op_tay, AddressingMode::Implied, 1};
    // -- TXA
    t[0x8a] = {&CPU::op_txa, AddressingMode::Implied, 1};
    // -- TYA
    t[0x98] = {&CPU::op_tya, AddressingMode::Implied, 1};

    // stack operations
    // -- TSX
    t[0xba] = {&CPU::op_tsx, AddressingMode::Implied, 1};
    // -- TXS
    t[0x9a] = {&CPU::op_txs, AddressingMode::Implied, 1};
    // -- PHA
    t[0x48] = {&CPU::op_pha, AddressingMode::Implied, 1};
    // -- PHP
    t[0x08] = {&CPU::op_php, AddressingMode::Implied, 1};
    // -- PLA
    t[0x68] = {&CPU::op_pla, AddressingMode::Implied, 1};
    // -- PLP
    t[0x28] = {&CPU::op_plp, AddressingMode::Implied, 1};

    // logical operations
    // -- AND
    t[0x29] = {&CPU::op_and, AddressingMode::Immediate, 2};
    t[0x25] = {&CPU::op_and, AddressingMode::ZeroPage, 2};
    t[0x35] = {&CPU::op_and, AddressingMode::ZeroPage_X, 2};
    t[0x2d] = {&CPU::op_and, AddressingMode::Absolute, 3};
    t[0x3d] = {&CPU::op_and, AddressingMode::Absolute_X, 3};
    t[0x39] = {&CPU::op_and, AddressingMode::Absolute_Y, 3};
    t[0x21] = {&CPU::op_and, AddressingMode::Indirect_X, 2};
    t[0x31] = {&CPU::op_and, AddressingMode::Indirect_Y, 2};
    // -- EOR
    t[0x49] = {&CPU::op_eor, AddressingMode::Immediate, 2};
    t[0x45] = {&CPU::op_eor, AddressingMode::ZeroPage, 2};
    t[0x55] = {&CPU::op_eor, AddressingMode::ZeroPage_X, 2};
    t[0x4d] = {&CPU::op_eor, AddressingMode::Absolute, 3};
    t[0x5d] = {&CPU::op_eor, AddressingMode::Absolute_X, 3};
    t[0x59] = {&CPU::op_eor, AddressingMode::Absolute_Y, 3};
    t[0x41] = {&CPU::op_eor, AddressingMode::Indirect_X, 2};
    t[0x51] = {&CPU::op_eor, AddressingMode::Indirect_Y, 2};
    // -- ORA
    t[0x09] = {&CPU::op_ora, AddressingMode::Immediate, 2};
    t[0x05] = {&CPU::op_ora, AddressingMode::ZeroPage, 2};
    t[0x15] = {&CPU::op_ora, AddressingMode::ZeroPage_X, 2};
    t[0x0d] = {&CPU::op_ora, AddressingMode::Absolute, 3};
    t[0x1d] = {&CPU::op_ora, AddressingMode::Absolute_X, 3};
    t[0x19] = {&CPU::op_ora, AddressingMode::Absolute_Y, 3};
    t[0x01] = {&CPU::op_ora, AddressingMode::Indirect_X, 2};
    t[0x11] = {&CPU::op_ora, AddressingMode::Indirect_Y, 2};
    // -- BIT
    t[0x24] = {&CPU::op_bit, AddressingMode::ZeroPage, 2};
    t[0x2c] = {&CPU::op_bit, AddressingMode::Absolute, 3};

    return t;
  }();

  return table;
}
