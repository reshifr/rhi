#include "../corrects.h"

void map_init(struct rhifunc func) {
  struct rhim* map;
  map = rhim_init(&func, RHI_FIXED);
  map_print(map);
}
