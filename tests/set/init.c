#include "../tests.h"

#define CONS_TESTS(func, mode, print) \
  do { \
    struct rhis* set; \
    HANDLE((set=rhis_init(func, mode))==NULL, "Set init failed."); \
    print(set); \
    rhis_free(set); \
  } while(0)

extern struct rhifunc func;

void set_init(void) {
  TEST(CONS_TESTS(&func, RHI_FIXED, set_print), 1);
  TEST(CONS_TESTS(&func, RHI_SHRINK, set_print), 1);
  TEST(CONS_TESTS(&func, RHI_EXTEND, set_print), 1);
  TEST(CONS_TESTS(&func, RHI_SHRINK|RHI_EXTEND, set_print), 1);
}
