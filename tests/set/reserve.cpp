#include "../utils.hpp"

static void unit_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::print(set);
  rhis_free(set);
}

static void unit_test(rhiuint size, int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    size, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::mprint(set);
  rhis_free(set);
}

void set::unit_reserve(void) {
  TEST(1, unit_test(RHI_FIXED));
  TEST(1, unit_test(RHI_FIXED));
  TEST(1, unit_test(RHI_SHRINK));
  TEST(1, unit_test(RHI_EXTEND));
  TEST(1, unit_test(RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(0, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(1, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(100, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(10000, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(1000000, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(100000000, RHI_SHRINK|RHI_EXTEND));
}
