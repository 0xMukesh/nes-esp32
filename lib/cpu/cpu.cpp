#include "cpu.hpp"
#include <stdexcept>

CPU::CPU()
    : pc(0), sp(static_cast<uint8_t>(memory_map::STACK_START)), reg_a(0),
      reg_x(0), reg_y(0), status(0) {}

void CPU::load_program(const std::vector<uint8_t> &program,
                       uint16_t start_addr) {
  for (size_t i = 0; i < program.size(); ++i) {
    bus.mem_write(start_addr + i, program[i]);
  }

  pc = start_addr;
}

void CPU::step() {
  uint8_t code = fetch_next_byte();
  const auto &entry = GetOpTable()[code];
  (this->*entry.handler)(entry.mode);
  pc += entry.bytes - 1;
}

// load operations
void CPU::op_lda(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  set_register_a(value);
}
void CPU::op_ldx(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  set_register_x(value);
}
void CPU::op_ldy(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  set_register_y(value);
}
// store operations
void CPU::op_sta(AddressingMode mode) {
  auto addr = get_addr(mode);
  bus.mem_write(addr, reg_a);
}
void CPU::op_stx(AddressingMode mode) {
  auto addr = get_addr(mode);
  bus.mem_write(addr, reg_x);
}
void CPU::op_sty(AddressingMode mode) {
  auto addr = get_addr(mode);
  bus.mem_write(addr, reg_y);
}
// register transfers
void CPU::op_tax(AddressingMode) { set_register_x(reg_a); }
void CPU::op_tay(AddressingMode) { set_register_y(reg_a); }
void CPU::op_txa(AddressingMode) { set_register_a(reg_x); }
void CPU::op_tya(AddressingMode) { set_register_a(reg_y); }
// stack operations
void CPU::op_tsx(AddressingMode) { set_register_x(sp); }
void CPU::op_txs(AddressingMode) { sp = reg_x; }
void CPU::op_pha(AddressingMode) { stack_push(reg_a); }
void CPU::op_php(AddressingMode) { stack_push(status); }
void CPU::op_pla(AddressingMode) {
  uint8_t data = stack_pop();
  set_register_a(data);
}
void CPU::op_plp(AddressingMode) {
  uint8_t data = stack_pop();
  status = data;
}
// logical operations
void CPU::op_and(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  set_register_a(reg_a & value);
}
void CPU::op_eor(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  set_register_a(reg_a ^ value);
}
void CPU::op_ora(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  set_register_a(reg_a | value);
}
void CPU::op_bit(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  auto and_value = reg_a & value;

  if (and_value == 0) {
    add_status(flags::ZERO);
  } else {
    remove_status(flags::ZERO);
  }

  if (value & (1 << 7)) {
    add_status(flags::NEGATIVE);
  } else {
    remove_status(flags::NEGATIVE);
  }

  if (value & (1 << 6)) {
    add_status(flags::OVERFLOW);
  } else {
    remove_status(flags::OVERFLOW);
  }
}

// register utils
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

// flag utils
void CPU::add_status(uint8_t mask) { status |= mask; }
void CPU::remove_status(uint8_t mask) { status &= ~mask; }
void CPU::update_zero_and_negative_flags(uint8_t value) {
  if (value == 0) {
    add_status(flags::ZERO);
  } else {
    remove_status(flags::ZERO);
  }

  if ((value & (1 << 7)) != 0) {
    add_status(flags::NEGATIVE);
  } else {
    remove_status(flags::NEGATIVE);
  }
}

// stack utils
void CPU::stack_push(uint8_t data) {
  bus.mem_write(memory_map::STACK_START + static_cast<uint16_t>(sp), data);
  --sp;
}
uint8_t CPU::stack_pop() {
  ++sp;
  return bus.mem_read(memory_map::STACK_START + static_cast<uint16_t>(sp));
}

// mem utils
uint8_t CPU::mem_read(uint16_t addr) { return bus.mem_read(addr); }
uint16_t CPU::mem_read_u16(uint16_t addr) { return bus.mem_read_u16(addr); }

// additional utils
uint8_t CPU::fetch_next_byte() { return bus.mem_read(pc++); }
uint16_t CPU::get_addr(AddressingMode mode) {
  switch (mode) {
  case Immediate:
    return pc;
  case ZeroPage:
    return bus.mem_read(pc);
  case ZeroPage_X:
    return bus.mem_read(pc) + reg_x;
  case ZeroPage_Y:
    return bus.mem_read(pc) + reg_y;
  case Absolute:
    return bus.mem_read_u16(pc);
  case Absolute_X:
    return bus.mem_read_u16(pc) + static_cast<uint16_t>(reg_x);
  case Absolute_Y:
    return bus.mem_read_u16(pc) + static_cast<uint16_t>(reg_y);
  case Indirect: {
    uint16_t low = bus.mem_read(pc);
    uint16_t high = bus.mem_read(pc + 1);
    return (high << 8) | low;
  }
  case Indirect_X: {
    uint8_t base = bus.mem_read(pc);
    uint8_t zero_page_addr = (base + reg_x) & 0xFF;
    uint8_t low = bus.mem_read(zero_page_addr);
    uint8_t high = bus.mem_read((zero_page_addr + 1) & 0xFF);
    return (high << 8) | low;
  }
  case Indirect_Y: {
    uint8_t zero_page_addr = bus.mem_read(pc);
    uint8_t low = bus.mem_read(zero_page_addr);
    uint8_t high = bus.mem_read((zero_page_addr + 1) & 0xFF);
    uint16_t base_addr = (high << 8) | low;
    return base_addr + static_cast<uint16_t>(reg_y);
  }
  default:
    throw std::runtime_error("invalid addressing mode: " +
                             std::to_string(static_cast<int>(mode)));
  }
}