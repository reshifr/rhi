#include "utils.hpp"

int main(void) {
  TEST(1, set::unit_init());
  TEST(1, set::unit_reserve());
  TEST(1, set::unit_insert());
  TEST(1, set::auto_insert());
  TEST(1, set::perf_insert());
  return EXIT_SUCCESS;
}
