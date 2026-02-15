#include "../lib/cpu/cpu.hpp"
#include "constants.hpp"
#include <cstdint>
#include <unity.h>
#include <vector>

void test_lda_immediate() {
  auto program = std::vector<uint8_t>{0xa9, 0x01, 0x00};
  CPU cpu;

  cpu.load_program(program);
  cpu.step();

  TEST_ASSERT_EQUAL(0x01, cpu.get_reg_a());
  TEST_ASSERT_EQUAL(0, cpu.get_status());
}

void test_lda_zero_flag() {
  auto program = std::vector<uint8_t>{0xa9, 0x0, 0x00};
  CPU cpu;
  cpu.load_program(program);
  cpu.step();

  TEST_ASSERT_EQUAL(0, cpu.get_reg_a());
  TEST_ASSERT_EQUAL(flags::ZERO, cpu.get_status());
}

void test_lda_negative_flag() {
  auto program = std::vector<uint8_t>{0xa9, static_cast<uint8_t>(1 - 2), 0x00};
  CPU cpu;
  cpu.load_program(program);
  cpu.step();

  TEST_ASSERT_EQUAL(static_cast<uint8_t>(1 - 2), cpu.get_reg_a());
  TEST_ASSERT_EQUAL(flags::NEGATIVE, cpu.get_status());
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_lda_immediate);
  RUN_TEST(test_lda_zero_flag);
  RUN_TEST(test_lda_negative_flag);
  UNITY_END();

  return 0;
}