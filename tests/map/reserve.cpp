#include "../utils.hpp"

static void unit_test(int mode) {
  struct rhim* map;
  HANDLE((map=rhim_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhim_reserve() failed.");
  utils::print(map);
  rhim_free(map);
}

static void unit_test(rhiuint size, int mode) {
  struct rhim* map;
  HANDLE((map=rhim_reserve(utils::hash, utils::equal,
    size, mode))==NULL, "Error: rhim_reserve() failed.");
  utils::mprint(map);
  rhim_free(map);
}

void map::reserve(void) {
  TEST(1, unit_test(RHI_FIXED));
  TEST(1, unit_test(RHI_FIXED));
  TEST(1, unit_test(RHI_SHRINK));
  TEST(1, unit_test(RHI_EXTEND));
  TEST(1, unit_test(RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(100, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(10000, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(1000000, RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_test(100000000, RHI_SHRINK|RHI_EXTEND));
}
