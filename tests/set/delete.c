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
    /* [2] delete NULL */ \
    r = rhis_delete(set, NULL); \
    ASSERT(r, "[2] Delete %s success.", "NULL"); \
    ASSERT(!r, "[2] Delete %s failed.", "NULL"); \
    print(set); \
 \
    rhis_free(set); \
    objs_destroy(keys[0]); \
    objs_destroy(keys[1]); \
  } while(0)

extern struct funcs funcs;
extern struct funcs kfuncs;

void set_delete(void) {
  TEST(CONS_TESTS(INIT, funcs, RHI_FIXED, set_print), 1);
  TEST(CONS_TESTS(INIT, funcs, RHI_SHRINK, set_print), 0);
  TEST(CONS_TESTS(INIT, funcs, RHI_EXTEND, set_print), 0);
  TEST(CONS_TESTS(INIT, funcs, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  TEST(CONS_TESTS(RESERVE, funcs, RHI_FIXED, set_print), 0);
  TEST(CONS_TESTS(RESERVE, funcs, RHI_SHRINK, set_print), 0);
  TEST(CONS_TESTS(RESERVE, funcs, RHI_EXTEND, set_print), 0);
  TEST(CONS_TESTS(RESERVE, funcs, RHI_SHRINK|RHI_EXTEND, set_print), 0);

  // TEST(PERF_TESTS(INIT, funcs, RHI_FIXED, set_mprint), 0);
  // TEST(PERF_TESTS(INIT, funcs, RHI_SHRINK, set_mprint), 0);
  // TEST(PERF_TESTS(INIT, funcs, RHI_EXTEND, set_mprint), 0);
  // TEST(PERF_TESTS(INIT, funcs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);

  // TEST(PERF_TESTS(RESERVE, funcs, RHI_FIXED, set_mprint), 0);
  // TEST(PERF_TESTS(RESERVE, funcs, RHI_SHRINK, set_mprint), 0);
  // TEST(PERF_TESTS(RESERVE, funcs, RHI_EXTEND, set_mprint), 0);
  // TEST(PERF_TESTS(RESERVE, funcs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);

  // TEST(MEMORY_TESTS(INIT, kfuncs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);
  // TEST(MEMORY_TESTS(RESERVE, kfuncs, RHI_SHRINK|RHI_EXTEND, set_mprint), 0);
}
