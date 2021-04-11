/**
 * Reshifr Hash Index
 * Copyright (c) 2019 Renol P. H. <reshifr@gmail.com>
 * 
 * MIT License
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <rhi.h>
#include <stdlib.h>

#ifdef RHI_PRIME_METHOD
 const rhiuint range[] = {
   RHIUINT_C(7),
   RHIUINT_C(17),
   RHIUINT_C(31),
   RHIUINT_C(67),
   RHIUINT_C(127),
   RHIUINT_C(257),
   RHIUINT_C(509),
   RHIUINT_C(1021),
   RHIUINT_C(2053),
   RHIUINT_C(4099),
   RHIUINT_C(8191),
   RHIUINT_C(16381),
   RHIUINT_C(32771),
   RHIUINT_C(65537),
   RHIUINT_C(131071),
   RHIUINT_C(262147),
   RHIUINT_C(524287),
   RHIUINT_C(1048573),
   RHIUINT_C(2097143),
   RHIUINT_C(4194301),
   RHIUINT_C(8388617),
   RHIUINT_C(16777213),
   RHIUINT_C(33554467),
   RHIUINT_C(67108859),
   RHIUINT_C(134217757),
   RHIUINT_C(268435459),
   RHIUINT_C(536870909),
   RHIUINT_C(1073741827),
   RHIUINT_C(2147483647),
 };

# define BEGIN_INDEX 0
# define END_INDEX 28
# define GET_SIZE(_index) (range[_index])
# define HASHVAL_INDEX(_hashval, _size) ((rhiuint)((_hashval)%(_size)))
#else
# define BEGIN_INDEX 3
# define END_INDEX 31
# define GET_SIZE(_index) ((rhiuint)1<<(_index))
# define HASHVAL_INDEX(_hashval, _size) ((rhiuint)((_hashval)&((_size)-1)))
#endif

/************
 * Set node *
 ************/

struct rhisnode {
  size_t hashval;
  void* key;
};

/*******
 * Set *
 *******/

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

#define RHIS_NODE(_hashval, _key) \
  ((struct rhisnode){ .hashval=_hashval, .key=_key })

/************
 * Map node *
 ************/

struct rhimnode {
  size_t hashval;
  void* key;
  void* val;
};

/*******
 * Map *
 *******/

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

#define PAIR(_key, _val) ((struct rhipair){ .key=_key, .val=_val })
#define CONSTPAIR(_key, _val) ((struct rhiconstpair){ .key=_key, .val=_val })
#define DEFPAIR PAIR(DEFVAL, DEFVAL)
#define DEFCONSTPAIR CONSTPAIR(DEFVAL, DEFVAL)
#define RHIM_NODE(_hashval, _key, _val) \
  ((struct rhimnode){ .hashval=_hashval, .key=_key, .val=_val })

/******************
 * Generic macros *
 ******************/

/* Default values for key and value */
#define DEFVAL NULL

/* Value handler of `iter` for `def_val` */
#define DEFITER RHIUINT_MAX

#define MIN_LOAD 0.18
#define MAX_LOAD 0.72
#define BEGIN_EXPONENT 3
#define IS_EMPTY(_node) ((_node).key==DEFVAL)
#define SET_EMPTY(_node) ((_node).key=DEFVAL)
#define HASHVAL_PROB(_prob, _size) HASHVAL_INDEX((_prob)+1, _size)
#define MIN_OCCUPIED(_index, _begin_index) ((_index)==(_begin_index) ? \
  RHIUINT_C(0) : ((rhiuint)((double)GET_SIZE(_index)*MIN_LOAD)))
#define MAX_OCCUPIED(_index) ((rhiuint)((double)GET_SIZE(_index)*MAX_LOAD))
#define COUNT(_obj) ((obj)->is_def_key ? (obj)->occupied+1 : (obj)->occupied)

/**
 * Set size constraints for set/map.
 */
#define SET_BOUND(_obj, _index, _size) \
  do { \
    (_obj)->index = (uint8_t)(_index); \
    (_obj)->min = MIN_OCCUPIED(_index, (_obj)->begin_index); \
    (_obj)->max = MAX_OCCUPIED(_index); \
    (_obj)->size = _size; \
  } while(0)

