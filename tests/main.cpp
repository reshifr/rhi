#include "utils.hpp"

int main(void) {
  TEST(0, set::init());
  TEST(0, set::reserve());
  TEST(1, set::insert());
  return EXIT_SUCCESS;
}
