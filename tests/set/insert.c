#include "../tests.h"

#define INIT 0
#define RESERVE 1

#define CONS_TESTS(initial, func, mode, print) \
  do { \
    bool r; \
    struct rhis* set; \
    struct objs* keys[2]; \
 \
    /* initial set */ \
    if( initial==INIT ) { \
      /* using `rhi_init` */ \
      HANDLE((set=rhis_init(func, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using `rhi_reserve` */ \
      HANDLE((set=rhis_reserve(func, \
        CONS_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    /* [1] initial list of keys */ \
    HANDLE((keys[0]=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN, \
      CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "[1] Keys init failed."); \
    /* [2] initial list of keys */ \
    HANDLE((keys[1]=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN, \
      CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "[2] Keys init failed."); \
 \
    /* [1] insert keys */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) { \
      r = rhis_insert(set, keys[0]->objs[i]); \
      ASSERT(r, "[1] Insert %s success.", (const char*)keys[0]->objs[i]); \
      ASSERT(!r, "[1] Insert %s failed.", (const char*)keys[0]->objs[i]); \
    } \
    print(set); \
    /* [2] insert keys */ \
    for(rhiuint i=0; i<keys[1]->count; ++i) { \
      r = rhis_insert(set, keys[1]->objs[i]); \
      ASSERT(r, "[2] Insert %s success.", (const char*)keys[1]->objs[i]); \
      ASSERT(!r, "[2] Insert %s failed.", (const char*)keys[1]->objs[i]); \
    } \
    print(set); \
 \
    /* [1] insert `NULL` */ \
    r = rhis_insert(set, NULL); \
    ASSERT(r, "[1] Insert %s success.", "NULL"); \
    ASSERT(!r, "[1] Insert %s failed.", "NULL"); \
    /* [2] insert `NULL` */ \
    r = rhis_insert(set, NULL); \
    ASSERT(r, "[2] Insert %s success.", "NULL"); \
    ASSERT(!r, "[2] Insert %s failed.", "NULL"); \
 \
    print(set); \
    rhis_free(set); \
    objs_destroy(keys[0]); \
    objs_destroy(keys[1]); \
  } while(0)

#define PERF_TESTS(initial, func, mode, print) \
  do { \
    long start; \
    struct rhis* set; \
    struct objs* keys; \
 \
    /* initial set */ \
    if( initial==INIT ) { \
      /* using `rhi_init` */ \
      HANDLE((set=rhis_init(func, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using `rhi_reserve` */ \
      HANDLE((set=rhis_reserve(func, \
        PERF_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    /* initial list of keys */ \
    HANDLE((keys=objs_init(PERF_MIN_OBJLEN, PERF_MAX_OBJLEN, \
      PERF_NUM_KEYS, PERF_OBJCASE))==NULL, "Keys init failed."); \
 \
    /* insert keys */ \
    start = get_clock(); \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_insert(set, keys->objs[i]); \
 \
    printf("Insert time: %ld ms\n", get_clock()-start); \
    print(set); \
    rhis_free(set); \
    objs_destroy(keys); \
  } while(0)

extern struct rhifunc func;

void set_insert(void) {
  TEST(CONS_TESTS(INIT, &func, RHI_FIXED, set_print), 0);
  TEST(CONS_TESTS(INIT, &func, RHI_SHRINK, set_print), 0);
  TEST(CONS_TESTS(INIT, &func, RHI_EXTEND, set_print), 0);
  TEST(CONS_TESTS(INIT, &func, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  TEST(CONS_TESTS(RESERVE, &func, RHI_FIXED, set_print), 0);
  TEST(CONS_TESTS(RESERVE, &func, RHI_SHRINK, set_print), 0);
  TEST(CONS_TESTS(RESERVE, &func, RHI_EXTEND, set_print), 0);
  TEST(CONS_TESTS(RESERVE, &func, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  TEST(PERF_TESTS(INIT, &func, RHI_FIXED, set_mprint), 0);
  TEST(PERF_TESTS(INIT, &func, RHI_SHRINK, set_mprint), 0);
  TEST(PERF_TESTS(INIT, &func, RHI_EXTEND, set_mprint), 0);
  TEST(PERF_TESTS(INIT, &func, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);

  TEST(PERF_TESTS(RESERVE, &func, RHI_FIXED, set_mprint), 0);
  TEST(PERF_TESTS(RESERVE, &func, RHI_SHRINK, set_mprint), 0);
  TEST(PERF_TESTS(RESERVE, &func, RHI_EXTEND, set_mprint), 0);
  TEST(PERF_TESTS(RESERVE, &func, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);
}
