#pragma once

#include "constants.hpp"
#include <bus/bus.hpp>
#include <cstdint>
#include <vector>

enum AddressingMode {
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

  uint16_t get_pc() { return pc; }
  uint8_t get_reg_a() { return reg_a; }
  uint8_t get_reg_x() { return reg_x; }
  uint8_t get_reg_y() { return reg_y; }
  uint8_t get_status() { return status; }

  Bus bus;

private:
  uint16_t pc;
  uint8_t sp;
  uint8_t reg_a;
  uint8_t reg_x;
  uint8_t reg_y;
  uint8_t status;

  // load operations
  void op_lda(AddressingMode &mode);
  void op_ldx(AddressingMode &mode);
  void op_ldy(AddressingMode &mode);
  // store operations
  void op_sta(AddressingMode &mode);
  void op_stx(AddressingMode &mode);
  void op_sty(AddressingMode &mode);
  // register transfers
  void op_tax();
  void op_tay();
  void op_txa();
  void op_tya();
  // stack operations
  void op_tsx();
  void op_txs();
  void op_pha();
  void op_php();
  void op_pla();
  void op_plp();

  void op_inx();

  void set_register_a(uint8_t value);
  void set_register_x(uint8_t value);
  void set_register_y(uint8_t value);
  void update_zero_and_negative_flags(uint8_t value);

  void stack_push(uint8_t data);
  uint8_t stack_pop();

  uint8_t fetch_next_byte();
  uint16_t get_addr(AddressingMode &mode);
};
