#include "../corrects.h"

void set_reserve(struct rhifunc func) {
  struct rhis* set;

#if 1
  /* Reserve with fixed */
  HANDLE((set=rhis_reserve(&func, RESERVE_SIZE,
    RHI_FIXED))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);

  /* Reserve with shrink */
  HANDLE((set=rhis_reserve(&func, RESERVE_SIZE,
    RHI_SHRINK))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
  
  /* Reserve with extend */
  HANDLE((set=rhis_reserve(&func, RESERVE_SIZE,
    RHI_EXTEND))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
  
  /* Reserve with shrink and extend */
  HANDLE((set=rhis_reserve(&func, RESERVE_SIZE,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
#endif

#if 1
  /* Reserve at least 1000 */
  HANDLE((set=rhis_reserve(&func, 1000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);

  /* Reserve at least 1000_000 */
  HANDLE((set=rhis_reserve(&func, 1000000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);

  /* Reserve at least 100_000_000 */
  HANDLE((set=rhis_reserve(&func, 100000000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);
#endif
}
