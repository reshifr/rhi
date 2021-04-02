#include "../corrects.h"

void map_reserve(struct rhifunc func) {
  struct rhim* map;
  map = rhim_reserve(&func, 100, RHI_FIXED);
  map_print(map);
}
