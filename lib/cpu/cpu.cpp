#include "cpu.hpp"
#include "../constants.hpp"

CPU::CPU() : program_counter(0), register_a(0), status(0) {};

void CPU::interpret(std::vector<uint8_t> program) {
  program_counter = 0;

  while (true) {
    uint8_t opcode = program[program_counter];
    ++program_counter;

    switch (opcode) {
    case 0xa9:
      lda(program);
      break;
    case 0x00:
      return;
    }
  }
}

void CPU::lda(std::vector<uint8_t> program) {
  uint8_t param = program[program_counter];
  ++program_counter;

  register_a = param;

  if (register_a == 0) {
    status |= flags::ZERO;
  } else {
    status &= ~flags::ZERO;
  }

  if ((register_a & 0b10000000) != 0) {
    status |= flags::NEGATIVE;
  } else {
    status &= ~flags::NEGATIVE;
  }

  return;
}
