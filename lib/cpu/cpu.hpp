#pragma once

#include "../bus/bus.hpp"
#include "../constants/constants.hpp"
#include <cstdint>
#include <vector>

enum AddressingMode : uint8_t {
  Immediate,
  ZeroPage,
  ZeroPage_X,
  ZeroPage_Y,
  Absolute,
  Absolute_X,
  Absolute_Y,
  Indirect,
  Indirect_X, // indexed indirect
  Indirect_Y, // indirect indexed
  Implied
};

class CPU {
public:
  CPU();

  void load_program(const std::vector<uint8_t> &program,
                    uint16_t start_addr = memory_map::PRGROM_START);
  void step();

  uint16_t get_pc() const { return pc; }
  uint8_t get_sp() const { return sp; }
  uint8_t get_reg_a() const { return reg_a; }
  uint8_t get_reg_x() const { return reg_x; }
  uint8_t get_reg_y() const { return reg_y; }
  uint8_t get_status() const { return status; }

  // mem utils
  uint8_t mem_read(uint16_t addr);
  uint16_t mem_read_u16(uint16_t addr);

private:
  uint16_t pc;
  uint8_t sp;
  uint8_t reg_a;
  uint8_t reg_x;
  uint8_t reg_y;
  uint8_t status;
  Bus bus;

  // opcode helpers
  struct OpCode {
    void (CPU::*handler)(AddressingMode);
    AddressingMode mode;
    uint8_t bytes;
  };

  static const std::array<OpCode, 256> &GetOpTable();

  // load operations
  void op_lda(AddressingMode mode);
  void op_ldx(AddressingMode mode);
  void op_ldy(AddressingMode mode);
  // store operations
  void op_sta(AddressingMode mode);
  void op_stx(AddressingMode mode);
  void op_sty(AddressingMode mode);
  // register transfers
  void op_tax(AddressingMode);
  void op_tay(AddressingMode);
  void op_txa(AddressingMode);
  void op_tya(AddressingMode);
  // stack operations
  void op_tsx(AddressingMode);
  void op_txs(AddressingMode);
  void op_pha(AddressingMode);
  void op_php(AddressingMode);
  void op_pla(AddressingMode);
  void op_plp(AddressingMode);
  // logical operations (performed on register A)
  void op_and(AddressingMode mode);
  void op_eor(AddressingMode mode);
  void op_ora(AddressingMode mode);
  void op_bit(AddressingMode mode);

  // register utils
  void set_register_a(uint8_t value);
  void set_register_x(uint8_t value);
  void set_register_y(uint8_t value);

  // flag utils
  void add_status(uint8_t mask);
  void remove_status(uint8_t mask);
  void update_zero_and_negative_flags(uint8_t value);

  // stack utils
  void stack_push(uint8_t data);
  uint8_t stack_pop();

  // additional utils
  uint8_t fetch_next_byte();
  uint16_t get_addr(AddressingMode mode);
};
