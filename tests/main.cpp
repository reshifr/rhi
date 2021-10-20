#include "utils.hpp"

int main(void) {
  TEST(0, set::init());
  TEST(0, set::reserve());
  TEST(0, set::insert());
  return EXIT_SUCCESS;
}
