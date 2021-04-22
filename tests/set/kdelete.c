#include "../tests.h"

#define INIT 0
#define RESERVE 1

#define CONS_TESTS(initial, funcs, mode, print) \
  do { \
    bool r; \
    struct rhis* set; \
    struct objs* keys[2]; \
 \
    /* initialize the dictionary */ \
    if( initial==INIT ) { \
      /* using rhi_init() */ \
      HANDLE((set=rhis_init(funcs.hash, funcs.equal, \
        funcs.keyfree, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using rhi_reserve() */ \
      HANDLE((set=rhis_reserve(funcs.hash, funcs.equal, funcs.keyfree, \
        CONS_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    keys[0] = objs_init(CONS_MIN_OBJLEN, \
      CONS_MAX_OBJLEN, CONS_NUM_KEYS, CONS_OBJCASE); \
 \
    keys[1] = objs_init(CONS_MIN_OBJLEN, \
      CONS_MAX_OBJLEN, CONS_NUM_KEYS, CONS_OBJCASE); \
 \
    /* [0] populates the dictionary by key[0] and NULL */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) \
      rhis_insert(set, keys[0]->objs[i]); \
    rhis_insert(set, NULL); \
    print(set); \
 \
    /* [0] delete by keys[0] */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) { \
      r = rhis_delete(set, keys[0]->objs[i]); \
      ASSERT(r, "[0] Delete %s success.", (const char*)keys[0]->objs[i]); \
      ASSERT(!r, "[0] Delete %s failed.", (const char*)keys[0]->objs[i]); \
    } \
 \
    /* [0] delete NULL */ \
    r = rhis_delete(set, NULL); \
    ASSERT(r, "[0] Delete %s success.", "NULL"); \
    ASSERT(!r, "[0] Delete %s failed.", "NULL"); \
    print(set); \
 \
    /* [1] populates the dictionary by key[0] and NULL */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) \
      rhis_insert(set, keys[0]->objs[i]); \
    rhis_insert(set, NULL); \
    print(set); \
 \
    /* [1] delete by keys[1] */ \
    for(rhiuint i=0; i<keys[1]->count; ++i) { \
      r = rhis_delete(set, keys[1]->objs[i]); \
      ASSERT(r, "[1] Delete %s success.", (const char*)keys[1]->objs[i]); \
      ASSERT(!r, "[1] Delete %s failed.", (const char*)keys[1]->objs[i]); \
    } \
 \
    /* [1] delete NULL */ \
    r = rhis_delete(set, NULL); \
    ASSERT(r, "[1] Delete %s success.", "NULL"); \
    ASSERT(!r, "[1] Delete %s failed.", "NULL"); \
 \
    /* [1] delete NULL */ \
    r = rhis_delete(set, NULL); \
    ASSERT(r, "[1] Delete %s success.", "NULL"); \
    ASSERT(!r, "[1] Delete %s failed.", "NULL"); \
    print(set); \
 \
    objs_free(keys[0]); \
    objs_free(keys[1]); \
  } while(0)

#define PERF_TESTS(initial, funcs, mode, print) \
  do { \
    long start; \
    struct rhis* set; \
    struct objs* keys; \
 \
    /* initialize the dictionary */ \
    if( initial==INIT ) { \
      /* using rhi_init() */ \
      HANDLE((set=rhis_init(funcs.hash, funcs.equal, \
        funcs.keyfree, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using rhi_reserve() */ \
      HANDLE((set=rhis_reserve(funcs.hash, funcs.equal, funcs.keyfree, \
        PERF_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    keys = objs_init(PERF_MIN_OBJLEN, \
      PERF_MAX_OBJLEN, PERF_NUM_KEYS, PERF_OBJCASE); \
 \
    /* populates the dictionary */ \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_insert(set, keys->objs[i]); \
    print(set); \
 \
    /* delete by keys */ \
    start = get_clock(); \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_delete(set, keys->objs[i]); \
    printf("Delete time: %ld ms\n", get_clock()-start); \
    print(set); \
    objs_free(keys); \
  } while(0)

#define MEMORY_TESTS(initial, funcs, mode, print) \
  do { \
    struct rhis* set; \
    struct objs* keys; \
 \
    /* initialize the dictionary */ \
    if( initial==INIT ) { \
      /* using rhi_init() */ \
      HANDLE((set=rhis_init(funcs.hash, funcs.equal, \
        funcs.keyfree, mode))==NULL, "Set init failed."); \
    } else if( initial==RESERVE ) { \
      /* using rhi_reserve() */ \
      HANDLE((set=rhis_reserve(funcs.hash, funcs.equal, funcs.keyfree, \
        PERF_RESERVE_SIZE, mode))==NULL, "Set reserve failed."); \
    } \
 \
    keys = objs_init(PERF_MIN_OBJLEN, \
      PERF_MAX_OBJLEN, PERF_NUM_KEYS, PERF_OBJCASE); \
    printf("Keys usage... "); \
    getchar(); \
 \
    /* insert keys */ \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_insert(set, keys->objs[i]); \
    print(set); \
    printf("Keys and set usage... "); \
    getchar(); \
 \
    /* delete by keys */ \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_delete(set, keys->objs[i]); \
    print(set); \
    objs_free(keys); \
    printf("Set usage... "); \
    getchar(); \
  } while(0)

extern struct funcs funcs;
extern struct funcs kfuncs;

void set_kdelete(void) {
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
