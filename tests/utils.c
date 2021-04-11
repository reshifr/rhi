#include "tests.h"

/******************
 * String objects *
 ******************/

#define OVERHEAD_LEN 1
#define MAX_COMB 26

/**
 * Generate string object with minimum length = `min`, maximum
 * length = `max` count = `count`, with case = `mode`.
 */
struct objs* objs_init(int min, int max, rhiuint count, int mode) {
  struct objs* objs;
  HANDLE((objs=malloc(sizeof(struct objs)+
    sizeof(void*)*count))==NULL, "Objs bad alloc.");
  char* buf;
  objs->count = count;
  int range = max-min+1;
  int off_char = mode==LOWER ? 'a' : 'A';
  HANDLE((buf=malloc((size_t)
    (max+OVERHEAD_LEN)))==NULL, "Buf bad alloc.");
  buf[max] = 0;
  for(rhiuint i=0; i<count; ++i) {
    memset(buf, 0, (size_t)max);
    int rands_len = rand()%range+min;
    for(int m=0; m<rands_len; ++m) {
      if( mode==(LOWER|UPPER) )
        off_char = rand()&1 ? 'a' : 'A';
      buf[m] = (char)(rand()%MAX_COMB+off_char);
    }
    objs->objs[i] = malloc(sizeof(char)*(size_t)(max+OVERHEAD_LEN));
    memcpy(objs->objs[i], buf, sizeof(char)*(size_t)(max+OVERHEAD_LEN));
  }
  return objs;
}

/**
 * Make an `objs` duplication.
 */
struct objs* objs_dup(const struct objs* objs) {
  struct objs* newobjs;
  HANDLE((newobjs=malloc(sizeof(struct objs)+
    sizeof(void*)*objs->count))==NULL, "Objs bad alloc.");
  newobjs->count = objs->count;
  for(rhiuint i=0; i<objs->count; ++i) {
    int len = (size_t)strlen(objs->objs[i]);
    newobjs->objs[i] = malloc(sizeof(char)*(size_t)(len+OVERHEAD_LEN));
    memcpy(newobjs->objs[i], objs->objs[i],
      sizeof(char)*(size_t)(len+OVERHEAD_LEN));
  }
  return objs;
}

/**
 * Free memory only for `objs`.
 */
void objs_free(struct objs* objs) {
  free(objs);
}

/**
 * Free memory for `objs[]` and `objs`.
 */
void objs_destroy(struct objs* objs) {
  for(rhiuint i=0; i<objs->count; ++i)
    free(objs->objs[i]);
  free(objs);
}

/*********
 * Clock *
 *********/

/**
 * Get the current time in milliseconds.
 */
long get_clock(void) {
#if defined _WIN32 || defined _WIN64
  return clock();
#elif defined __unix__ || (defined __APPLE__ && defined __MACH__)
  return clock()/1000;
#endif
}

#define HASHVAL_INDEX(_hashval, _size) ((rhiuint)((_hashval)%(_size)))
#define IS_EMPTY(_node) ((_node).key==NULL)

/*******
 * Set *
 *******/

struct rhisnode {
  size_t hashval;
  void* key;
};

struct rhis {
  uint8_t mode;
  uint8_t index;
  uint8_t begin_index;
  uint8_t is_def_key: 4;
  uint8_t is_end: 4;
  rhiuint iter;
  rhiuint min;
  rhiuint max;
  rhiuint size;
  rhiuint occupied;
  rhihash hash;
  rhiequal equal;
  rhikeyfree keyfree;
  struct rhisnode* nodes;
};

/**
 * Print set.
 */
