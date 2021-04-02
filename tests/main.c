#include "utils.h"

struct rhifunc func = {
  murmur_hash, equal, NULL, NULL
};

int main(void) {
  TEST(set_init(func), 0);
  TEST(set_reserve(func), 0);
  TEST(set_insert(func), 1);

  return 0;
}
