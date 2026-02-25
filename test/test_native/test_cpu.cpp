#include "../lib/cpu/cpu.hpp"
#include <cstdint>
#include <unity.h>
#include <vector>

const char *REGISTER_A_MISMATCH = "register A mismatch";
const char *REGISTER_X_MISMATCH = "register X mismatch";
const char *REGISTER_Y_MISMATCH = "register Y mismatch";
const char *STATUS_MISMATCH = "status mismatch";
const char *MEMORY_VALUE_MISMATCH = "memory value mismatch";

CPU simulate_program(std::vector<uint8_t> program) {
  CPU cpu;
  cpu.load_program(program);

  for (auto _ : program) {
    cpu.step();
  }

  return cpu;
}

// -- LDA
void test_load_acc_immediate() {
  auto cpu = simulate_program({0xa9, 0x01, // loads 0x01 into register A
                               0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::UNUSED | flags::BREAK, cpu.get_status(),
                            STATUS_MISMATCH);
}
void test_load_acc_zero_flag() {
  auto cpu = simulate_program({0xa9, 0x00, // loads 0x00 into register A
                               0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x00, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::ZERO, cpu.get_status(), STATUS_MISMATCH);
}
void test_load_acc_negative_flag() {
  auto cpu = simulate_program(
      {0xa9,
       static_cast<uint8_t>(
           1 - 2), // loads -1 (aka 255 in 8-bit system) into register A
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(static_cast<uint8_t>(1 - 2), cpu.get_reg_a(),
                            REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::NEGATIVE, cpu.get_status(), STATUS_MISMATCH);
}

// -- LDX
void test_load_reg_x_immediate() {
  auto cpu = simulate_program({0xa2, 0x01, // loads 0x01 into register X
                               0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- LDY
void test_load_reg_y_immediate() {
  auto cpu = simulate_program({0xa0, 0x01, // loads 0x01 into register Y
                               0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_y(), REGISTER_Y_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- STA
void test_store_acc_zero_page() {
  auto cpu = simulate_program(
      {0xa9, 0x01, // loads 0x01 into register A
       0x85, 0x05, // stores value present in register A at $0x05
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.mem_read(0x05), MEMORY_VALUE_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}
void test_store_acc_zero_page_x() {
  auto cpu = simulate_program(
      {0xa9, 0x01, // loads 0x01 into register A
       0xa2, 0x02, // loads 0x02 into register X
       0x95, 0x05, // stores value present in register A at $(0x05 + 0x02)
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x02, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.mem_read(0x05 + 0x02),
                            MEMORY_VALUE_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- STX
void test_store_reg_x_zero_page() {
  auto cpu = simulate_program(
      {0xa2, 0x01, // loads 0x02 into register X
       0x86, 0x05, // stores value present in register X at $0x05
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.mem_read(0x05), MEMORY_VALUE_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}
void test_store_reg_x_zero_page_y() {
  auto cpu = simulate_program(
      {0xa2, 0x01, // loads 0x01 into register X
       0xa0, 0x02, // loads 0x02 into register Y
       0x96, 0x05, // stores value present in register X at $(0x05 + 0x02)
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x02, cpu.get_reg_y(), REGISTER_Y_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.mem_read(0x05 + 0x02),
                            MEMORY_VALUE_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- STY
void test_store_reg_y_zero_page() {
  auto cpu = simulate_program(
      {0xa0, 0x01, // loads 0x01 into register Y
       0x84, 0x05, // stores value present in register Y at $0x05
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_y(), REGISTER_Y_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.mem_read(0x05), MEMORY_VALUE_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}
void test_store_reg_y_zero_page_x() {
  auto cpu = simulate_program(
      {0xa0, 0x01, // loads 0x01 into register Y
       0xa2, 0x02, // loads 0x02 into register X
       0x94, 0x05, // stores value present in register Y at $(0x05 + 0x02)
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_y(), REGISTER_Y_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x02, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.mem_read(0x05 + 0x02),
                            MEMORY_VALUE_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- TAX
void test_transfer_acc_to_reg_x() {
  auto cpu =
      simulate_program({0xa9, 0x01, // loads 0x01 into register A
                        0xaa, // transfers value present in register A to X
                        0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}
void test_transfer_acc_zero_flag() {
  auto cpu =
      simulate_program({0xa9, 0x00, // loads 0x00 into register A
                        0xaa, // transfers value present in register A to X
                        0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x00, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x00, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::ZERO, cpu.get_status(), STATUS_MISMATCH);
}
void test_transfer_acc_negative_flag() {
  auto cpu = simulate_program(
      {0xa9, static_cast<uint8_t>(1 - 2), // loads 255 into register A
       0xaa, // transfers value present in register A to X
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(255, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(255, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::NEGATIVE, cpu.get_status(), STATUS_MISMATCH);
}

// -- TAY
void test_transfer_acc_to_reg_y() {
  auto cpu =
      simulate_program({0xa9, 0x01, // loads 0x01 into register A
                        0xa8, // transfers value present in register A to Y
                        0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_y(), REGISTER_Y_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- TXA
void test_transfer_reg_x_to_acc() {
  auto cpu =
      simulate_program({0xa2, 0x01, // loads 0x01 into register X
                        0x8a, // transfers value present in register X to A
                        0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_x(), REGISTER_X_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- TYA
void test_transfer_reg_y_to_acc() {
  auto cpu =
      simulate_program({0xa0, 0x01, // loads 0x01 into register Y
                        0x98, // transfers value present in register Y to A
                        0x00});
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_y(), REGISTER_Y_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0x01, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(0, cpu.get_status(), STATUS_MISMATCH);
}

// -- BIT
void test_bit_test() {
  uint8_t data = 0b11000001;
  auto cpu = simulate_program(
      {0xa9, 0b11000001, // loads value into register A
       0x85, 0x60,       // dumps value of register A at $0x60
       0x24, 0x60,       // performs bit test with value stored at $0x60 as mask
       0x00});
  TEST_ASSERT_EQUAL_MESSAGE(data, cpu.get_reg_a(), REGISTER_A_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(data, cpu.mem_read(0x60), MEMORY_VALUE_MISMATCH);
  TEST_ASSERT_EQUAL_MESSAGE(flags::NEGATIVE | flags::OVERFLOW, cpu.get_status(),
                            STATUS_MISMATCH);
}

int main() {
  UNITY_BEGIN();
  // load operations
  RUN_TEST(test_load_acc_immediate);
  RUN_TEST(test_load_acc_zero_flag);
  RUN_TEST(test_load_acc_negative_flag);
  RUN_TEST(test_load_reg_x_immediate);
  RUN_TEST(test_load_reg_y_immediate);

  // store operations
  RUN_TEST(test_store_acc_zero_page);
  RUN_TEST(test_store_acc_zero_page_x);
  RUN_TEST(test_store_reg_x_zero_page);
  RUN_TEST(test_store_reg_x_zero_page_y);
  RUN_TEST(test_store_reg_y_zero_page);
  RUN_TEST(test_store_reg_y_zero_page_x);

  // register transfers
  RUN_TEST(test_transfer_acc_to_reg_x);
  RUN_TEST(test_transfer_acc_zero_flag);
  RUN_TEST(test_transfer_acc_negative_flag);
  RUN_TEST(test_transfer_acc_to_reg_y);
  RUN_TEST(test_transfer_reg_x_to_acc);
  RUN_TEST(test_transfer_reg_y_to_acc);

  // logical operations
  RUN_TEST(test_bit_test);

  UNITY_END();

  return 0;
}