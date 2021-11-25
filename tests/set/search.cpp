#include "../utils.hpp"

#define UNIT_SEARCH() \
  do { \
    printf("\n"); \
    bool success = rhis_search(set, NULL); \
    printf("%snull ", success ? "@" : ""); \
    keys.load([&set](void* obj) { \
      bool success = rhis_search(set, obj); \
      printf("%s%s ", success ? "@" : "", (const char*)obj); \
    }); \
    std::printf("\nuniqueness=%" PRIrhiu " count=%" PRIrhiu, \
      keys.uniqueness(), keys.size()); \
    utils::print(set); \
  } while(0)

static void unit_init_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::objs<utils::UPPER> keys(1, 1, utils::UNIT_COUNT);
  keys.load([&set](void* obj) {
    rhis_insert(set, obj);
  });
  UNIT_SEARCH();
  keys.shuffle();
  rhis_insert(set, NULL);
  UNIT_SEARCH();
  keys.add(utils::UNIT_COUNT);
  UNIT_SEARCH();
  rhis_free(set);
}

static void unit_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::objs<utils::UPPER> keys(1, 1, utils::UNIT_COUNT);
  UNIT_SEARCH();
  keys.shuffle();
  rhis_insert(set, NULL);
  UNIT_SEARCH();
  keys.add(utils::UNIT_COUNT);
  UNIT_SEARCH();
  rhis_free(set);
}

void set::unit_search(void) {
  TEST(1, unit_init_test(RHI_FIXED));
  TEST(1, unit_init_test(RHI_SHRINK));
  TEST(1, unit_init_test(RHI_EXTEND));
  TEST(1, unit_init_test(RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_reserve_test(RHI_FIXED));
  TEST(1, unit_reserve_test(RHI_SHRINK));
  TEST(1, unit_reserve_test(RHI_EXTEND));
  TEST(1, unit_reserve_test(RHI_SHRINK|RHI_EXTEND));
}
