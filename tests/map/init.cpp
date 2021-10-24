#include "../utils.hpp"

static void unit_test(int mode) {
  struct rhim* map;
  HANDLE((map=rhim_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhim_init() failed.");
  utils::print(map);
  rhim_free(map);
}

void map::init(void) {
  TEST(1, unit_test(RHI_FIXED));
  TEST(1, unit_test(RHI_SHRINK));
  TEST(1, unit_test(RHI_EXTEND));
  TEST(1, unit_test(RHI_SHRINK|RHI_EXTEND));
}
