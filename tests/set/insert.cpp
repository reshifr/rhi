#include "../utils.hpp"

#define UNIT_INSERT() \
  do { \
    printf("\n"); \
    bool success = rhis_insert(set, NULL); \
    printf("%sNULL ", success ? "@" : ""); \
    keys.load([&set](void* obj) { \
      bool success = rhis_insert(set, obj); \
      printf("%s%s ", success ? "@" : "", (const char*)obj); \
    }); \
    std::printf("\nuniqueness=%" PRIrhiu " count=%" PRIrhiu, \
      keys.uniqueness, keys.count); \
    utils::print(set); \
  } while(0)

static void unit_init_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::objs<utils::UPPER> keys(1, 1, utils::UNIT_COUNT);
  UNIT_INSERT();
  keys.shuffle();
  UNIT_INSERT();
  keys.add(utils::UNIT_COUNT);
  UNIT_INSERT();
  rhis_free(set);
}

static void unit_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::objs<utils::UPPER> keys(1, 1, utils::UNIT_COUNT);
  UNIT_INSERT();
  keys.shuffle();
  UNIT_INSERT();
  keys.add(utils::UNIT_COUNT);
  UNIT_INSERT();
  rhis_free(set);
}

#define AUTO_INSERT() \
  do { \
    rhis_insert(set, NULL); \
    keys.load([&set](void* obj) { \
      rhis_insert(set, obj); \
    }); \
    std::printf("\nuniqueness=%" PRIrhiu " count=%" PRIrhiu, \
      keys.uniqueness, keys.count); \
    utils::mprint(set); \
    HANDLE((set->mode&RHI_FIXED) && \
      set->occupied!=keys.uniqueness && set->occupied!=set->max, \
      "Failed: rhis_insert() occupied in trouble."); \
    HANDLE((set->mode&RHI_EXTEND) && set->occupied!=keys.uniqueness, \
      "Failed: rhis_insert() occupied in trouble."); \
  } while(0)

static void auto_init_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::objs keys(3, 15, utils::AUTO_COUNT);
  AUTO_INSERT();
  keys.shuffle();
  AUTO_INSERT();
  HANDLE(!set->has_null_inserted,
    "Failed: rhis_insert() has_null_inserted in trouble.");
  keys.add(utils::AUTO_COUNT);
  AUTO_INSERT();
  HANDLE(!set->has_null_inserted,
    "Failed: rhis_insert() has_null_inserted in trouble.");
  rhis_free(set);
}

static void auto_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::timer clock;
  utils::objs keys(3, 15, utils::AUTO_COUNT);
  AUTO_INSERT();
  keys.shuffle();
  HANDLE(!set->has_null_inserted,
    "Failed: rhis_insert() has_null_inserted in trouble.");
  keys.add(utils::AUTO_COUNT);
  AUTO_INSERT();
  HANDLE(!set->has_null_inserted,
    "Failed: rhis_insert() has_null_inserted in trouble.");
  rhis_free(set);
}

#define PERF_INSERT() \
  do { \
    clock.start(); \
    keys.load([&set](void* obj) { \
      rhis_insert(set, obj); \
    }); \
    std::printf("\n%" PRId64 " milliseconds\n", \
      clock.result<utils::timer::mili>()); \
    std::printf("uniqueness=%" PRIrhiu " count=%" PRIrhiu, \
      keys.uniqueness, keys.count); \
    utils::mprint(set); \
  } while(0)

static void perf_init_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::timer clock;
  utils::objs keys(3, 15, utils::AUTO_COUNT);
  PERF_INSERT();
  PERF_INSERT();
  rhis_free(set);
}

static void perf_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::timer clock;
  utils::objs keys(3, 15, utils::AUTO_COUNT);
  PERF_INSERT();
  PERF_INSERT();
  rhis_free(set);
}

void set::unit_insert(void) {
  TEST(1, unit_init_test(RHI_FIXED));
  TEST(1, unit_init_test(RHI_SHRINK));
  TEST(1, unit_init_test(RHI_EXTEND));
  TEST(1, unit_init_test(RHI_SHRINK|RHI_EXTEND));
  TEST(1, unit_reserve_test(RHI_FIXED));
  TEST(1, unit_reserve_test(RHI_SHRINK));
  TEST(1, unit_reserve_test(RHI_EXTEND));
  TEST(1, unit_reserve_test(RHI_SHRINK|RHI_EXTEND));
}

void set::auto_insert(void) {
  TEST(1, auto_init_test(RHI_FIXED));
  TEST(1, auto_init_test(RHI_SHRINK));
  TEST(1, auto_init_test(RHI_EXTEND));
  TEST(1, auto_init_test(RHI_SHRINK|RHI_EXTEND));
  TEST(1, auto_reserve_test(RHI_FIXED));
  TEST(1, auto_reserve_test(RHI_SHRINK));
  TEST(1, auto_reserve_test(RHI_EXTEND));
  TEST(1, auto_reserve_test(RHI_SHRINK|RHI_EXTEND));
}

void set::perf_insert(void) {
  TEST(1, perf_init_test(RHI_FIXED));
  TEST(1, perf_init_test(RHI_SHRINK));
  TEST(1, perf_init_test(RHI_EXTEND));
  TEST(1, perf_init_test(RHI_SHRINK|RHI_EXTEND));
  TEST(1, perf_reserve_test(RHI_FIXED));
  TEST(1, perf_reserve_test(RHI_SHRINK));
  TEST(1, perf_reserve_test(RHI_EXTEND));
  TEST(1, perf_reserve_test(RHI_SHRINK|RHI_EXTEND));
}
