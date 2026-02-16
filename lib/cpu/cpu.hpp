#pragma once

#include <array>
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
                    uint16_t start_addr = 0x8000);
  void step();

  uint16_t get_pc() { return pc; }
  uint8_t get_reg_a() { return reg_a; }
  uint8_t get_reg_x() { return reg_x; }
  uint8_t get_reg_y() { return reg_y; }
  uint8_t get_status() { return status; }

  uint8_t mem_read(uint16_t addr);

private:
  std::array<uint8_t, 0xFFFF> memory;
  uint16_t pc;
  uint8_t reg_a;
  uint8_t reg_x;
  uint8_t reg_y;
  uint8_t status;

  void op_lda(AddressingMode &mode);
  void op_ldx(AddressingMode &mode);
  void op_ldy(AddressingMode &mode);
  void op_sta(AddressingMode &mode);
  void op_stx(AddressingMode &mode);
  void op_sty(AddressingMode &mode);
  void op_tax();
  void op_tay();
  void op_txa();
  void op_tya();
  void op_inx();

  void set_register_a(uint8_t value);
  void set_register_x(uint8_t value);
  void set_register_y(uint8_t value);
  void update_zero_and_negative_flags(uint8_t value);

  uint16_t mem_read_u16(uint16_t addr);
  void mem_write(uint16_t addr, uint8_t data);

  uint8_t fetch_next_byte();
  uint16_t get_addr(AddressingMode &mode);
};
