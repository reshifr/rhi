#include "../tests.h"

void set_insert(struct rhifunc* func) {
  long start;
  struct rhis* set;
  struct objs* keys;

/* Consistency tests for `rhi_init` */
#if 0
  /* Fixed mode */
  HANDLE((set=rhis_init(func,
    RHI_FIXED))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN,
    CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);

  /* Extend mode */
  HANDLE((set=rhis_init(func,
    RHI_EXTEND))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN,
    CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);

  /* Shrink and extend mode */
  HANDLE((set=rhis_init(func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN,
    CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);
#endif

/* Consistency tests for `rhi_reserve` */
#if 0
  /* Reserve with fixed */
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(MIN_OBJLEN, MAX_OBJLEN,
    NUM_KEYS, OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);
#endif

/* Performance tests for `rhis_init` */
#if 1
  HANDLE((set=rhis_init(func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(PERF_MIN_OBJLEN, PERF_MAX_OBJLEN,
    PERF_NUM_KEYS, PERF_OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  start = get_clock();
  for(rhiuint i=0; i<keys->count; ++i)
    rhis_insert(set, keys->objs[i]);
  printf("Insert time in `rhi_init`: %ld ms\n", get_clock()-start);
  set_mprint(set);
  rhis_free(set);
  objs_destroy(keys);
#endif

/* Performance tests for `rhis_reserve` */
#if 1
  HANDLE((set=rhis_reserve(func, PERF_RESERVE_SIZE,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(PERF_MIN_OBJLEN, PERF_MAX_OBJLEN,
    PERF_NUM_KEYS, PERF_OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  start = get_clock();
  for(rhiuint i=0; i<keys->count; ++i)
    rhis_insert(set, keys->objs[i]);
  printf("Insert time in `rhis_reserve`: %ld ms\n", get_clock()-start);
  set_mprint(set);
  rhis_free(set);
  objs_destroy(keys);
#endif
}
