#include "../tests.h"

#define CONS_TESTS(funcs, mode, print) \
  do { \
    struct rhis* set; \
    HANDLE((set=rhis_init(funcs.hash, funcs.equal, \
      funcs.keyfree, mode))==NULL, "Set init failed."); \
    print(set); \
  } while(0)

extern struct funcs funcs;

void set_init(void) {
  TEST(CONS_TESTS(funcs, RHI_FIXED, set_print), 1);
  TEST(CONS_TESTS(funcs, RHI_SHRINK, set_print), 1);
  TEST(CONS_TESTS(funcs, RHI_EXTEND, set_print), 1);
  TEST(CONS_TESTS(funcs, RHI_SHRINK|RHI_EXTEND, set_print), 1);
}
