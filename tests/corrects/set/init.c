#include "../corrects.h"

void set_init(struct rhifunc func) {
  struct rhis* set;

#if 1
  /* Initial set with fixed */
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Initial set with shrink */
  HANDLE((set=rhis_init(&func,
    RHI_SHRINK))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Initial set with extend */
  HANDLE((set=rhis_init(&func,
    RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Initial set with shrink and extend */
  HANDLE((set=rhis_init(&func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);
#endif
}

