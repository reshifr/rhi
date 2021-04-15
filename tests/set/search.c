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
    /* [2] initial list of keys */ \
    keys[1] = objs_init(CONS_MIN_OBJLEN, \
      CONS_MAX_OBJLEN, CONS_NUM_KEYS, CONS_OBJCASE); \
 \
    /* [1] search with the dictionary is not populated */ \
    print(set); \
    for(rhiuint i=0; i<keys[0]->count; ++i) { \
      r = rhis_search(set, keys[0]->objs[i]); \
      ASSERT(r, "[1] Search %s found.", (const char*)keys[0]->objs[i]); \
      ASSERT(!r, "[1] Search %s not found.", (const char*)keys[0]->objs[i]); \
    } \
 \
    /* populates the dictionary */ \
    for(rhiuint i=0; i<keys[0]->count; ++i) \
      rhis_insert(set, keys[0]->objs[i]); \
 \
    /* [2] search with the dictionary is populated */ \
    print(set); \
    for(rhiuint i=0; i<keys[1]->count; ++i) { \
      r = rhis_search(set, keys[1]->objs[i]); \
      ASSERT(r, "[2] Search %s found.", (const char*)keys[1]->objs[i]); \
      ASSERT(!r, "[2] Search %s not found.", (const char*)keys[1]->objs[i]); \
    } \
 \
    rhis_free(set); \
    objs_destroy(keys[0]); \
    objs_destroy(keys[1]); \
  } while(0)

extern struct funcs funcs;

void set_search(void) {
  TEST(CONS_TESTS(INIT, funcs, RHI_FIXED, set_print), 1);
  TEST(CONS_TESTS(INIT, funcs, RHI_SHRINK, set_print), 0);
  TEST(CONS_TESTS(INIT, funcs, RHI_EXTEND, set_print), 0);
  TEST(CONS_TESTS(INIT, funcs, RHI_SHRINK|RHI_EXTEND, set_print), 0);
}