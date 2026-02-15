#pragma once

#include <cstdint>
#include <vector>

class CPU {
public:
  CPU();
  void interpret(std::vector<uint8_t>);

  // registers
  uint16_t program_counter;
  uint8_t register_a;
  uint8_t status;

private:
  // instruction handlers
  void lda(std::vector<uint8_t>);
};
