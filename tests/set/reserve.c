#include "../tests.h"

#define CONS_TESTS(func, size, mode, print) \
  do { \
    struct rhis* set; \
    HANDLE((set=rhis_reserve(func, \
      size, mode))==NULL, "Set reserve failed."); \
    print(set); \
    rhis_free(set); \
  } while(0)

extern struct rhifunc func;

void set_reserve(void) {
  TEST(CONS_TESTS(&func, CONS_RESERVE_SIZE, RHI_FIXED, set_print), 1);
  TEST(CONS_TESTS(&func, CONS_RESERVE_SIZE, RHI_SHRINK, set_print), 1);
  TEST(CONS_TESTS(&func, CONS_RESERVE_SIZE, RHI_EXTEND, set_print), 1);
  TEST(CONS_TESTS(&func, CONS_RESERVE_SIZE, RHI_SHRINK|RHI_EXTEND, set_print), 1);

  TEST(CONS_TESTS(&func, 1000, RHI_SHRINK|RHI_EXTEND, set_mprint), 1);
  TEST(CONS_TESTS(&func, 1000000, RHI_SHRINK|RHI_EXTEND, set_mprint), 1);
  TEST(CONS_TESTS(&func, 100000000, RHI_SHRINK|RHI_EXTEND, set_mprint), 1);
}
