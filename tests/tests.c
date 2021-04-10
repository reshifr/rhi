#include "tests.h"

/***************************
 * Collection of functions *
 ***************************/

struct rhifunc func = {
  murmur_hash, equal, NULL, NULL
};

struct rhifunc kfunc = {
  murmur_hash, equal, free, NULL
};

struct rhifunc vfunc = {
  murmur_hash, equal, NULL, free
};

struct rhifunc ffunc = {
  murmur_hash, equal, free, free
};

int main(void) {
  TEST(set_init, 0);
  TEST(set_reserve, 1);
  TEST(set_insert, 0);

  return 0;
}
