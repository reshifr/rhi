#include "../tests.h"

extern struct rhifunc func;

static void cons_tests(void) {
  struct rhis* set;

  /* Fixed mode */
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Shrink mode */
  HANDLE((set=rhis_init(&func,
    RHI_SHRINK))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Extend mode */
  HANDLE((set=rhis_init(&func,
    RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Shrink and extend mode */
  HANDLE((set=rhis_init(&func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);
}

void set_init(void) {
  cons_tests();
}