/**
 * Obtain floor(log2(n)) value.
 */
static inline int msb_index(rhiuint n) {
  int i = 0;
  if( n>=RHIUINT_C(0x00010000) )
    i += 16, n >>= 16;
  if( n>=RHIUINT_C(0x00000100) )
    i += 8, n >>= 8;
  if( n>=RHIUINT_C(0x00000010) )
    i += 4, n >>= 4;
  if( n>=RHIUINT_C(0x00000004) )
    i += 2, n >>= 2;
  if( n>=RHIUINT_C(0x00000002) )
    i += 1, n >>= 1;
  return i;
}

/**
 * Obtain the appropriate `index` for the specified size.
 */
static inline int get_index(rhiuint size) {
  int index = msb_index((rhiuint)((double)size/MAX_LOAD))-
    (BEGIN_EXPONENT-BEGIN_INDEX);
  if( index<BEGIN_INDEX )
    index = BEGIN_INDEX;
  while( index<END_INDEX && size>MAX_OCCUPIED(index) )
    ++index;
  return index;
}

/**
 * Move current elements to the new `nodes`
 */
#define MOVE_NODES(_obj, _newsize, _newnodes) \
  do { \
    for(rhiuint _i=0, _prob; _i<(_obj)->size; ++_i) { \
      if( IS_EMPTY((_obj)->nodes[_i]) ) \
        continue; \
      _prob = HASHVAL_INDEX((_obj)->nodes[_i].hashval, _newsize); \
      for(rhiuint _inew=0; _inew<(_newsize); ++_inew) { \
        if( IS_EMPTY((_newnodes)[_prob]) ) { \
          (_newnodes)[_prob] = (_obj)->nodes[_i]; \
          break; \
        } \
        _prob = HASHVAL_PROB(_prob, _newsize); \
      } \
    } \
    free((_obj)->nodes); \
    (_obj)->nodes = _newnodes; \
  } while(0)

/**
 * Extend `nodes` size when `size` > `max`
 */
#define DECL_EXTEND_NODES(_func_name, _obj_type, _node_type) \
  static bool _func_name(_obj_type* obj) { \
    if( obj->index==END_INDEX ) \
      return false; \
    _node_type* newnodes; \
    int newindex = obj->index+1; \
    rhiuint newsize = GET_SIZE(newindex); \
    if( (newnodes=(_node_type*)calloc(newsize, sizeof(_node_type)))==NULL ) \
      return false; \
    MOVE_NODES(obj, newsize, newnodes); \
    SET_BOUND(obj, newindex, newsize); \
    return true; \
  }

/**
 * Shrink `nodes` size when `size` < `min`
 */
#define SHRINK_NODES(_obj, _node_type) \
  do { \
    if( (_obj)->index==(_obj)->begin_index ) \
      break; \
    _node_type* _newnodes; \
    int _newindex = (_obj)->index-1; \
    rhiuint _newsize = GET_SIZE(_newindex); \
    if( (_newnodes=(_node_type*)calloc(_newsize, sizeof(_node_type)))==NULL ) \
      break; \
    MOVE_NODES(_obj, _newsize, _newnodes); \
    SET_BOUND(_obj, _newindex, _newsize); \
  } while(0)

/**
 * If the subsequent element is in prob sequence shift the
 * element to an empty node, do until finding an empty node
 * when continuing the step for the next subsequent element.
 */
#define BACKWARD_SHIFT(_obj, _equal_prob) \
  do { \
    rhiuint _empty = _equal_prob; \
    for(rhiuint _i=0, _prob=_equal_prob; _i<(_obj)->size; ++_i) { \
      _prob = HASHVAL_PROB(_prob, (_obj)->size); \
      if( IS_EMPTY((_obj)->nodes[_prob]) ) { \
        SET_EMPTY((_obj)->nodes[_empty]); \
        break; \
      } \
      rhiuint _origin \
        = HASHVAL_INDEX((_obj)->nodes[_prob].hashval, (_obj)->size); \
      if( _empty<=_prob ? \
          _empty<_origin && _origin<=_prob : \
          _empty<_origin || _origin<=_prob ) \
        continue; \
      (_obj)->nodes[_empty] = (_obj)->nodes[_prob]; \
      _empty = _prob; \
    } \
  } while(0)

