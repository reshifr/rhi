#include "../utils.hpp"

static void unit_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::print(set);
  rhis_free(set);
}

void set::init(void) {
  TEST(1, unit_test(RHI_FIXED));
  TEST(1, unit_test(RHI_SHRINK));
  TEST(1, unit_test(RHI_EXTEND));
  TEST(1, unit_test(RHI_SHRINK|RHI_EXTEND));
}
