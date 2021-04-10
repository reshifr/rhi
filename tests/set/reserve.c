#include "../tests.h"

extern struct rhifunc func;

static void cons_tests(void) {
  struct rhis* set;

#if 1
  /* Fixed mode */
  HANDLE((set=rhis_reserve(&func, CONS_RESERVE_SIZE,
    RHI_FIXED))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);

  /* Shrink mode */
  HANDLE((set=rhis_reserve(&func, CONS_RESERVE_SIZE,
    RHI_SHRINK))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
  
  /* Extend mode*/
  HANDLE((set=rhis_reserve(&func, CONS_RESERVE_SIZE,
    RHI_EXTEND))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
  
  /* Shrink and extend mode */
  HANDLE((set=rhis_reserve(&func, CONS_RESERVE_SIZE,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_print(set);
  rhis_free(set);
#endif

#if 1
  /* At least 1000 */
  HANDLE((set=rhis_reserve(&func, 1000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);

  /* At least 1000_000 */
  HANDLE((set=rhis_reserve(&func, 1000000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);

  /* At least 100_000_000 */
  HANDLE((set=rhis_reserve(&func, 100000000,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set reserve failed.");
  set_mprint(set);
  rhis_free(set);
#endif
}

void set_reserve(void) {
  cons_tests();
}
