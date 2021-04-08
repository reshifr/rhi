#include "../set.h"

void set_insert(struct rhifunc func) {
  struct rhis* set;
  struct objs* keys;

#if 0
  /* Initial with fixed */
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(MIN_OBJLEN, MAX_OBJLEN,
    NUM_KEYS, OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);

  /* Initial with extend */
  HANDLE((set=rhis_init(&func,
    RHI_EXTEND))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(MIN_OBJLEN, MAX_OBJLEN,
    NUM_KEYS, OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);

  /* Initial with shrink and extend */
  HANDLE((set=rhis_init(&func,
    RHI_SHRINK|RHI_EXTEND))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(MIN_OBJLEN, MAX_OBJLEN,
    NUM_KEYS, OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);
#endif

#if 1
  /* Reserve with fixed */
  HANDLE((set=rhis_init(&func,
    RHI_FIXED))==NULL, "Set init failed.");
  /* Initial list of keys */
  HANDLE((keys=objs_init(MIN_OBJLEN, MAX_OBJLEN,
    NUM_KEYS, OBJCASE))==NULL, "Keys init failed.");
  /* Insert keys */
  for(rhiuint i=0; i<keys->count; ++i)
    ASSERT(!rhis_insert(set, keys->objs[i]),
      "Insert %s failed.", (const char*)keys->objs[i]);
  set_print(set);
  /* Insert NULL */
  ASSERT(!rhis_insert(set, NULL), "Insert %s failed.", "NULL");
  set_print(set);
  rhis_free(set);
  objs_destroy(keys);
#endif
}