#define DECL_COUNT(_func_name, _obj_type) \
  rhiuint _func_name(const _obj_type* obj) { \
    return COUNT(obj); \
  }

#define DECL_BEGIN(_func_name, _obj_type) \
  void _func_name(_obj_type* obj) { \
    if( obj->is_def_key ) { \
      obj->is_end = false; \
      obj->iter = DEFITER; \
      return; \
    } \
    for(rhiuint i=0; i<obj->size; ++i) { \
      if( IS_EMPTY(obj->nodes[i]) ) \
        continue; \
      obj->is_end = false; \
      obj->iter = i; \
      break; \
    } \
  }

#define DECL_NEXT(_func_name, _obj_type) \
  void _func_name(_obj_type* obj) { \
    rhiuint i = obj->iter==DEFITER ? 0 : obj->iter+1; \
    for(; i<obj->size; ++i) { \
      if( IS_EMPTY(obj->nodes[i]) ) \
        continue; \
      obj->iter = i; \
      return; \
    } \
    obj->is_end = true; \
  }

#define DECL_END(_func_name, _obj_type) \
  bool _func_name(_obj_type* obj) { \
    return obj->is_end; \
  }

#define DECL_ITERS(_func_name, _obj_type) \
  rhiuint* _func_name(const _obj_type* obj) { \
    rhiuint count; \
    if( (count=COUNT(obj))==0 ) \
      return NULL; \
    rhiuint* iter; \
    if( (iter=(rhiuint*)malloc(count*sizeof(rhiuint)))==NULL ) \
      return NULL; \
    rhiuint start = 0; \
    if( obj->is_def_key ) \
      iter[start++] = DEFITER; \
    for(rhiuint i=0, prob=start; i<obj->size; ++i) { \
      if( IS_EMPTY(obj->nodes[i]) ) \
        continue; \
      iter[prob++] = i; \
    } \
    return iter; \
  }

/* ===== Set ===== */

/*********************
 * Initial functions *
 *********************/

#define RHIS_INIT(_func, _mode, _index) \
  do { \
    struct rhis* _set; \
    if( (_set=(struct rhis*)calloc(1, sizeof(struct rhis)))==NULL ) \
      return NULL; \
    rhiuint _initsize = GET_SIZE(_index); \
    if( (_set->nodes=(struct rhisnode*)calloc( \
        _initsize, sizeof(struct rhisnode)))==NULL ) { \
      free(_set); \
      return NULL; \
    } \
    _set->mode = (uint8_t)(_mode); \
    _set->begin_index = (uint8_t)(_index); \
    SET_BOUND(_set, _index, _initsize); \
    _set->hash = (_func)->hash; \
    _set->equal = (_func)->equal; \
    _set->keyfree = (_func)->keyfree; \
    return _set; \
  } while(0)

/**
 * \brief   Initial set
 * 
 * Set will be initialized to the default size.
 * 
 * \param   func  Collection of overridable functions
 * \param   mode  Set mode
 * 
 * \return  On success, the pointer of set is returned. On
 *          failure, `NULL` is returned.
 */
struct rhis* rhis_init(const struct rhifunc* func, int mode) {
  RHIS_INIT(func, mode, BEGIN_INDEX);
}

/**
 * \brief   Reserve set
 * 
 * Set will be initialized to the specified size, the size to
 * be set >= the specified size. the maximum set size for
 * prime method is 1546188225, and the default method is
 * 1546188226.
 * 
 * \param   func  Collection of overridable functions
 * \param   size  Specific size reserved
 * \param   mode  Set mode
 * 
 * \return  On success, the pointer of set is returned. On
 *          failure, `NULL` is returned.
 */
struct rhis* rhis_reserve(const struct rhifunc* func, rhiuint size, int mode) {
  int index = get_index(size);
  RHIS_INIT(func, mode, index);
}

