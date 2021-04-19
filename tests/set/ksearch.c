#include "../tests.h"

#define INIT 0
#define RESERVE 1

#define CONS_TESTS(initial, funcs, mode, print) \
  do { \
    const void* r; \
    bool rbool; \
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
    /* [0] initialize the object list */ \
    keys[0] = objs_init(CONS_MIN_OBJLEN, \
      CONS_MAX_OBJLEN, CONS_NUM_KEYS, CONS_OBJCASE); \
 \
    /* [1] initialize the object list */ \
    keys[1] = objs_init(CONS_MIN_OBJLEN, \
      CONS_MAX_OBJLEN, CONS_NUM_KEYS, CONS_OBJCASE); \
 \
    /* [0] ksearch with the dictionary is not populated */ \
    print(set); \
    for(rhiuint i=0; i<keys[0]->count; ++i) { \
      r = rhis_ksearch(set, keys[0]->objs[i]); \
      ASSERT(r==NULL, "[0] Ksearch %s not found.", \
        (const char*)keys[0]->objs[i]); \
      ASSERT(r!=NULL, "[0] Ksearch %s found.", (const char*)keys[0]->objs[i]); \
    } \
 \
    /* [0] ksearch NULL */ \
    rbool = rhis_search(set, NULL); \
    r = rhis_ksearch(set, NULL); \
    ASSERT(rbool && r==NULL, "[0] Ksearch %s found.", "NULL"); \
    ASSERT(!rbool && r==NULL, "[0] Ksearch %s not found.", "NULL"); \
 \
    /* populates the dictionary */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) \
      rhis_insert(set, keys[0]->objs[i]); \
 \
    /* populate with NULL */ \
    rhis_insert(set, NULL); \
 \
    /* [1] ksearch with the dictionary is populated */ \
    print(set); \
    for(rhiuint i=0; i<keys[1]->count; ++i) { \
      r = rhis_ksearch(set, keys[1]->objs[i]); \
      ASSERT(r, "[1] Ksearch %s found.", (const char*)keys[1]->objs[i]); \
      ASSERT(!r, "[1] Ksearch %s not found.", (const char*)keys[1]->objs[i]); \
    } \
 \
    /* [1] ksearch NULL */ \
    rbool = rhis_search(set, NULL); \
    r = rhis_ksearch(set, NULL); \
    ASSERT(rbool && r==NULL, "[1] Ksearch %s found.", "NULL"); \
    ASSERT(!rbool && r==NULL, "[1] Ksearch %s not found.", "NULL"); \
 \
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
    /* initialize the object list */ \
    keys = objs_init(PERF_MIN_OBJLEN, \
      PERF_MAX_OBJLEN, PERF_NUM_KEYS, PERF_OBJCASE); \
 \
    /* [0] ksearch with the dictionary is not populated */ \
    start = get_clock(); \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_ksearch(set, keys->objs[i]); \
    printf("[0] Ksearch time: %ld ms\n", get_clock()-start); \
    print(set); \
 \
    /* populates the dictionary */ \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_insert(set, keys->objs[i]); \
 \
    /* [1] ksearch with the dictionary is populated */ \
    start = get_clock(); \
    for(rhiuint i=0; i<keys->count; ++i) \
      rhis_ksearch(set, keys->objs[i]); \
    printf("[1] Ksearch time: %ld ms\n", get_clock()-start); \
    print(set); \
 \
    rhis_free(set); \
    objs_destroy(keys); \
  } while(0)

extern struct funcs funcs;

void set_ksearch(void) {
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
}
