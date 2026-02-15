#include "../lib/cpu/cpu.hpp"
#include "constants.hpp"
#include <cstdint>
#include <unity.h>
#include <vector>

const char *REGISTER_A_MISMATCH = "register A mismatch";
const char *REGISTER_X_MISMATCH = "register X mismatch";
const char *STATUS_MISMATCH = "status mismatch";

CPU simulate_program(std::vector<uint8_t> program) {
  CPU cpu;
  cpu.load_program(program);

  while (cpu.mem_read(cpu.get_pc()) != 0x00) {
    cpu.step();
  }

  return cpu;
}

void test_lda_immediate() {
  auto cpu = simulate_program({0xa9, 0x01, 0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

void test_lda_zero_flag() {
  auto cpu = simulate_program({0xa9, 0x0, 0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::ZERO, cpu.get_status(), STATUS_MISMATCH);
}

void test_lda_negative_flag() {
  auto cpu = simulate_program({0xa9, static_cast<uint8_t>(1 - 2), 0x00});
  TEST_ASSERT_EQUAL_MESSAGE(static_cast<uint8_t>(1 - 2), cpu.get_reg_a(),
                            REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::NEGATIVE, cpu.get_status(), STATUS_MISMATCH);
}

void test_tax() {
  auto cpu = simulate_program({0xa9, 0x01, 0xaa, 0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

void test_inx() {
  auto cpu = simulate_program({0xa9, 0x01, 0xaa, 0xe8, 0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x02, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

void test_inx_overflow() {
  auto cpu = simulate_program({0xa9, 0xff, 0xaa, 0xe8, 0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0xff, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x0, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::ZERO, cpu.get_status(), STATUS_MISMATCH);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_lda_immediate);
  RUN_TEST(test_lda_zero_flag);
  RUN_TEST(test_lda_negative_flag);
  RUN_TEST(test_tax);
  RUN_TEST(test_inx);
  RUN_TEST(test_inx_overflow);
  UNITY_END();

  return 0;
}