/********************
 * Insert functions *
 ********************/

DECL_EXTEND_NODES(set_extend_nodes, struct rhis, struct rhisnode)

/**
 * \brief   Insert key into set
 * 
 * Insertion failed because
 *  - Key was in set before.
 *  - Maximum set limit has been reached with set mode, not in
 *    RHI_EXTEND.
 *  - On rare condition, memory allocation may fail when set
 *    is extended.
 * 
 * \param   set  Set for insertion
 * \param   key  Set mode
 * 
 * \return  On success, `true` is returned. On failure,
 *          `false` is returned.
 */
bool rhis_insert(struct rhis* set, void* key) {
  /* handling of default key values */
  if( key==DEFVAL ) {
    if( set->is_def_key )
      return false;
    set->is_def_key = true;
    return true;
  }
  if( set->occupied<set->max ||
      ((set->mode&RHI_EXTEND) && set_extend_nodes(set)) ) {
    size_t hashval = set->hash(key);
    rhiuint prob = HASHVAL_INDEX(hashval, set->size);
    for(rhiuint i=0; i<set->size; ++i) {
      if( IS_EMPTY(set->nodes[prob]) ) {
        ++set->occupied;
        set->nodes[prob] = RHIS_NODE(hashval, key);
        return true;
      }
      if( hashval==set->nodes[prob].hashval &&
          set->equal(key, set->nodes[prob].key) )
        return false;
      prob = HASHVAL_PROB(prob, set->size);
    }
  }
  return false;
}

/*********************
 * Replace functions *
 *********************/

#define RHIS_REPLACE(_set, _hashval, _key, _ret, _def_ret) \
  do { \
    if( ((_set)->mode&RHI_EXTEND) && set_extend_nodes(_set) ) { \
      rhiuint _prob = HASHVAL_INDEX(_hashval, (_set)->size); \
      for(rhiuint _i=0; _i<(_set)->size; ++_i) { \
        if( IS_EMPTY((_set)->nodes[_prob]) ) { \
          ++(_set)->occupied; \
          (_set)->nodes[_prob] = RHIS_NODE(_hashval, _key); \
          return _ret; \
        } \
        _prob = HASHVAL_PROB(_prob, (_set)->size); \
      } \
    } \
    return _def_ret; \
  } while(0)

/**
 * \brief   Replace key into set
 * 
 * When key will be replaced
 *  - If `keyfree` is set, key will be destroyed.
 * 
 * Replacement failed because key was unique
 *  - But the maximum set limit has been reached with set
 *    mode, not in `RHI_EXTEND`.
 *  - On rare condition, memory allocation may fail when set
 *    is extended.
 * 
 * \param   set  Set for replacement
 * \param   key  Set mode
 * 
 * \return  On success, `true` is returned. On failure,
 *          `false` is returned.
 */
bool rhis_replace(struct rhis* set, void* key) {
  /* handling of default key values */
  if( key==DEFVAL ) {
    set->is_def_key = true;
    return true;
  }
  size_t hashval = set->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, set->size);
  for(rhiuint i=0; i<set->size; ++i) {
    if( IS_EMPTY(set->nodes[prob]) ) {
      if( set->occupied<set->max ) {
        ++set->occupied;
        set->nodes[prob] = RHIS_NODE(hashval, key);
        return true;
      }
      RHIS_REPLACE(set, hashval, key, true, false);
    }
    if( hashval==set->nodes[prob].hashval &&
        set->equal(key, set->nodes[prob].key) ) {
      if( set->keyfree!=NULL )
        set->keyfree(set->nodes[prob].key);
      set->nodes[prob] = RHIS_NODE(hashval, key);
      return true;
    }
    prob = HASHVAL_PROB(prob, set->size);
  }
  return false;
}

