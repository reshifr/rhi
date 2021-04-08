#include "../corrects.h"

void set_reserve(struct rhifunc func) {
  struct rhis* set;

#if 0
  HANDLE((set=rhis_reserve(&func, 10,
    RHI_FIXED))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
  
  HANDLE((set=rhis_reserve(&func, 10,
    RHI_SHRINK))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
  
  HANDLE((set=rhis_reserve(&func, 10,
    RHI_EXTEND))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
  
  HANDLE((set=rhis_reserve(&func, 10,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
#endif

#if 1
  HANDLE((set=rhis_reserve(&func, 100,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);

  HANDLE((set=rhis_reserve(&func, 1000000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);

  HANDLE((set=rhis_reserve(&func, 500000000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);
#endif
}
