#include "../tests.h"

#define INIT 0
#define RESERVE 1

#define CONS_TESTS(initial, funcs, mode, print) \
  do { \
    bool r; \
    struct rhis* set; \
    struct objs* keys[2]; \
 \
    /* initial set */ \
    if( initial==INIT ) { \
      /* using `rhi_init` */ \
      HANDLE((set=rhis_init(funcs.hash, funcs.equal, \
        funcs.keyfree, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using `rhi_reserve` */ \
      HANDLE((set=rhis_reserve(funcs.hash, funcs.equal, funcs.keyfree, \
        CONS_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    /* [1] initial list of keys */ \
    keys[0] = objs_init(CONS_MIN_OBJLEN, \
      CONS_MAX_OBJLEN, CONS_NUM_KEYS, CONS_OBJCASE); \
 \
    /* [2] initial list of keys */ \
    keys[1] = objs_init(CONS_MIN_OBJLEN, \
      CONS_MAX_OBJLEN, CONS_NUM_KEYS, CONS_OBJCASE); \
 \
    /* [1] replace keys */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) { \
      r = rhis_replace(set, keys[0]->objs[i]); \
      ASSERT(r, "[1] Replace %s success.", (const char*)keys[0]->objs[i]); \
      ASSERT(!r, "[1] Replace %s failed.", (const char*)keys[0]->objs[i]); \
    } \
    print(set); \
 \
    /* [2] replace keys */ \
    for(rhiuint i=0; i<keys[1]->count; ++i) { \
      r = rhis_replace(set, keys[1]->objs[i]); \
      ASSERT(r, "[2] Replace %s success.", (const char*)keys[1]->objs[i]); \
      ASSERT(!r, "[2] Replace %s failed.", (const char*)keys[1]->objs[i]); \
    } \
    print(set); \
 \
    /* [1] replace `NULL` */ \
    r = rhis_replace(set, NULL); \
    ASSERT(r, "[1] Replace %s success.", "NULL"); \
    ASSERT(!r, "[1] Replace %s failed.", "NULL"); \
 \
    /* [2] replace `NULL` */ \
    r = rhis_replace(set, NULL); \
    ASSERT(r, "[2] Replace %s success.", "NULL"); \
    ASSERT(!r, "[2] Replace %s failed.", "NULL"); \
 \
    print(set); \
    rhis_free(set); \
    objs_destroy(keys[0]); \
    objs_destroy(keys[1]); \
  } while(0)

#define PERF_TESTS(initial, funcs, mode, print) \
  do { \
    long start; \
    struct rhis* set; \
    struct objs* keys; \
 \
    /* initial set */ \
    if( initial==INIT ) { \
      /* using `rhi_init` */ \
      HANDLE((set=rhis_init(funcs.hash, funcs.equal, \
        funcs.keyfree, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using `rhi_reserve` */ \
      HANDLE((set=rhis_reserve(funcs.hash, funcs.equal, funcs.keyfree, \
        PERF_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    /* initial list of keys */ \
    keys = objs_init(PERF_MIN_OBJLEN, \
      PERF_MAX_OBJLEN, PERF_NUM_KEYS, PERF_OBJCASE); \
 \
    /* replace keys */ \
    start = get_clock(); \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_replace(set, keys->objs[i]); \
    printf("Replace time: %ld ms\n", get_clock()-start); \
    print(set); \
 \
    rhis_free(set); \
    objs_destroy(keys); \
  } while(0)

#define MEMORY_TESTS(initial, funcs, mode, print) \
  do { \
    struct rhis* set; \
    struct objs* keys; \
 \
    /* initial set */ \
    if( initial==INIT ) { \
      /* using `rhi_init` */ \
      HANDLE((set=rhis_init(funcs.hash, funcs.equal, \
        funcs.keyfree, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using `rhi_reserve` */ \
      HANDLE((set=rhis_reserve(funcs.hash, funcs.equal, funcs.keyfree, \
        PERF_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    /* initial list of keys */ \
    keys = objs_init(PERF_MIN_OBJLEN, \
      PERF_MAX_OBJLEN, PERF_NUM_KEYS, PERF_OBJCASE); \
    printf("Keys usage... "); \
    getchar(); \
 \
    /* [1] replace keys */ \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_replace(set, keys->objs[i]); \
    print(set); \
    printf("Keys and set usage... "); \
    getchar(); \
 \
    /* [2] replace keys */ \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_replace(set, keys->objs[i]); \
    print(set); \
    objs_free(keys); \
    printf("Set usage... "); \
    getchar(); \
  } while(0)

extern struct funcs funcs;
extern struct funcs kfuncs;

void set_replace(void) {
  TEST(CONS_TESTS(INIT, funcs, RHI_FIXED, set_print), 0);
  TEST(CONS_TESTS(INIT, funcs, RHI_SHRINK, set_print), 0);
  TEST(CONS_TESTS(INIT, funcs, RHI_EXTEND, set_print), 0);
  TEST(CONS_TESTS(INIT, funcs, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  TEST(CONS_TESTS(RESERVE, funcs, RHI_FIXED, set_print), 0);
  TEST(CONS_TESTS(RESERVE, funcs, RHI_SHRINK, set_print), 0);
  TEST(CONS_TESTS(RESERVE, funcs, RHI_EXTEND, set_print), 0);
  TEST(CONS_TESTS(RESERVE, funcs, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  TEST(PERF_TESTS(INIT, funcs, RHI_FIXED, set_mprint), 0);
  TEST(PERF_TESTS(INIT, funcs, RHI_SHRINK, set_mprint), 0);
  TEST(PERF_TESTS(INIT, funcs, RHI_EXTEND, set_mprint), 0);
  TEST(PERF_TESTS(INIT, funcs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);

  TEST(PERF_TESTS(RESERVE, funcs, RHI_FIXED, set_mprint), 0);
  TEST(PERF_TESTS(RESERVE, funcs, RHI_SHRINK, set_mprint), 0);
  TEST(PERF_TESTS(RESERVE, funcs, RHI_EXTEND, set_mprint), 0);
  TEST(PERF_TESTS(RESERVE, funcs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);

  TEST(MEMORY_TESTS(INIT, kfuncs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);
  TEST(MEMORY_TESTS(RESERVE, kfuncs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);
}