void* rhis_kreplace(struct rhis* set, void* key) {
  /* handling of default key values */
  if( key==DEFVAL ) {
    set->is_def_key = true;
    return DEFVAL;
  }
  size_t hashval = set->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, set->size);
  for(rhiuint i=0; i<set->size; ++i) {
    if( IS_EMPTY(set->nodes[prob]) ) {
      if( set->occupied<set->max ) {
        ++set->occupied;
        set->nodes[prob] = RHIS_NODE(hashval, key);
        return DEFVAL;
      }
      RHIS_REPLACE(set, hashval, key, DEFVAL, DEFVAL);
    }
    if( hashval==set->nodes[prob].hashval &&
        set->equal(key, set->nodes[prob].key) ) {
      void* obtained_key = set->nodes[prob].key;
      set->nodes[prob] = RHIS_NODE(hashval, key);
      return obtained_key;
    }
    prob = HASHVAL_PROB(prob, set->size);
  }
  return DEFVAL;
}


/**************************
 * Search functions (set) *
 **************************/

#define RHIS_SEARCH(_set, _key, \
  _def_key_ret, _empty_ret, _equal_ret, _def_ret) \
  do { \
    /* handling of default key values */ \
    if( (_key)==DEFVAL ) \
      return _def_key_ret; \
    size_t _hashval = (_set)->hash(_key); \
    rhiuint _prob = HASHVAL_INDEX(_hashval, (_set)->size); \
    for(rhiuint _i=0; _i<(_set)->size; ++_i) { \
      if( IS_EMPTY((_set)->nodes[_prob]) ) \
        return _empty_ret; \
      if( _hashval==(_set)->nodes[_prob].hashval && \
          (_set)->equal(_key, (_set)->nodes[_prob].key) ) \
        return _equal_ret; \
      _prob = HASHVAL_PROB(_prob, (_set)->size); \
    } \
    return _def_ret; \
  } while(0)

bool rhis_search(const struct rhis* set, const void* key) {
  RHIS_SEARCH(set, key, set->is_def_key, false, true, false);
}

const void* rhis_ksearch(const struct rhis* set, const void* key) {
  RHIS_SEARCH(set, key, DEFVAL, DEFVAL, set->nodes[_prob].key, DEFVAL);
}

/**************************
 * Delete functions (set) *
 **************************/

bool rhis_delete(struct rhis* set, void* key) {
  /* handling of default key values */
  if( key==DEFVAL ) {
    if( set->is_def_key ) {
      set->is_def_key = false;
      return true;
    }
    return false;
  }
  size_t hashval = set->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, set->size);
  for(rhiuint i=0; i<set->size; ++i) {
    if( IS_EMPTY(set->nodes[prob]) )
      return false;
    if( hashval==set->nodes[prob].hashval &&
        set->equal(key, set->nodes[prob].key) ) {
      if( set->keyfree!=NULL )
        set->keyfree(set->nodes[prob].key);
      BACKWARD_SHIFT(set, prob);
      if( --set->occupied<set->min && (set->mode&RHI_SHRINK) )
        SHRINK_NODES(set, struct rhisnode);
      return true;
    }
    prob = HASHVAL_PROB(prob, set->size);
  }
  return false;
}


/*********************************
 * Miscellaneous functions (set) *
 *********************************/

DECL_COUNT(rhis_count, struct rhis)

void rhis_free(struct rhis *set) {
  for(rhiuint i=0; i<set->size; ++i) {
    if( IS_EMPTY(set->nodes[i]) )
      continue;
    if( set->keyfree!=NULL )
      set->keyfree(set->nodes[i].key);
  }
  free(set->nodes);
  free(set);
}


/************************************
 * Forward traverse functions (set) *
 ************************************/

#define RHIS_GET(_set, _iter) \
  ((_iter)==DEFITER ? DEFVAL : (_set)->nodes[_iter].key)

DECL_BEGIN(rhis_begin, struct rhis)
DECL_NEXT(rhis_next, struct rhis)
DECL_END(rhis_end, struct rhis)

const void* rhis_current(const struct rhis* set) {
  return RHIS_GET(set, set->iter);
}


/***********************************
 * Random traverse functions (set) *
 ***********************************/

DECL_ITERS(rhis_iters, struct rhis)

const void* rhis_get(const struct rhis* set, rhiuint iter) {
  return RHIS_GET(set, iter);
}


