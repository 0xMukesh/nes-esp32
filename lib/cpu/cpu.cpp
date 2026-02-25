#include "cpu.hpp"
#include <optional>
#include <stdexcept>

CPU::CPU()
    : pc(0), sp(static_cast<uint8_t>(memory_map::STACK_START)), reg_a(0),
      reg_x(0), reg_y(0), status(0b00100000) {}

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
void CPU::op_php(AddressingMode) {
  set_flag(flags::BREAK, true);
  stack_push(status);
}
void CPU::op_pla(AddressingMode) {
  uint8_t data = stack_pop();
  set_register_a(data);
}
void CPU::op_plp(AddressingMode) {
  uint8_t data = stack_pop();
  status = data;
  set_flag(flags::BREAK, false);
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

  set_flag(flags::ZERO, and_value == 0);
  set_flag(flags::NEGATIVE, (value >> 7) == 1);
  set_flag(flags::OVERFLOW, (value >> 6) == 1);
}
// increment operations
void CPU::op_inc(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  value += 1;

  bus.mem_write(addr, value);
  set_flag(flags::ZERO, value == 0);
  set_flag(flags::NEGATIVE, (value >> 7) == 1);
}
void CPU::op_inx(AddressingMode) { set_register_x(reg_x + 1); }
void CPU::op_iny(AddressingMode) { set_register_y(reg_y + 1); }
// decrement operations
void CPU::op_dec(AddressingMode mode) {
  auto addr = get_addr(mode);
  uint8_t value = bus.mem_read(addr);
  value -= 1;

  bus.mem_write(addr, value);
  set_flag(flags::ZERO, value == 0);
  set_flag(flags::NEGATIVE, (value >> 7) == 1);
}
void CPU::op_dex(AddressingMode) { set_register_x(reg_x - 1); }
void CPU::op_dey(AddressingMode) { set_register_y(reg_y - 1); }
// shifts
void CPU::op_asl(AddressingMode mode) {
  uint8_t value = 0;
  std::optional<uint16_t> addr;

  if (mode == AddressingMode::Accumulator) {
    value = reg_a;
  } else {
    addr = get_addr(mode);
    value = bus.mem_read(addr.value());
  }

  set_flag(flags::CARRY, (value >> 7) == 1);
  value <<= 1;

  if (mode == AddressingMode::Accumulator) {
    set_register_a(value);
  } else {
    if (!addr.has_value()) {
      throw std::runtime_error("failed to extract address");
    }

    bus.mem_write(addr.value(), value);
    update_zero_and_negative_flags(value);
  }
}
// status flag changes
void CPU::op_clc(AddressingMode) { set_flag(flags::CARRY, false); }
void CPU::op_cld(AddressingMode) { set_flag(flags::DECIMAL_MODE, false); }
void CPU::op_cli(AddressingMode) { set_flag(flags::INTERRUPT_DISABLE, false); }
void CPU::op_clv(AddressingMode) { set_flag(flags::OVERFLOW, false); }
void CPU::op_sec(AddressingMode) { set_flag(flags::CARRY, true); }
void CPU::op_sed(AddressingMode) { set_flag(flags::DECIMAL_MODE, true); }
void CPU::op_sei(AddressingMode) { set_flag(flags::INTERRUPT_DISABLE, true); }
// system functions
void CPU::op_brk(AddressingMode) {
  pc += 1;
  stack_push(pc);
  stack_push(status);

  pc = bus.mem_read_u16(INTERRUPT_VECTOR);
  set_flag(flags::BREAK, true);
}
void CPU::op_nop(AddressingMode) {}
void CPU::op_rti(AddressingMode) {
  status = stack_pop();
  set_flag(flags::BREAK, false);
  pc = stack_pop_u16();
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
void CPU::set_flag(uint8_t mask, bool condition) {
  condition ? status |= mask : status &= ~mask;
}
void CPU::update_zero_and_negative_flags(uint8_t value) {
  set_flag(flags::ZERO, value == 0);
  set_flag(flags::NEGATIVE, (value >> 7) == 1);
}

// stack utils
void CPU::stack_push(uint8_t data) {
  bus.mem_write(memory_map::STACK_START + static_cast<uint16_t>(sp), data);
  sp -= 1;
}
uint8_t CPU::stack_pop() {
  sp += 1;
  return bus.mem_read(memory_map::STACK_START + static_cast<uint16_t>(sp));
}
uint16_t CPU::stack_pop_u16() {
  auto low = static_cast<uint16_t>(stack_pop());
  auto high = static_cast<uint16_t>(stack_pop());

  return (high << 8) | low;
}

// mem utils
uint8_t CPU::mem_read(uint16_t addr) { return bus.mem_read(addr); }
uint16_t CPU::mem_read_u16(uint16_t addr) { return bus.mem_read_u16(addr); }

// additional utils
uint8_t CPU::fetch_next_byte() { return bus.mem_read(pc++); }
uint16_t CPU::get_addr(AddressingMode mode) {
  switch (mode) {
  case Immediate:
    return pc++;
  case ZeroPage:
    return bus.mem_read(pc++);
  case ZeroPage_X:
    return bus.mem_read(pc++) + reg_x;
  case ZeroPage_Y:
    return bus.mem_read(pc++) + reg_y;
  case Absolute: {
    uint16_t addr = bus.mem_read_u16(pc);
    pc += 2;
    return addr;
  }
  case Absolute_X: {
    uint16_t addr = bus.mem_read_u16(pc);
    pc += 2;
    return addr;
  }
  case Absolute_Y: {
    uint16_t addr = bus.mem_read_u16(pc++) + static_cast<uint16_t>(reg_y);
    pc += 2;
    return addr;
  }
  case Indirect: {
    uint16_t ptr = bus.mem_read_u16(pc);
    pc += 2;
    return bus.mem_read_u16(ptr);
  }
  case Indirect_X: {
    uint8_t base = bus.mem_read(pc++);
    uint8_t zero_page_addr = (base + reg_x) & 0xFF;
    uint8_t low = bus.mem_read(zero_page_addr);
    uint8_t high = bus.mem_read((zero_page_addr + 1) & 0xFF);
    return (high << 8) | low;
  }
  case Indirect_Y: {
    uint8_t zero_page_addr = bus.mem_read(pc++);
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