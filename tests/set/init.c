#include "../tests.h"

void set_init(struct rhifunc* func) {
  struct rhis* set;

/* Consistency tests */
#if 1
  /* Fixed mode */
  HANDLE((set=rhis_init(func,
    RHI_FIXED))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Shrink mode */
  HANDLE((set=rhis_init(func,
    RHI_SHRINK))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Extend mode */
  HANDLE((set=rhis_init(func,
    RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Shrink and extend mode */
  HANDLE((set=rhis_init(func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);
#endif
}