/* ===================================Map=================================== */


/***************************
 * Initial functions (map) *
 ***************************/

#define RHIM_INIT(_func, _mode, _index) \
  do { \
    struct rhim* _map; \
    if( (_map=(struct rhim*)calloc(1, sizeof(struct rhim)))==NULL ) \
      return NULL; \
    rhiuint _initsize = GET_SIZE(_index); \
    if( (_map->nodes=(struct rhimnode*)calloc( \
        _initsize, sizeof(struct rhimnode)))==NULL ) { \
      free(_map); \
      return NULL; \
    } \
    _map->mode = (uint8_t)(_mode); \
    _map->begin_index = (uint8_t)(_index); \
    SET_BOUND(_map, _index, _initsize); \
    _map->hash = (_func)->hash; \
    _map->equal = (_func)->equal; \
    _map->keyfree = (_func)->keyfree; \
    _map->valfree = (_func)->valfree; \
    return _map; \
  } while(0)

struct rhim* rhim_init(const struct rhifunc* func, int mode) {
  RHIM_INIT(func, mode, BEGIN_INDEX);
}

struct rhim* rhim_reserve(const struct rhifunc* func, rhiuint size, int mode) {
  int index = get_index(size);
  RHIM_INIT(func, mode, index);
}


/**************************
 * Insert functions (map) *
 **************************/

DECL_EXTEND_NODES(map_extend_nodes, struct rhim, struct rhimnode)

bool rhim_insert(struct rhim* map, void* key, void* val) {
  if( key==DEFVAL ) {
    if( map->is_def_key )
      return false;
    map->is_def_key = true;
    map->def_val = val;
    return true;
  }
  if( map->occupied<map->max ||
      ((map->mode&RHI_EXTEND) && map_extend_nodes(map)) ) {
    size_t hashval = map->hash(key);
    rhiuint prob = HASHVAL_INDEX(hashval, map->size);
    for(rhiuint i=0; i<map->size; ++i) {
      if( IS_EMPTY(map->nodes[prob]) ) {
        ++map->occupied;
        map->nodes[prob] = RHIM_NODE(hashval, key, val);
        return true;
      }
      if( hashval==map->nodes[prob].hashval &&
          map->equal(key, map->nodes[prob].key) )
        return false;
      prob = HASHVAL_PROB(prob, map->size);
    }
  }
  return false;
}


/*********************
 * Replace functions *
 *********************/

#define RHIM_REPLACE(_map, _hashval, _key, _val, _ret, _def_ret) \
  do { \
    if( ((_map)->mode&RHI_EXTEND) && map_extend_nodes(_map) ) { \
      rhiuint _prob = HASHVAL_INDEX(_hashval, (_map)->size); \
      for(rhiuint _i=0; _i<(_map)->size; ++_i) { \
        if( IS_EMPTY((_map)->nodes[_prob]) ) { \
          ++(_map)->occupied; \
          (_map)->nodes[_prob] = RHIM_NODE(_hashval, _key, _val); \
          return _ret; \
        } \
        _prob = HASHVAL_PROB(_prob, (_map)->size); \
      } \
    } \
    return _def_ret; \
  } while(0)

bool rhim_replace(struct rhim* map, void* key, void* val) {
  if( key==DEFVAL ) {
    if( map->is_def_key ) {
      if( map->valfree!=NULL )
        map->valfree(map->def_val);
      map->def_val = val;
      return true;
    }
    map->is_def_key = true;
    map->def_val = val;
    return true;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, map->size);
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) ) {
      if( map->occupied<map->max ) {
        ++map->occupied;
        map->nodes[prob] = RHIM_NODE(hashval, key, val);
        return true;
      }
      RHIM_REPLACE(map, hashval, key, val, true, false);
    }
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) ) {
      if( map->keyfree!=NULL )
        map->keyfree(map->nodes[prob].key);
      if( map->valfree!=NULL )
        map->valfree(map->nodes[prob].val);
      map->nodes[prob] = RHIM_NODE(hashval, key, val);
      return true;
    }
    prob = HASHVAL_PROB(prob, map->size);
  }
  return false;
}

