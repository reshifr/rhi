#include "../utils.hpp"

#define UNIT_INSERT() \
  do { \
    printf("\n"); \
    rhiuint n = 0; \
    bool ret = false; \
    printf("%snull ", rhis_insert(set, NULL) ? "@" : ""); \
    keys.load([&](void* obj) { \
      ret = rhis_insert(set, obj); \
      printf("%s%s ", ret ? "@" : "", (const char*)obj); \
      if( ret ) \
        ++n; \
    }); \
    std::printf("\nuniqueness=%" PRIrhiu " count=%" PRIrhiu \
      " insert=%" PRIrhiu, keys.uniqueness(), keys.size(), n); \
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
    rhiuint n = 0; \
    rhiuint old_occupied = set->occupied; \
    bool ret = false; \
    rhis_insert(set, NULL); \
    keys.load([&](void* obj) { \
      ret = rhis_insert(set, obj); \
      if( ret ) \
        ++n; \
    }); \
    std::printf("\nuniqueness=%" PRIrhiu " count=%" PRIrhiu \
      " insert=%" PRIrhiu, keys.uniqueness(), keys.size(), n); \
    utils::mprint(set); \
    HANDLE((set->occupied-old_occupied)!=n, \
      "Failed: rhis_insert() occupied in trouble."); \
    HANDLE((set->mode&RHI_FIXED) && \
      set->occupied!=keys.uniqueness() && set->occupied!=set->max, \
      "Failed: rhis_insert() occupied in trouble."); \
    HANDLE((set->mode&RHI_EXTEND) && set->occupied!=keys.uniqueness(), \
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
  rhis_free(set);
}

static void auto_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::AUTO_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::objs keys(3, 15, utils::AUTO_COUNT);
  AUTO_INSERT();
  keys.shuffle();
  HANDLE(!set->has_null_inserted,
    "Failed: rhis_insert() has_null_inserted in trouble.");
  keys.add(utils::AUTO_COUNT);
  AUTO_INSERT();
  rhis_free(set);
}

#define PERFOM_INSERT() \
  do { \
    clock.start(); \
    keys.load([&](void* obj) { \
      rhis_insert(set, obj); \
    }); \
    std::printf("\n%" PRId64 " milliseconds\n", \
      clock.result<utils::timer::mili>()); \
    std::printf("uniqueness=%" PRIrhiu " count=%" PRIrhiu, \
      keys.uniqueness(), keys.size()); \
    utils::mprint(set); \
  } while(0)

static void perfom_init_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_init(utils::hash, utils::equal,
    mode))==NULL, "Error: rhis_init() failed.");
  utils::timer clock;
  utils::objs keys(3, 15, utils::AUTO_COUNT);
  PERFOM_INSERT();
  PERFOM_INSERT();
  rhis_free(set);
}

static void perfom_reserve_test(int mode) {
  struct rhis* set;
  HANDLE((set=rhis_reserve(utils::hash, utils::equal,
    utils::UNIT_RESERVE, mode))==NULL, "Error: rhis_reserve() failed.");
  utils::timer clock;
  utils::objs keys(3, 15, utils::AUTO_COUNT);
  PERFOM_INSERT();
  PERFOM_INSERT();
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

void set::perfom_insert(void) {
  TEST(1, perfom_init_test(RHI_FIXED));
  TEST(1, perfom_init_test(RHI_SHRINK));
  TEST(1, perfom_init_test(RHI_EXTEND));
  TEST(1, perfom_init_test(RHI_SHRINK|RHI_EXTEND));
  TEST(1, perfom_reserve_test(RHI_FIXED));
  TEST(1, perfom_reserve_test(RHI_SHRINK));
  TEST(1, perfom_reserve_test(RHI_EXTEND));
  TEST(1, perfom_reserve_test(RHI_SHRINK|RHI_EXTEND));
}
