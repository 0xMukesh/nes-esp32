#include "../../lib/constants.hpp"
#include "../../lib/cpu/cpu.hpp"
#include <bitset>
#include <cstdint>
#include <iostream>
#include <tuple>
#include <unity.h>
#include <vector>

void test_lda(void) {
  // program, register a, status
  auto tests = std::vector<std::tuple<std::vector<uint8_t>, uint8_t, uint8_t>>{
      {{0xa9, 0x05, 0x00}, 0x05, 0},
      {{0xa9, 0x00, 0x00}, 0x00, 0 | flags::ZERO},
      {{0xa9, static_cast<uint8_t>(1 - 2), 0x00},
       static_cast<uint8_t>(1 - 2),
       0 | flags::NEGATIVE}};

  for (auto t : tests) {
    CPU cpu;
    auto program = std::get<0>(t);
    auto register_a = std::get<1>(t);
    auto status = std::get<2>(t);

    cpu.interpret(program);

    std::cout << "status: " << std::bitset<8>(cpu.status) << std::endl;
    TEST_ASSERT_EQUAL(register_a, cpu.register_a);
  }
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_lda);
  UNITY_END();

  return 0;
}