void* rhim_vreplace(struct rhim* map, void* key, void* val) {
  if( key==DEFVAL ) {
    if( map->is_def_key ) {
      void* obtained_val = map->def_val;
      map->def_val = val;
      return obtained_val;
    }
    map->is_def_key = true;
    map->def_val = val;
    return DEFVAL;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, map->size);
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) ) {
      if( map->occupied<map->max ) {
        ++map->occupied;
        map->nodes[prob] = RHIM_NODE(hashval, key, val);
        return DEFVAL;
      }
      RHIM_REPLACE(map, hashval, key, val, DEFVAL, DEFVAL);
    }
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) ) {
      if( map->keyfree!=NULL )
        map->keyfree(map->nodes[prob].key);
      void* obtained_val = map->nodes[prob].val;
      map->nodes[prob] = RHIM_NODE(hashval, key, val);
      return obtained_val;
    }
    prob = HASHVAL_PROB(prob, map->size);
  }
  return DEFVAL;
}

struct rhipair rhim_kvreplace(struct rhim* map, void* key, void* val) {
  if( key==DEFVAL ) {
    if( map->is_def_key ) {
      struct rhipair pair = PAIR(DEFVAL, map->def_val);
      map->def_val = val;
      return pair;
    }
    map->is_def_key = true;
    map->def_val = val;
    return DEFPAIR;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, map->size);
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) ) {
      if( map->occupied<map->max ) {
        ++map->occupied;
        map->nodes[prob] = RHIM_NODE(hashval, key, val);
        return DEFPAIR;
      }
      RHIM_REPLACE(map, hashval, key, val, DEFPAIR, DEFPAIR);
    }
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) ) {
      struct rhipair pair = PAIR(map->nodes[prob].key, map->nodes[prob].val);
      map->nodes[prob] = RHIM_NODE(hashval, key, val);
      return pair;
    }
    prob = HASHVAL_PROB(prob, map->size);
  }
  return DEFPAIR;
}


/**************************
 * Search functions (map) *
 **************************/

#define RHIM_SEARCH(_map, _key, \
  _def_key_ret, _empty_ret, _equal_ret, _def_ret) \
  do { \
    if( (_key)==DEFVAL ) \
      return _def_key_ret; \
    size_t _hashval = (_map)->hash(_key); \
    rhiuint _prob = HASHVAL_INDEX(_hashval, (_map)->size); \
    for(rhiuint _i=0; _i<(_map)->size; ++_i) { \
      if( IS_EMPTY((_map)->nodes[_prob]) ) \
        return _empty_ret; \
      if( _hashval==(_map)->nodes[_prob].hashval && \
          (_map)->equal(_key, (_map)->nodes[_prob].key) ) \
        return _equal_ret; \
      _prob = HASHVAL_PROB(_prob, (_map)->size); \
    } \
    return _def_ret; \
  } while(0)

bool rhim_search(const struct rhim* map, const void* key) {
  RHIM_SEARCH(map, key, map->is_def_key, false, true, false);
}

void* rhim_vsearch(const struct rhim* map, const void* key) {
  RHIM_SEARCH(map, key, map->is_def_key ? map->def_val : DEFVAL, DEFVAL,
    map->nodes[_prob].val, DEFVAL);
}

struct rhiconstpair rhim_kvsearch(const struct rhim* map, const void* key) {
  RHIM_SEARCH(map, key, map->is_def_key ?
    CONSTPAIR(DEFVAL, map->def_val): DEFCONSTPAIR, DEFCONSTPAIR,
    CONSTPAIR(map->nodes[_prob].key, map->nodes[_prob].val), DEFCONSTPAIR);
}


/**************************
 * Delete functions (map) *
 **************************/

