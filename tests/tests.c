#include "tests.h"

/*************
 * Functions *
 *************/

struct funcs funcs = {
  murmur_hash, equal, NULL, NULL
};

struct funcs kfuncs = {
  murmur_hash, equal, free, NULL
};

struct funcs vfuncs = {
  murmur_hash, equal, NULL, free
};

struct funcs ffuncs = {
  murmur_hash, equal, free, free
};

int main(void) {
  srand((unsigned)time(NULL));
  TEST(set_init(), 0);
  TEST(set_reserve(), 0);
  TEST(set_insert(), 1);
  TEST(set_search(), 1);
  TEST(set_ksearch(), 1);
  TEST(set_replace(), 1);
  TEST(set_kreplace(), 1);
  return EXIT_SUCCESS;
}
