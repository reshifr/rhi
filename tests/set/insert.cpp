#include "../utils.hpp"

#define UNIT_INSERT() \
  do { \
    printf("\n"); \
    rhis_insert(set, NULL); \
    keys.load([&set](void* obj) { \
      bool success = rhis_insert(set, obj); \
      printf("%s%s ", success ? "@" : "", (const char*)obj); \
    }); \
    std::printf("\nuniqueness=%" PRIrhiu " count=%" PRIrhiu, \
      keys.uniqueness, utils::UNIT_COUNT); \
    utils::print(set); \
  } while(0)

static void unit_init_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::objs<utils::UNIT_CASE>
    keys(utils::UNIT_MIN, utils::UNIT_MAX, utils::UNIT_COUNT);
  // Insert
  UNIT_INSERT();
  keys.reverse();
  // Insert
  UNIT_INSERT();
  rhis_free(set);
}

static void unit_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::objs<utils::UNIT_CASE>
    keys(utils::UNIT_MIN, utils::UNIT_MAX, utils::UNIT_COUNT);
  // Insert
  UNIT_INSERT();
  keys.reverse();
  // Insert
  UNIT_INSERT();
  rhis_free(set);
}

#define PERF_INSERT(type) \
  do { \
    clock.start(); \
    keys.load([&set](void* obj) { \
      rhis_insert(set, obj); \
    }); \
    std::printf("\n%s=%" PRId64 " ms\n", \
      type, clock.result<utils::timer::mili>()); \
    std::printf("uniqueness=%" PRIrhiu " count=%" PRIrhiu, \
      keys.uniqueness, utils::PERF_COUNT); \
    utils::mprint(set); \
  } while(0)

static void perf_init_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::timer clock;
  utils::objs<utils::PERF_CASE>
    keys(utils::PERF_MIN, utils::PERF_MAX, utils::PERF_COUNT);
  PERF_INSERT("insert");
  keys.reverse();
  // Insert
  PERF_INSERT("reserve_insert");
  rhis_free(set);
}

static void perf_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::timer clock;
  utils::objs<utils::PERF_CASE>
    keys(utils::PERF_MIN, utils::PERF_MAX, utils::PERF_COUNT);
  PERF_INSERT("insert");
  keys.reverse();
  // Insert
  PERF_INSERT("reserve_insert");
  rhis_free(set);
}

void set::insert(void) {
  TEST(0, unit_init_test(RHI_FIXED));
  TEST(0, unit_init_test(RHI_SHRINK));
  TEST(0, unit_init_test(RHI_EXTEND));
  TEST(0, unit_init_test(RHI_SHRINK|RHI_EXTEND));
  TEST(0, unit_reserve_test(RHI_FIXED));
  TEST(0, unit_reserve_test(RHI_SHRINK));
  TEST(0, unit_reserve_test(RHI_EXTEND));
  TEST(0, unit_reserve_test(RHI_SHRINK|RHI_EXTEND));
  TEST(0, perf_init_test(RHI_FIXED));
  TEST(0, perf_init_test(RHI_SHRINK));
  TEST(0, perf_init_test(RHI_EXTEND));
  TEST(1, perf_init_test(RHI_SHRINK|RHI_EXTEND));
  TEST(0, perf_reserve_test(RHI_FIXED));
  TEST(0, perf_reserve_test(RHI_SHRINK));
  TEST(0, perf_reserve_test(RHI_EXTEND));
  TEST(0, perf_reserve_test(RHI_SHRINK|RHI_EXTEND));
}
