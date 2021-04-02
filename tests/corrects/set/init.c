#include "../corrects.h"

void set_init(struct rhifunc func) {
  struct rhis* set;

  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  HANDLE((set=rhis_init(&func,
    RHI_SHRINK))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  HANDLE((set=rhis_init(&func,
    RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);

  HANDLE((set=rhis_init(&func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  set_print(set);
  rhis_free(set);
}

