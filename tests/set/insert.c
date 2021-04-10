#include "../tests.h"

/* Using `rhi_init` */
#define CONS_INIT_TESTS(func, mode, print) \
  do { \
    struct rhis* set; \
    struct objs* keys[2]; \
 \
    /* Initial set */ \
    HANDLE((set=rhis_init(func, mode))==NULL, "Set init failed."); \
 \
    /* Initial list of keys 1 */ \
    HANDLE((keys[0]=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN, \
      CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "Keys init 1 failed."); \
    /* Initial list of keys 2 */ \
    HANDLE((keys[1]=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN, \
      CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "Keys init 2 failed."); \
 \
    /* Insert keys 1 */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) \
      ASSERT(!rhis_insert(set, keys[0]->objs[i]), \
        "Insert %s 1 failed.", (const char*)keys[0]->objs[i]); \
    print(set); \
    /* Insert keys 2 */ \
    for(rhiuint i=0; i<keys[1]->count; ++i) \
      ASSERT(!rhis_insert(set, keys[1]->objs[i]), \
        "Insert %s 2 failed.", (const char*)keys[1]->objs[i]); \
    print(set); \
 \
    /* Insert NULL 1 */ \
    ASSERT(!rhis_insert(set, NULL), "Insert %s 1 failed.", "NULL"); \
    /* Insert NULL 2*/ \
    ASSERT(!rhis_insert(set, NULL), "Insert %s 2 failed.", "NULL"); \
 \
    print(set); \
    rhis_free(set); \
    objs_destroy(keys[0]); \
    objs_destroy(keys[1]); \
  } while(0)

/* Using `rhi_reserve` */
#define CONS_RESERVE_TESTS(func, mode, print) \
  do { \
    struct rhis* set; \
    struct objs* keys[2]; \
 \
    /* Reserve set */ \
    HANDLE((set=rhis_reserve(func, \
      CONS_RESERVE_SIZE, mode))==NULL, "Set init failed."); \
 \
    /* Initial list of keys 1 */ \
    HANDLE((keys[0]=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN, \
      CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "Keys init 1 failed."); \
    /* Initial list of keys 2 */ \
    HANDLE((keys[1]=objs_init(CONS_MIN_OBJLEN, CONS_MAX_OBJLEN, \
      CONS_NUM_KEYS, CONS_OBJCASE))==NULL, "Keys init 2 failed."); \
 \
    /* Insert keys 1 */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) \
      ASSERT(!rhis_insert(set, keys[0]->objs[i]), \
        "Insert %s 1 failed.", (const char*)keys[0]->objs[i]); \
    print(set); \
    /* Insert keys 2 */ \
    for(rhiuint i=0; i<keys[1]->count; ++i) \
      ASSERT(!rhis_insert(set, keys[1]->objs[i]), \
        "Insert %s 2 failed.", (const char*)keys[1]->objs[i]); \
    print(set); \
 \
    /* Insert NULL 1 */ \
    ASSERT(!rhis_insert(set, NULL), "Insert %s 1 failed.", "NULL"); \
    /* Insert NULL 2*/ \
    ASSERT(!rhis_insert(set, NULL), "Insert %s 2 failed.", "NULL"); \
 \
    print(set); \
    rhis_free(set); \
    objs_destroy(keys[0]); \
    objs_destroy(keys[1]); \
  } while(0)

/* Using `rhi_init` */
#define PERF_INIT_TESTS(func, mode, print) \
  do { \
    long start; \
    struct rhis* set; \
    struct objs* keys; \
 \
    /* Initial set */ \
    HANDLE((set=rhis_init(func, mode))==NULL, "Set init failed."); \
 \
    /* Initial list of keys */ \
    HANDLE((keys=objs_init(PERF_MIN_OBJLEN, PERF_MAX_OBJLEN, \
      PERF_NUM_KEYS, PERF_OBJCASE))==NULL, "Keys init failed."); \
 \
    /* Insert keys */ \
    start = get_clock(); \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_insert(set, keys->objs[i]); \
 \
    printf("Insert time in `rhi_init`: %ld ms\n", get_clock()-start); \
    print(set); \
    rhis_free(set); \
    objs_destroy(keys); \
  } while(0)

/* Using `rhi_reserve` */
#define PERF_RESERVE_TESTS(func, mode, print) \
  do { \
    long start; \
    struct rhis* set; \
    struct objs* keys; \
 \
    /* Initial set */ \
    HANDLE((set=rhis_reserve(func, \
      PERF_RESERVE_SIZE, mode))==NULL, "Set init failed."); \
 \
    /* Initial list of keys */ \
    HANDLE((keys=objs_init(PERF_MIN_OBJLEN, PERF_MAX_OBJLEN, \
      PERF_NUM_KEYS, PERF_OBJCASE))==NULL, "Keys init failed."); \
 \
    /* Insert keys */ \
    start = get_clock(); \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_insert(set, keys->objs[i]); \
 \
    printf("Insert time in `rhi_init`: %ld ms\n", get_clock()-start); \
    print(set); \
    rhis_free(set); \
    objs_destroy(keys); \
  } while(0)

extern struct rhifunc func;

void set_insert(void) {
  TEST(CONS_INIT_TESTS(&func, RHI_FIXED, set_print), 0);
  TEST(CONS_INIT_TESTS(&func, RHI_SHRINK, set_print), 0);
  TEST(CONS_INIT_TESTS(&func, RHI_EXTEND, set_print), 0);
  TEST(CONS_INIT_TESTS(&func, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  TEST(CONS_RESERVE_TESTS(&func, RHI_FIXED, set_print), 0);
  TEST(CONS_RESERVE_TESTS(&func, RHI_SHRINK, set_print), 0);
  TEST(CONS_RESERVE_TESTS(&func, RHI_EXTEND, set_print), 0);
  TEST(CONS_RESERVE_TESTS(&func, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  TEST(PERF_INIT_TESTS(&func, RHI_FIXED, set_mprint), 0);
  TEST(PERF_INIT_TESTS(&func, RHI_SHRINK, set_mprint), 0);
  TEST(PERF_INIT_TESTS(&func, RHI_EXTEND, set_mprint), 0);
  TEST(PERF_INIT_TESTS(&func, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);

  TEST(PERF_RESERVE_TESTS(&func, RHI_FIXED, set_mprint), 0);
  TEST(PERF_RESERVE_TESTS(&func, RHI_SHRINK, set_mprint), 0);
  TEST(PERF_RESERVE_TESTS(&func, RHI_EXTEND, set_mprint), 0);
  TEST(PERF_RESERVE_TESTS(&func, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);
}