void set_print(struct rhis* set) {
  char* mode;
  switch( set->mode&(RHI_SHRINK|RHI_EXTEND) ) {
    case RHI_FIXED:
      mode = "FIXED";
      break;
    case RHI_SHRINK:
      mode = "SHRINK";
      break;
    case RHI_EXTEND:
      mode = "EXTEND";
      break;
    default:
      mode = "ALL";
      break;
  }
  printf(
    "\n"
    "index=%"PRIu8" begin_index=%"PRIu8"\n"
    "mode=%s size=%"PRIrhiu" min=%"PRIrhiu" "
    "max=%"PRIrhiu" occupied=%"PRIrhiu"\n\n",
    set->index, set->begin_index,
    mode, set->size, set->min, set->max, set->occupied
  );
  printf(
    "|%s|%5s|0x%.16"PRIx64"|%5s|%5"PRIrhiu"|\n\n",
    set->is_def_key ? "O" : "-", "x", UINT64_C(0), "", RHIUINT_C(0)
  );
  for(rhiuint i=0; i<set->size; ++i) {
    bool empty = IS_EMPTY(set->nodes[i]);
    printf(
      "|%s|%5"PRIrhiu"|0x%.16"PRIx64"|%5s|%5"PRIrhiu"|\n",
      empty ? "-" : "O", i,
      empty ? UINT64_C(0) : (uint64_t)set->nodes[i].hashval,
      empty ? "" : (const char*)set->nodes[i].key,
      empty ? RHIUINT_C(0) : HASHVAL_INDEX(set->nodes[i].hashval, set->size)
    );
  }
  printf("\n");
}

/**
 * Print only set metadata.
 */
void set_mprint(struct rhis* set) {
  char* mode;
  switch( set->mode&(RHI_SHRINK|RHI_EXTEND) ) {
    case RHI_FIXED:
      mode = "FIXED";
      break;
    case RHI_SHRINK:
      mode = "SHRINK";
      break;
    case RHI_EXTEND:
      mode = "EXTEND";
      break;
    default:
      mode = "ALL";
      break;
  }
  printf(
    "\n"
    "index=%"PRIu8" begin_index=%"PRIu8"\n"
    "mode=%s size=%"PRIrhiu" min=%"PRIrhiu" "
    "max=%"PRIrhiu" occupied=%"PRIrhiu"\n\n",
    set->index, set->begin_index,
    mode, set->size, set->min, set->max, set->occupied
  );
  printf(
    "|%s|%5s|0x%.16"PRIx64"|%5s|%5"PRIrhiu"|\n\n",
    set->is_def_key ? "O" : "-", "x", UINT64_C(0), "", RHIUINT_C(0)
  );
}

/*******
 * Map *
 *******/

struct rhimnode {
  size_t hashval;
  void* key;
  void* val;
};

struct rhim {
  uint8_t mode;
  uint8_t index;
  uint8_t begin_index;
  uint8_t is_def_key: 4;
  uint8_t is_end: 4;
  rhiuint iter;
  rhiuint min;
  rhiuint max;
  rhiuint size;
  rhiuint occupied;
  rhihash hash;
  rhiequal equal;
  rhikeyfree keyfree;
  rhivalfree valfree;
  void* def_val;
  struct rhimnode* nodes;
};

/**
 * Print map.
 */
void map_print(struct rhim* map) {
  char* mode;
  switch( map->mode&(RHI_SHRINK|RHI_EXTEND) ) {
    case RHI_FIXED:
      mode = "FIXED";
      break;
    case RHI_SHRINK:
      mode = "SHRINK";
      break;
    case RHI_EXTEND:
      mode = "EXTEND";
      break;
    default:
      mode = "ALL";
      break;
  }
  printf(
    "\n"
    "index=%"PRIu8" begin_index=%"PRIu8"\n"
    "mode=%s size=%"PRIrhiu" min=%"PRIrhiu" "
    "max=%"PRIrhiu" occupied=%"PRIrhiu"\n\n",
    map->index, map->begin_index,
    mode, map->size, map->min, map->max, map->occupied
  );
  printf(
    "|%s|%5s|0x%.16"PRIx64"|%5s|%5s|%5"PRIrhiu"|\n\n",
    map->is_def_key ? "O" : "-", "x", UINT64_C(0), "", 
    map->is_def_key ? (const char*)map->def_val : "", RHIUINT_C(0)
  );
  for(rhiuint i=0; i<map->size; ++i) {
    bool empty = IS_EMPTY(map->nodes[i]);
    printf(
      "|%s|%5"PRIrhiu"|0x%.16"PRIx64"|%5s|%5s|%5"PRIrhiu"|\n",
      empty ? "-" : "O", i,
      empty ? UINT64_C(0) : (uint64_t)map->nodes[i].hashval,
      empty ? "" : (const char*)map->nodes[i].key,
      empty ? "" : (const char*)map->nodes[i].val,
      empty ? RHIUINT_C(0) : HASHVAL_INDEX(map->nodes[i].hashval, map->size)
    );
  }
  printf("\n");
}

