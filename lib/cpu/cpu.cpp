#include "cpu.hpp"
#include "constants.hpp"
#include "opcodes.hpp"
#include <stdexcept>

CPU::CPU() : pc(0), reg_a(0), reg_x(0), reg_y(0), status(0) {}

void CPU::load_program(const std::vector<uint8_t> &program,
                       uint16_t start_addr) {
  for (size_t i = 0; i < program.size(); ++i) {
    mem_write(start_addr + i, program[i]);
  }

  pc = start_addr;
}

void CPU::step() {
  uint8_t code = fetch_next_byte();
  auto map = GetOpCodeMap();
  auto opcode = map[code];

  if (map.find(code) == map.end()) {
    printf("invalid opcode: 0x%02x", code);
    return;
  }

  switch (code) {
  // load operations
  case 0xa9:
  case 0xa5:
  case 0xb5:
  case 0xad:
  case 0xbd:
  case 0xb9:
  case 0xa1:
  case 0xb1:
    op_lda(opcode.mode);
    break;
  case 0xa2:
  case 0xa6:
  case 0xb6:
  case 0xae:
  case 0xbe:
    op_ldx(opcode.mode);
    break;
  case 0xa0:
  case 0xa4:
  case 0xb4:
  case 0xac:
  case 0xbc:
    op_ldy(opcode.mode);
    break;

  // store operations
  case 0x85:
  case 0x95:
  case 0x8d:
  case 0x9d:
  case 0x99:
  case 0x81:
  case 0x91:
    op_sta(opcode.mode);
    break;
  case 0x86:
  case 0x96:
  case 0x8e:
    op_stx(opcode.mode);
    break;
  case 0x84:
  case 0x94:
  case 0x8c:
    op_sty(opcode.mode);
    break;

  // register transfers
  case 0xaa:
    op_tax();
    break;
  case 0xa8:
    op_tay();
    break;
  case 0x8a:
    op_txa();
    break;
  case 0x98:
    op_tya();
    break;

  case 0xe8:
    op_inx();
    break;
  case 0x00:
    return;
  }

  pc += opcode.num_bytes - 1;
}

// instruction handlers
void CPU::op_lda(AddressingMode &mode) {
  auto addr = get_addr(mode);
  uint8_t value = mem_read(addr);
  set_register_a(value);
}
void CPU::op_ldx(AddressingMode &mode) {
  auto addr = get_addr(mode);
  uint8_t value = mem_read(addr);
  set_register_x(value);
}
void CPU::op_ldy(AddressingMode &mode) {
  auto addr = get_addr(mode);
  uint8_t value = mem_read(addr);
  set_register_y(value);
}
void CPU::op_sta(AddressingMode &mode) {
  auto addr = get_addr(mode);
  mem_write(addr, reg_a);
}
void CPU::op_stx(AddressingMode &mode) {
  auto addr = get_addr(mode);
  mem_write(addr, reg_x);
}
void CPU::op_sty(AddressingMode &mode) {
  auto addr = get_addr(mode);
  mem_write(addr, reg_y);
}
void CPU::op_tax() { set_register_x(reg_a); }
void CPU::op_tay() { set_register_y(reg_a); }
void CPU::op_txa() { set_register_a(reg_x); }
void CPU::op_tya() { set_register_a(reg_y); }
void CPU::op_inx() { set_register_x(reg_x + 1); }

// register and flags utils
void CPU::set_register_a(uint8_t value) {
  reg_a = value;
  update_zero_and_negative_flags(value);
}
void CPU::set_register_x(uint8_t value) {
  reg_x = value;
  update_zero_and_negative_flags(value);
}
void CPU::set_register_y(uint8_t value) {
  reg_y = value;
  update_zero_and_negative_flags(value);
}
void CPU::update_zero_and_negative_flags(uint8_t value) {
  if (value == 0) {
    status |= flags::ZERO;
  } else {
    status &= ~flags::ZERO;
  }

  if ((value & (1 << 7)) != 0) {
    status |= flags::NEGATIVE;
  } else {
    status &= ~flags::NEGATIVE;
  }
}

// memory utils
uint8_t CPU::mem_read(uint16_t addr) { return memory[addr]; }
uint16_t CPU::mem_read_u16(uint16_t addr) {
  auto low = static_cast<uint16_t>(mem_read(addr));
  auto high = static_cast<uint16_t>(mem_read(addr + 1));

  return (high << 8) | low;
}
void CPU::mem_write(uint16_t addr, uint8_t data) { memory[addr] = data; }

// additional utils
uint8_t CPU::fetch_next_byte() { return memory[pc++]; }
uint16_t CPU::get_addr(AddressingMode &mode) {
  switch (mode) {
  case Immediate:
    return pc;
  case ZeroPage:
    return mem_read(pc);
  case ZeroPage_X:
    return mem_read(pc) + reg_x;
  case ZeroPage_Y:
    return mem_read(pc) + reg_y;
  case Absolute:
    return mem_read_u16(pc);
  case Absolute_X:
    return mem_read_u16(pc) + static_cast<uint16_t>(reg_x);
  case Absolute_Y:
    return mem_read_u16(pc) + static_cast<uint16_t>(reg_y);
  case Indirect: {
    uint16_t low = mem_read(pc);
    uint16_t high = mem_read(pc + 1);
    return (high << 8) | low;
  }
  case Indirect_X: {
    uint8_t base = mem_read(pc);
    uint8_t zero_page_addr = (base + reg_x) & 0xFF;
    uint8_t low = mem_read(zero_page_addr);
    uint8_t high = mem_read((zero_page_addr + 1) & 0xFF);
    return (high << 8) | low;
  }
  case Indirect_Y: {
    uint8_t zero_page_addr = mem_read(pc);
    uint8_t low = mem_read(zero_page_addr);
    uint8_t high = mem_read((zero_page_addr + 1) & 0xFF);
    uint16_t base_addr = (high << 8) | low;
    return base_addr + static_cast<uint16_t>(reg_y);
  }
  default:
    throw std::runtime_error("invalid addressing mode: " +
                             std::to_string(static_cast<int>(mode)));
  }
}