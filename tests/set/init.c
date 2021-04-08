#include "../corrects.h"

void set_init(struct rhifunc func) {
  struct rhis* set;

#if 1
  /* Initial with fixed */
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Initial with shrink */
  HANDLE((set=rhis_init(&func,
    RHI_SHRINK))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Initial with extend */
  HANDLE((set=rhis_init(&func,
    RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  /* Initial with shrink and extend */
  HANDLE((set=rhis_init(&func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);
#endif
}