/**
 * Print only map metadata.
 */
void map_mprint(struct rhim* map) {
  char* mode;
  switch( map->mode&(RHI_SHRINK|RHI_EXTEND) ) {
    case RHI_FIXED:
      mode = "FIXED";
      break;
    case RHI_SHRINK:
      mode = "SHRINK";
      break;
    case RHI_EXTEND:
      mode = "EXTEND";
      break;
    default:
      mode = "ALL";
      break;
  }
  printf(
    "\n"
    "index=%"PRIu8" begin_index=%"PRIu8"\n"
    "mode=%s size=%"PRIrhiu" min=%"PRIrhiu" "
    "max=%"PRIrhiu" occupied=%"PRIrhiu"\n\n",
    map->index, map->begin_index,
    mode, map->size, map->min, map->max, map->occupied
  );
  printf(
    "|%s|%5s|0x%.16"PRIx64"|%5s|%5s|%5"PRIrhiu"|\n\n",
    map->is_def_key ? "O" : "-", "x", UINT64_C(0), "", 
    map->is_def_key ? (const char*)map->def_val : "", RHIUINT_C(0)
  );
}

/*****************
 * Miscellaneous *
 *****************/

/**
 * Hash function for string object.
 */
size_t murmur_hash(const void* obj) {
  size_t len = strlen(obj);
#if SIZE_MAX == UINT64_MAX
  const int r = 47;
  const uint64_t m = UINT64_C(0xc6a4a7935bd1e995);
  uint64_t h = SEED^(len*m);
  const uint64_t* data0 = (const uint64_t*)obj;
  const uint64_t* end = data0+(len>>3);
  while( data0!=end ) {
    uint64_t k = *data0++;
    k *= m;
    k ^= k>>r;
    k *= m;
    h ^= k;
    h *= m;
  }
  const uint8_t* data1 = (const uint8_t*)data0;
  switch( len&7 ) {
    case 7: goto b7;
    case 6: goto b6;
    case 5: goto b5;
    case 4: goto b4;
    case 3: goto b3;
    case 2: goto b2;
    case 1: goto b1;
    default: goto b0;
  }
  b7: h ^= (uint64_t)data1[6]<<48;
  b6: h ^= (uint64_t)data1[5]<<40;
  b5: h ^= (uint64_t)data1[4]<<32;
  b4: h ^= (uint64_t)data1[3]<<24;
  b3: h ^= (uint64_t)data1[2]<<16;
  b2: h ^= (uint64_t)data1[1]<<8;
  b1: h ^= (uint64_t)data1[0];
  b0:
    h *= m;
    h ^= h>>r;
    h *= m;
    h ^= h>>r;
#else
  const int r = 24;
  const uint32_t m = UINT32_C(0x5bd1e995);
  uint32_t h = SEED^len;
  const uint8_t* data = (const uint8_t*)obj;
  while( len>=4 ) {
    uint32_t k = *(uint32_t*)data;
    k *= m;
    k ^= k>>r;
    k *= m;
    h *= m;
    h ^= k;
    data += 4;
    len -= 4;
  }
  switch( len ) {
    case 3: goto b3;
    case 2: goto b2;
    case 1: goto b1;
    default: goto b0;
  };
  b3: h ^= (uint32_t)data[2]<<16;
  b2: h ^= (uint32_t)data[1]<<8;
  b1: h ^= (uint32_t)data[0];
  b0:
    h *= m;
    h ^= h>>13;
    h *= m;
    h ^= h>>15;
#endif
  return h;
}

/**
 * Equal function for string object.
 */
bool equal(const void* first_obj, const void* second_obj) {
  return strcmp(first_obj, second_obj)==0;
}
