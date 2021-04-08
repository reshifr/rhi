#include "../corrects.h"

void set_insert(struct rhifunc func) {
  struct rhis* set;
  struct objs* keys;

#if 0
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  HANDLE((keys=objs_init(3, 4, 14,
    LOWER))==NULL, "Keys init failed.");
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);
#endif

#if 0
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  HANDLE((keys=objs_init(3, 4, 14,
    LOWER))==NULL, "Keys init failed.");
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);
#endif
}

