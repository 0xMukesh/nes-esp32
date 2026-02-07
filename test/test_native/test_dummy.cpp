#include <cstdio>
#include <unity.h>

void test_addition(void) {
  int result = 2 + 2;
  TEST_ASSERT_EQUAL(4, result);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_addition);
  UNITY_END();

  return 0;
}