bool rhim_delete(struct rhim* map, void* key) {
  if( key==DEFVAL ) {
    if( map->is_def_key ) {
      map->is_def_key = false;
      if( map->valfree!=NULL )
        map->valfree(map->def_val);
      return true;
    }
    return false;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, map->size);
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) )
      return false;
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) ) {
      if( map->keyfree!=NULL )
        map->keyfree(map->nodes[prob].key);
      if( map->valfree!=NULL )
        map->valfree(map->nodes[prob].val);
      BACKWARD_SHIFT(map, prob);
      if( --map->occupied<map->min && (map->mode&RHI_SHRINK) )
        SHRINK_NODES(map, struct rhimnode);
      return true;
    }
    prob = HASHVAL_PROB(prob, map->size);
  }
  return false;
}

void* rhim_vdelete(struct rhim* map, void* key) {
  if( key==DEFVAL ) {
    if( map->is_def_key ) {
      map->is_def_key = false;
      return map->def_val;
    }
    return DEFVAL;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, map->size);
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) )
      return DEFVAL;
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) ) {
      if( map->keyfree!=NULL )
        map->keyfree(map->nodes[prob].key);
      void* val = map->nodes[prob].val;
      BACKWARD_SHIFT(map, prob);
      if( --map->occupied<map->min && (map->mode&RHI_SHRINK) )
        SHRINK_NODES(map, struct rhimnode);
      return val;
    }
    prob = HASHVAL_PROB(prob, map->size);
  }
  return DEFVAL;
}

struct rhipair rhim_kvdelete(struct rhim* map, void* key) {
  if( key==DEFVAL ) {
    if( map->is_def_key ) {
      map->is_def_key = false;
      return PAIR(DEFVAL, map->def_val);
    }
    return DEFPAIR;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_INDEX(hashval, map->size);
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) )
      return DEFPAIR;
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) ) {
      struct rhipair pair = PAIR(map->nodes[prob].key, map->nodes[prob].val);
      BACKWARD_SHIFT(map, prob);
      if( --map->occupied<map->min && (map->mode&RHI_SHRINK) )
        SHRINK_NODES(map, struct rhimnode);
      return pair;
    }
    prob = HASHVAL_PROB(prob, map->size);
  }
  return DEFPAIR;
}


/*********************************
 * Miscellaneous functions (map) *
 *********************************/

DECL_COUNT(rhim_count, struct rhim)

void rhim_free(struct rhim *map) {
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[i]) )
      continue;
    if( map->keyfree!=NULL )
      map->keyfree(map->nodes[i].key);
    if( map->valfree!=NULL )
      map->valfree(map->nodes[i].val);
  }
  if( map->valfree!=NULL && map->is_def_key )
    map->valfree(map->def_val);
  free(map->nodes);
  free(map);
}


/************************************
 * Forward traverse functions (map) *
 ************************************/

#define RHIM_GET(_map, _iter) \
  ((_iter)==DEFITER ? \
   CONSTPAIR(DEFVAL, map->def_val) : \
   CONSTPAIR((_map)->nodes[_iter].key, (_map)->nodes[_iter].val))
#define RHIM_KGET(_map, _iter) \
  ((_iter)==DEFITER ? DEFVAL : (_map)->nodes[_iter].key)
#define RHIM_VGET(_map, _iter) \
  ((_iter)==DEFITER ? map->def_val : (_map)->nodes[_iter].val)

DECL_BEGIN(rhim_begin, struct rhim)
DECL_NEXT(rhim_next, struct rhim)
DECL_END(rhim_end, struct rhim)

struct rhiconstpair rhim_current(const struct rhim* map) {
  return RHIM_GET(map, map->iter);
}

const void* rhim_kcurrent(const struct rhim* map) {
  return RHIM_KGET(map, map->iter);
}

void* rhim_vcurrent(const struct rhim* map) {
  return RHIM_VGET(map, map->iter);
}


/***********************************
 * Random traverse functions (map) *
 ***********************************/

DECL_ITERS(rhim_iters, struct rhim)

struct rhiconstpair rhim_get(const struct rhim* map, rhiuint iter) {
  return RHIM_GET(map, iter);
}

const void* rhim_kget(const struct rhim* map, rhiuint iter) {
  return RHIM_KGET(map, iter);
}

void* rhim_vget(const struct rhim* map, rhiuint iter) {
  return RHIM_VGET(map, iter);
}

