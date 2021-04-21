#include "../tests.h"

#define CONS_TESTS(funcs, size, mode, print) \
  do { \
    struct rhis* set; \
    HANDLE((set=rhis_reserve(funcs.hash, funcs.equal, \
      funcs.keyfree, size, mode))==NULL, "Set reserve failed."); \
    print(set); \
  } while(0)

extern struct funcs funcs;

void set_reserve(void) {
  TEST(CONS_TESTS(funcs, CONS_RESERVE_SIZE, RHI_FIXED, set_print), 1);
  TEST(CONS_TESTS(funcs, CONS_RESERVE_SIZE, RHI_SHRINK, set_print), 1);
  TEST(CONS_TESTS(funcs, CONS_RESERVE_SIZE, RHI_EXTEND, set_print), 1);
  TEST(CONS_TESTS(funcs, CONS_RESERVE_SIZE, RHI_SHRINK|RHI_EXTEND, set_print), 1);

  TEST(CONS_TESTS(funcs, 1000, RHI_SHRINK|RHI_EXTEND, set_mprint), 1);
  TEST(CONS_TESTS(funcs, 1000000, RHI_SHRINK|RHI_EXTEND, set_mprint), 1);
  TEST(CONS_TESTS(funcs, 100000000, RHI_SHRINK|RHI_EXTEND, set_mprint), 1);
}
