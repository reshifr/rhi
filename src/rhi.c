/**
 * Reshifr Hash Index
 * Copyright (c) 2019 Renol P. H. <reshifr@gmail.com>
 * 
 * MIT License
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <rhi.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef RHI_PRIME
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

# define BEGIN_RANGE 0
# define END_RANGE 28
# define GET_SIZE(_range) (range[_range])
# define HASHVAL_TO_INDEX(_hashval, _size) ((rhiuint)((_hashval)%(_size)))
#else
# define BEGIN_RANGE 3
# define END_RANGE 31
# define GET_SIZE(_range) ((rhiuint)1<<(_range))
# define HASHVAL_TO_INDEX(_hashval, _size) ((rhiuint)((_hashval)&((_size)-1)))
#endif

#define OOM_ERROR_MESSAGE \
  "Error: Used system memory has run out."
#define RANGE_ERROR_MESSAGE \
  "Error: The number of elements exceeded the range possible."
#define ERROR(_message) \
  do { \
    fprintf(stderr, "%s\n", _message); \
    exit(EXIT_FAILURE); \
  } while(0)

/***************
 * Node of set *
 ***************/

struct rhisnode {
  size_t hashval;  /* The hash value of the key. */
  void* key;  /* The key that was stored. */
};

/*******
 * Set *
 *******/

struct rhis {
  uint8_t mode;  /* The shrink/extend method of the table. */
  uint8_t begin_range;  /* The begin range. */
  uint8_t curr_range;  /* The current range. */
  uint8_t has_null_inserted: 4;  /* Has NULL key been inserted? */
  uint8_t has_iter_ended: 4;  /* Has the iterator ended? */
  rhiuint iter_index;  /* The iterator index. */
  rhiuint min;  /* The minimum number of elements of the nodes. */
  rhiuint max;  /* The maximum number of elements of the nodes. */
  rhiuint size;  /* The current number of nodes in the table. */
  rhiuint occupied;  /* The number of elements occupied in the nodes. */
  rhihash hash;  /* The key hash function applied to the table. */
  rhiequal equal;  /* The key equal function is applied to the table. */
  rhikeyfree free;  /* The key destroy function is applied to the table. */
  struct rhisnode* nodes;  /* An array of nodes. */
};

/**************
 * Set macros *
 **************/

#define RHIS_NODE(_hashval, _key) \
  ((struct rhisnode){ .hashval=_hashval, .key=_key })

/***************
 * Node of map *
 ***************/

struct rhimnode {
  size_t hashval;  /* The hash value of the key. */
  void* key;  /* The key that was stored. */
  void* val;  /* The value that was stored. */
};

/*******
 * Map *
 *******/

struct rhim {
  uint8_t mode;  /* The shrink/extend method of the table. */
  uint8_t begin_range;  /* The begin range. */
  uint8_t curr_range;  /* The current range. */
  uint8_t has_null_inserted: 4;  /* Has NULL key been inserted? */
  uint8_t has_iter_ended: 4;  /* Has the iterator ended? */
  rhiuint iter_index;  /* The iterator index. */
  rhiuint min;  /* The minimum number of elements of the nodes. */
  rhiuint max;  /* The maximum number of elements of the nodes. */
  rhiuint size;  /* The current number of nodes in the table. */
  rhiuint occupied;  /* The number of elements occupied in the nodes. */
  rhihash hash;  /* The key hash function applied to the table. */
  rhiequal equal;  /* The key equal function is applied to the table. */
  rhikeyfree free_key;  /* The key destroy function is applied to the table. */
  rhivalfree free_val;  /* The value destroy function is applied to the table. */
  void* null_val;  /* The value pair of NULL key. */
  struct rhimnode* nodes;  /* An array of nodes. */
};

/**************
 * Map macros *
 **************/

#define PAIR(_key, _val) ((struct rhipair){ .key=_key, .val=_val })
#define RHIM_NODE(_hashval, _key, _val) \
  ((struct rhimnode){ .hashval=_hashval, .key=_key, .val=_val })

/****************
 * Table macros *
 ****************/

#define MIN_LOAD 0.18
#define MAX_LOAD 0.72
#define BEGIN_EXPONENT 3
#define NULL_ITER RHIUINT_MAX
#define IS_EMPTY(_node) ((_node).key==NULL)
#define SET_EMPTY(_node) ((_node).key=NULL)
#define NOT_EMPTY(_node) ((_node).key!=NULL)
#define HASHVAL_TO_PROB(_prob, _size) HASHVAL_TO_INDEX((_prob)+1, _size)
#define MIN_OCCUPIED(_curr_range, _begin_range) \
  ((_curr_range)==(_begin_range) ? \
   RHIUINT_C(0) : ((rhiuint)((double)GET_SIZE(_curr_range)*MIN_LOAD)))
#define MAX_OCCUPIED(_curr_range) \
  ((rhiuint)((double)GET_SIZE(_curr_range)*MAX_LOAD))
#define COUNT(_table) ((_table)->has_null_inserted ? \
  (_table)->occupied+1 : (_table)->occupied)

/**
 * Set the index, min, max, and size fields.
 */
#define SET_BOUNDARY(_table, _curr_range, _size) \
  do { \
    (_table)->curr_range = (uint8_t)(_curr_range); \
    (_table)->min = MIN_OCCUPIED(_curr_range, (_table)->begin_range); \
    (_table)->max = MAX_OCCUPIED(_curr_range); \
    (_table)->size = _size; \
  } while(0)

/**
 * Get the value of floor(log2(size)).
 */
#define MSB_RANGE(_curr_range, _size) \
  do { \
    (_curr_range) = 0; \
    if( (_size)>=RHIUINT_C(0x00010000) ) \
      (_curr_range) += 16, (_size) >>= 16; \
    if( (_size)>=RHIUINT_C(0x00000100) ) \
      (_curr_range) += 8, (_size) >>= 8; \
    if( (_size)>=RHIUINT_C(0x00000010) ) \
      (_curr_range) += 4, (_size) >>= 4; \
    if( (_size)>=RHIUINT_C(0x00000004) ) \
      (_curr_range) += 2, (_size) >>= 2; \
    if( (_size)>=RHIUINT_C(0x00000002) ) \
      (_curr_range) += 1, (_size) >>= 1; \
  } while(0)

/**
 * Get the appropriate range from the given size.
 */
#define GET_RANGE(_curr_range, _size) \
  do { \
    rhiuint _target_size = (rhiuint)((double)(_size)/MAX_LOAD); \
    MSB_RANGE(_curr_range, _target_size); \
    (_curr_range) -= BEGIN_EXPONENT-BEGIN_RANGE; \
    if( (_curr_range)<BEGIN_RANGE ) \
      (_curr_range) = BEGIN_RANGE; \
    while( (_curr_range)<END_RANGE && size>MAX_OCCUPIED(_curr_range) ) \
      ++(_curr_range); \
  } while(0)

/**
 * Move current elements to the new array of nodes.
 */
#define MOVE_NODES(_table, _new_size, _new_nodes, __node) \
  do { \
    for(rhiuint _i=0; _i<(_table)->size; ++_i) { \
      if( IS_EMPTY((_table)->nodes[_i]) ) \
        continue; \
      __node _elm = (_table)->nodes[_i]; \
      rhiuint _curr_origin = HASHVAL_TO_INDEX(_elm.hashval, _new_size); \
      rhiuint _prob = _curr_origin; \
      /* Insertion of elements from the nodes to the new nodes. */ \
      for(;;) { \
        if( IS_EMPTY((_new_nodes)[_prob]) ) { \
          (_new_nodes)[_prob] = _elm; \
          break; \
        } \
        rhiuint _cmp_origin = \
          HASHVAL_TO_INDEX((_new_nodes)[_prob].hashval, _new_size); \
        if( _curr_origin<_cmp_origin ) { \
          __node _tmp_elm = _elm; \
          _elm = (_new_nodes)[_prob]; \
          (_new_nodes)[_prob] = _tmp_elm; \
          _curr_origin = _cmp_origin; \
        } \
        _prob = HASHVAL_TO_PROB(_prob, _new_size); \
      } \
    } \
    free((_table)->nodes); \
    (_table)->nodes = _new_nodes; \
  } while(0)

/**
 * Extend the current array of nodes when it reaches its size >
 * its maximum size.
 */
#define EXTEND_NODES(_table, __node) \
  do { \
    if( (_table)->curr_range==END_RANGE ) \
      ERROR(RANGE_ERROR_MESSAGE); \
    __node* _new_nodes; \
    int _new_range = (_table)->curr_range+1; \
    rhiuint _new_size = GET_SIZE(_new_range); \
    if( (_new_nodes=(__node*)calloc(_new_size, sizeof(__node)))==NULL ) \
      ERROR(OOM_ERROR_MESSAGE); \
    MOVE_NODES(_table, _new_size, _new_nodes, __node); \
    SET_BOUNDARY(_table, _new_range, _new_size); \
  } while(0)

#define SEARCH(_table, _key, _null_ret, _equal_ret, _fail_ret) \
  do { \
    /* Handling NULL key. */ \
    if( (_key)==NULL ) \
      return _null_ret; \
    size_t _hashval = (_table)->hash(_key); \
    rhiuint _curr_origin = HASHVAL_TO_INDEX(_hashval, (_table)->size); \
    rhiuint _prob = _curr_origin; \
    while( NOT_EMPTY((_table)->nodes[_prob]) && \
           _curr_origin>=HASHVAL_TO_INDEX( \
             (_table)->nodes[_prob].hashval, \
             (_table)->size \
           ) ) { \
      if( _hashval==(_table)->nodes[_prob].hashval && \
          (_table)->equal(_key, (_table)->nodes[_prob].key) ) \
        return _equal_ret; \
      _prob = HASHVAL_TO_PROB(_prob, (_table)->size); \
    } \
    return _fail_ret; \
  } while(0)

/**
 * Shrink the current array of nodes when it reaches its size <
 * its minimum size.
 */
#define SHRINK_NODES(_table, __node) \
  do { \
    if( (_table)->curr_range==(_table)->begin_range ) \
      break; \
    __node* _new_nodes; \
    int _new_range = (_table)->curr_range-1; \
    rhiuint _new_size = GET_SIZE(_new_range); \
    if( (_new_nodes=(__node*)calloc(_new_size, sizeof(__node)))==NULL ) \
      break; \
    MOVE_NODES(_table, _new_size, _new_nodes, __node); \
    SET_BOUNDARY(_table, _new_range, _new_size); \
  } while(0)

/**
 * If the subsequent element is in prob sequence, shift the
 * element to an empty node, and do so until you find an empty
 * node when continuing the step for the next subsequent
 * element.
 */
#define BACKWARD_SHIFT(_table, _equal_prob) \
  do { \
    rhiuint _empty = _equal_prob; \
    for(;;) { \
      rhiuint _prob = HASHVAL_TO_PROB(_prob, (_table)->size); \
      if( IS_EMPTY((_table)->nodes[_prob]) ) { \
        SET_EMPTY((_table)->nodes[_empty]); \
        break; \
      } \
      rhiuint _origin = \
        HASHVAL_TO_INDEX((_table)->nodes[_prob].hashval, (_table)->size); \
      if( _empty<=_prob ? \
          _empty<_origin && _origin<=_prob : \
          _empty<_origin || _origin<=_prob ) \
        continue; \
      (_table)->nodes[_empty] = (_table)->nodes[_prob]; \
      _empty = _prob; \
    } \
  } while(0)

#define __COUNT(__func, __table) \
  rhiuint __func(const __table* table) { \
    return COUNT(table); \
  }

#define __BEGIN(__func, __table) \
  void __func(__table* table) { \
    if( table->has_null_inserted ) { \
      table->has_iter_ended = false; \
      table->iter_index = NULL_ITER; \
      return; \
    } \
    for(rhiuint i=0; i<table->size; ++i) { \
      if( IS_EMPTY(table->nodes[i]) ) \
        continue; \
      table->has_iter_ended = false; \
      table->iter_index = i; \
      break; \
    } \
    table->has_iter_ended = true; \
  }

#define __NEXT(__func, __table) \
  void __func(__table* table) { \
    rhiuint i = table->iter_index==NULL_ITER ? 0 : table->iter_index+1; \
    for(; i<table->size; ++i) { \
      if( IS_EMPTY(table->nodes[i]) ) \
        continue; \
      table->iter_index = i; \
      return; \
    } \
    table->has_iter_ended = true; \
  }

#define __HAS_ENDED(__func, __table) \
  bool __func(const __table* table) { \
    return table->has_iter_ended; \
  }

/* ===== Set ===== */

/****************************
 * Initialization functions *
 ****************************/

#define RHIS_INIT(_hash, _equal, _free, _mode, _curr_range) \
  do { \
    struct rhis* _set; \
    if( (_set=(struct rhis*)calloc(1, sizeof(struct rhis)))==NULL ) \
      return NULL; \
    rhiuint _init_size = GET_SIZE(_curr_range); \
    if( (_set->nodes=(struct rhisnode*)calloc( \
         _init_size, sizeof(struct rhisnode)))==NULL ) { \
      free(_set); \
      return NULL; \
    } \
    _set->mode = (uint8_t)(_mode); \
    _set->begin_range = (uint8_t)(_curr_range); \
    SET_BOUNDARY(_set, _curr_range, _init_size); \
    _set->hash = _hash; \
    _set->equal = _equal; \
    _set->free = _free; \
    return _set; \
  } while(0)

/**
 * \brief   Create a table.
 * 
 * The table will be initialized at default size.
 * 
 * \param   hash    Hash function
 * \param   equal   Equal function
 * \param   free    Key destroyer function. If not needed, set
 *                  the argument as NULL.
 * \param   mode    Mode
 * 
 * \return  On success, pointer of table is returned. On
 *          failure, NULL is returned.
 */
struct rhis* rhis_init(
  rhihash hash,
  rhiequal equal,
  rhikeyfree free,
  int mode
) {
  RHIS_INIT(hash, equal, free, mode, BEGIN_RANGE);
}

/**
 * \brief   Reserve a table.
 * 
 * The table will be initialized to the specified size, the
 * size of which is set >= specified size. The maximum table
 * size (RHI_PRIME enabled) is 1,546,188,225 + 1 (+ NULL key),
 * and the default is 1,546,188,226 + 1.
 * 
 * \param   hash    Hash function
 * \param   equal   Equal function
 * \param   free    Key destroyer function. If not needed, set
 *                  the argument as NULL.
 * \param   size    Reserved size
 * \param   mode    Mode
 * 
 * \return  On success, pointer of table is returned. On
 *          failure, NULL is returned.
 */
struct rhis* rhis_reserve(
  rhihash hash,
  rhiequal equal,
  rhikeyfree free,
  rhiuint size,
  int mode
) {
  int curr_range;
  GET_RANGE(curr_range, size);
  RHIS_INIT(hash, equal, free, mode, curr_range);
}

/***********************
 * Insertion functions *
 ***********************/

/**
 * \brief   Insert the key into the table.
 * 
 * Insertion failed due to:
 *  - The key is already in the table,
 *  - When the mode is not set with RHI_EXTEND and the maximum
 *    number of elements is reached.
 * 
 * \param   set   Table
 * \param   key   Key
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
bool rhis_insert(struct rhis* set, void* key) {
  /* Handling NULL key. */
  if( key==NULL ) {
    if( set->has_null_inserted )
      return false;
    set->has_null_inserted = true;
    return true;
  }
  /* Insertion when the nodes are full. */
  if( set->occupied==set->max ) {
    if( set->mode&RHI_EXTEND ) {
      size_t hashval = set->hash(key);
      rhiuint curr_origin = HASHVAL_TO_INDEX(hashval, set->size);
      rhiuint prob = curr_origin;
      /* In the nodes, search for the equal key. */
      while( NOT_EMPTY(set->nodes[prob]) &&
             curr_origin>=HASHVAL_TO_INDEX(
               set->nodes[prob].hashval,
               set->size
             ) ) {
        if( hashval==set->nodes[prob].hashval &&
            set->equal(key, set->nodes[prob].key) )
          return false;
        prob = HASHVAL_TO_PROB(prob, set->size);
      }
      EXTEND_NODES(set, struct rhisnode);
      struct rhisnode elm = RHIS_NODE(hashval, key);
      curr_origin = HASHVAL_TO_INDEX(hashval, set->size);
      prob = curr_origin;
      /* Insertion when the nodes have been extended. */
      for(;;) {
        if( IS_EMPTY(set->nodes[prob]) ) {
          ++set->occupied;
          set->nodes[prob] = elm;
          return true;
        }
        rhiuint cmp_origin =
          HASHVAL_TO_INDEX(set->nodes[prob].hashval, set->size);
        if( curr_origin<cmp_origin ) {
          struct rhisnode tmp_elm = elm;
          elm = set->nodes[prob];
          set->nodes[prob] = tmp_elm;
          curr_origin = cmp_origin;
        }
        prob = HASHVAL_TO_PROB(prob, set->size);
      }
    }
    return false;
  }
  struct rhisnode elm = RHIS_NODE(set->hash(key), key);
  rhiuint curr_origin = HASHVAL_TO_INDEX(elm.hashval, set->size);
  rhiuint prob = curr_origin;
  /* Insertion when the nodes are not full. */
  for(;;) {
    if( IS_EMPTY(set->nodes[prob]) ) {
      ++set->occupied;
      set->nodes[prob] = elm;
      return true;
    }
    if( elm.hashval==set->nodes[prob].hashval &&
        set->equal(elm.key, set->nodes[prob].key) )
      return false;
    rhiuint cmp_origin = HASHVAL_TO_INDEX(set->nodes[prob].hashval, set->size);
    if( curr_origin<cmp_origin ) {
      struct rhisnode tmp_elm = elm;
      elm = set->nodes[prob];
      set->nodes[prob] = tmp_elm;
      curr_origin = cmp_origin;
    }
    prob = HASHVAL_TO_PROB(prob, set->size);
  }
  return false;
}

/********************
 * Search functions *
 ********************/

/**
 * \brief   Search the key in the table.
 * 
 * Search failed because the given key is not in the table.
 * 
 * \param   set   Table
 * \param   key   Key
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
bool rhis_search(const struct rhis* set, const void* key) {
  SEARCH(set, key, set->has_null_inserted, true, false);
}

/**********************
 * Deletion functions *
 **********************/

/**
 * \brief   Delete the key from the table.
 * 
 * If free() is not set as NULL, the given key will be
 * destroyed by free(). Deletion failed because the given key
 * is not in the table.
 * 
 * \param   set   Table
 * \param   key   Key
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
bool rhis_delete(struct rhis* set, void* key) {
  /* Handling NULL key. */
  if( key==NULL ) {
    if( set->has_null_inserted ) {
      set->has_null_inserted = false;
      return true;
    }
    return false;
  }
  size_t hashval = set->hash(key);
  rhiuint curr_origin = HASHVAL_TO_INDEX(hashval, set->size);
  rhiuint prob = curr_origin;
  while( NOT_EMPTY(set->nodes[prob]) &&
         curr_origin>=HASHVAL_TO_INDEX(set->nodes[prob].hashval, set->size) ) {
    if( hashval==set->nodes[prob].hashval &&
        set->equal(key, set->nodes[prob].key) ) {
      if( set->free!=NULL )
        set->free(set->nodes[prob].key);
      BACKWARD_SHIFT(set, prob);
      if( --set->occupied<set->min && (set->mode&RHI_SHRINK) )
        SHRINK_NODES(set, struct rhisnode);
      return true;
    }
    prob = HASHVAL_TO_PROB(prob, set->size);
  }
  return false;
}

/***********************
 * Traversal functions *
 ***********************/

/**
 * \brief  Initialize the iterator.
 * 
 * Iterator initialization on the first element, the first
 * element at the beginning is NULL key.
 * 
 * \param  set   Table
 */
__BEGIN(rhis_begin, struct rhis)

/**
 * \brief  Shift the iterator.
 * 
 * The iterator will move towards the next element in the
 * table.
 * 
 * \param  set   Table
 */
__NEXT(rhis_next, struct rhis)

/**
 * \brief   Check the iterator.
 * 
 * Check if the iterator has ended.
 * 
 * \param   set   Table
 * 
 * \return  If ended, true is returned. Else, false is
 *          returned.
 */
__HAS_ENDED(rhis_has_ended, struct rhis)

/**
 * \brief   Get the current key.
 * 
 * Get the key by the current iterator position in the table.
 * If the iterator is ended, NULL is returned.
 * 
 * \param   set   Table
 * 
 * \return  On success, the current key is returned. On
 *          failure, NULL is returned.
 */
const void* rhis_current(const struct rhis* set) {
  return set->iter_index==NULL_ITER ? NULL : set->nodes[set->iter_index].key;
}

/***************************
 * Miscellaneous functions *
 ***************************/

/**
 * \brief   Get the number of elements in the table.
 * 
 * The number of elements acquired is the whole element,
 * including NULL key.
 * 
 * \param   set   Table
 * 
 * \return  Number of elements in the table.
 */
__COUNT(rhis_count, struct rhis)

/**
 * \brief  Destroy the table.
 * 
 * Memory release used by the table. If free() is not set as
 * NULL, the whole key will be destroyed by free().
 * 
 * \param  set   Table
 */
void rhis_free(struct rhis *set) {
  for(rhiuint i=0; i<set->size; ++i) {
    if( IS_EMPTY(set->nodes[i]) )
      continue;
    if( set->free!=NULL )
      set->free(set->nodes[i].key);
  }
  free(set->nodes);
  free(set);
}

/* ===== Map ===== */

/****************************
 * Initialization functions *
 ****************************/

#define RHIM_INIT(_hash, _equal, _free_key, _free_val, _mode, _curr_range) \
  do { \
    struct rhim* _map; \
    if( (_map=(struct rhim*)calloc(1, sizeof(struct rhim)))==NULL ) \
      return NULL; \
    rhiuint _init_size = GET_SIZE(_curr_range); \
    if( (_map->nodes=(struct rhimnode*)calloc( \
         _init_size, sizeof(struct rhimnode)))==NULL ) { \
      free(_map); \
      return NULL; \
    } \
    _map->mode = (uint8_t)(_mode); \
    _map->begin_range = (uint8_t)(_curr_range); \
    SET_BOUNDARY(_map, _curr_range, _init_size); \
    _map->hash = _hash; \
    _map->equal = _equal; \
    _map->free_key = _free_key; \
    _map->free_val = _free_val; \
    return _map; \
  } while(0)

/**
 * \brief   Create a table.
 * 
 * The table will be initialized at default size.
 * 
 * \param   hash       Hash function
 * \param   equal      Equal function
 * \param   free_key   Key destroyer function. If not needed,
 *                     set the argument as NULL.
 * \param   free_val   Value destroyer function. If not needed,
 *                     set the argument as NULL.
 * \param   mode       Mode
 * 
 * \return  On success, pointer of table is returned. On
 *          failure, NULL is returned.
 */
struct rhim* rhim_init(
  rhihash hash,
  rhiequal equal,
  rhikeyfree free_key,
  rhivalfree free_val,
  int mode
) {
  RHIM_INIT(hash, equal, free_key, free_val, mode, BEGIN_RANGE);
}

/**
 * \brief   Reserve a table.
 * 
 * The table will be initialized to the specified size, the
 * size of which is set >= specified size. The maximum table
 * size (RHI_PRIME enabled) is 1,546,188,225 + 1 (+ NULL key),
 * and the default is 1,546,188,226 + 1.
 * 
 * \param   hash       Hash function
 * \param   equal      Equal function
 * \param   free_key   Key destroyer function. If not needed,
 *                     set the argument as NULL.
 * \param   free_val   Value destroyer function. If not needed,
 *                     set the argument as NULL.
 * \param   size       Reserved size
 * \param   mode       Mode
 * 
 * \return  On success, pointer of table is returned. On
 *          failure, NULL is returned.
 */
struct rhim* rhim_reserve(
  rhihash hash,
  rhiequal equal,
  rhikeyfree free_key,
  rhivalfree free_val,
  rhiuint size,
  int mode
) {
  int curr_range;
  GET_INDEX(curr_range, size);
  RHIM_INIT(hash, equal, free_key, free_val, mode, curr_range);
}

/***********************
 * Insertion functions *
 ***********************/

/**
 * \brief   Insert the key and value into the table.
 * 
 * Insertion failed due to:
 *  - The key is already in the table,
 *  - When the mode is not set with RHI_EXTEND and the maximum
 *    number of elements is reached.
 * 
 * \param   map   Table
 * \param   key   Key
 * \param   val   Value
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
bool rhim_insert(struct rhim* map, void* key, void* val) {
  /* Handling NULL key. */
  if( key==NULL ) {
    if( map->has_null_inserted )
      return false;
    map->has_null_inserted = true;
    map->null_val = val;
    return true;
  }
  /* Insertion when the nodes are full. */
  if( map->occupied==map->max ) {
    if( map->mode&RHI_EXTEND ) {
      size_t hashval = map->hash(key);
      rhiuint prob = HASHVAL_TO_INDEX(hashval, map->size);
      /* In the nodes, search for the equal key. */
      for(rhiuint i=0; i<map->size; ++i) {
        if( IS_EMPTY(map->nodes[prob]) ) {
          EXTEND_NODES(map, struct rhimnode);
          prob = HASHVAL_TO_INDEX(hashval, map->size);
          /* Insertion when the nodes have been extended. */
          for(i=0; i<map->size; ++i) {
            if( IS_EMPTY(map->nodes[prob]) ) {
              ++map->occupied;
              map->nodes[prob] = RHIM_NODE(hashval, key, val);
              return true;
            }
            prob = HASHVAL_TO_PROB(prob, map->size);
          }
          return false;
        }
        if( hashval==map->nodes[prob].hashval &&
            map->equal(key, map->nodes[prob].key) )
          return false;
        prob = HASHVAL_TO_PROB(prob, map->size);
      }
      return false;
    }
    return false;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_TO_INDEX(hashval, map->size);
  /* Insertion when the nodes are not full. */
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) ) {
      ++map->occupied;
      map->nodes[prob] = RHIM_NODE(hashval, key, val);
      return true;
    }
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) )
      return false;
    prob = HASHVAL_TO_PROB(prob, map->size);
  }
  return false;
}

/********************
 * Search functions *
 ********************/

/**
 * \brief   Search the key in the table.
 * 
 * Search failed because the given key is not in the table.
 * 
 * \param   map   Table
 * \param   key   Key
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
bool rhim_search(const struct rhim* map, const void* key) {
  SEARCH(map, key, map->has_null_inserted, false, true, false);
}

/**
 * \brief   Search the value in the table.
 * 
 * Search failed because the given key is not in the table.
 * 
 * \param   map   Table
 * \param   key   Key
 * 
 * \return  On success, the searched value is returned. On
 *          failure, NULL is returned.
 */
void* rhim_search_val(const struct rhim* map, const void* key) {
  SEARCH(
    map,
    key,
    map->has_null_inserted ? map->null_val : NULL,
    NULL,
    map->nodes[_prob].val,
    NULL
  );
}

/*************************
 * Replacement functions *
 *************************/

/**
 * \brief   Replace the key and value in the table.
 * 
 * If free_key() is not set as NULL, the old key is replaced by
 * the given key. The old key was destroyed by free_key(). If
 * free_val() is not set as NULL, the old value is replaced by
 * the given value. The old value was destroyed by free_val().
 * 
 * Replacement fails if the unique key is inserted when the
 * mode is not set with RHI_EXTEND and the maximum number of
 * elements is reached.
 * 
 * \param   set   Table
 * \param   key   Key
 * \param   val   Value
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
bool rhim_replace(struct rhim* map, void* key, void* val) {
  /* Handling NULL key. */
  if( key==NULL ) {
    if( map->has_null_inserted ) {
      if( map->free_val!=NULL )
        map->free_val(map->null_val);
      map->null_val = val;
      return true;
    }
    map->has_null_inserted = true;
    map->null_val = val;
    return true;
  }
  size_t hashval = map->hash(key);
  rhiuint prob = HASHVAL_TO_INDEX(hashval, map->size);
  for(rhiuint i=0; i<map->size; ++i) {
    if( IS_EMPTY(map->nodes[prob]) ) {
      if( map->occupied<map->max ) {
        ++map->occupied;
        map->nodes[prob] = RHIM_NODE(hashval, key, val);
        return true;
      }
      RHIM_INSERT(map, hashval, key, val, true, false);
    }
    if( hashval==map->nodes[prob].hashval &&
        map->equal(key, map->nodes[prob].key) ) {
      if( map->free_key!=NULL )
        map->free_key(map->nodes[prob].key);
      if( map->free_val!=NULL )
        map->free_val(map->nodes[prob].val);
      map->nodes[prob] = RHIM_NODE(hashval, key, val);
      return true;
    }
    prob = HASHVAL_TO_PROB(prob, map->size);
  }
  return false;
}

/**
 * \brief   Replace the value in the table.
 * 
 * If free_key() is not set as NULL, the old key is replaced by
 * the given key. The old key was destroyed by free_key().
 * 
 * The old value is replaced by the given value. Replacement
 * fails if the unique key is inserted when the mode is not set with
 * RHI_EXTEND and the maximum number of elements is reached.
 * 
 * \param   set   Table
 * \param   key   Key
 * 
 * \return  On success, the old key is returned. On failure,
 *          NULL is returned.
 */

// /**************************
//  * Delete functions (map) *
//  **************************/

// bool rhim_delete(struct rhim* map, void* key) {
//   if( key==DEFVAL ) {
//     if( map->is_def_key ) {
//       map->is_def_key = false;
//       if( map->valfree!=NULL )
//         map->valfree(map->def_val);
//       return true;
//     }
//     return false;
//   }
//   size_t hashval = map->hash(key);
//   rhiuint prob = HASHVAL_INDEX(hashval, map->size);
//   for(rhiuint i=0; i<map->size; ++i) {
//     if( IS_EMPTY(map->nodes[prob]) )
//       return false;
//     if( hashval==map->nodes[prob].hashval &&
//         map->equal(key, map->nodes[prob].key) ) {
//       if( map->keyfree!=NULL )
//         map->keyfree(map->nodes[prob].key);
//       if( map->valfree!=NULL )
//         map->valfree(map->nodes[prob].val);
//       BACKWARD_SHIFT(map, prob);
//       if( --map->occupied<map->min && (map->mode&RHI_SHRINK) )
//         SHRINK_NODES(map, struct rhimnode);
//       return true;
//     }
//     prob = HASHVAL_PROB(prob, map->size);
//   }
//   return false;
// }

// void* rhim_vdelete(struct rhim* map, void* key) {
//   if( key==DEFVAL ) {
//     if( map->is_def_key ) {
//       map->is_def_key = false;
//       return map->def_val;
//     }
//     return DEFVAL;
//   }
//   size_t hashval = map->hash(key);
//   rhiuint prob = HASHVAL_INDEX(hashval, map->size);
//   for(rhiuint i=0; i<map->size; ++i) {
//     if( IS_EMPTY(map->nodes[prob]) )
//       return DEFVAL;
//     if( hashval==map->nodes[prob].hashval &&
//         map->equal(key, map->nodes[prob].key) ) {
//       if( map->keyfree!=NULL )
//         map->keyfree(map->nodes[prob].key);
//       void* val = map->nodes[prob].val;
//       BACKWARD_SHIFT(map, prob);
//       if( --map->occupied<map->min && (map->mode&RHI_SHRINK) )
//         SHRINK_NODES(map, struct rhimnode);
//       return val;
//     }
//     prob = HASHVAL_PROB(prob, map->size);
//   }
//   return DEFVAL;
// }


// /*********************************
//  * Miscellaneous functions (map) *
//  *********************************/

// DECL_COUNT(rhim_count, struct rhim)

// void rhim_free(struct rhim *map) {
//   for(rhiuint i=0; i<map->size; ++i) {
//     if( IS_EMPTY(map->nodes[i]) )
//       continue;
//     if( map->keyfree!=NULL )
//       map->keyfree(map->nodes[i].key);
//     if( map->valfree!=NULL )
//       map->valfree(map->nodes[i].val);
//   }
//   if( map->valfree!=NULL && map->is_def_key )
//     map->valfree(map->def_val);
//   free(map->nodes);
//   free(map);
// }


// /************************************
//  * Forward traverse functions (map) *
//  ************************************/

// #define RHIM_GET(_map, _iter) \
//   ((_iter)==DEFITER ? \
//    CONSTPAIR(DEFVAL, map->def_val) : \
//    CONSTPAIR((_map)->nodes[_iter].key, (_map)->nodes[_iter].val))
// #define RHIM_KGET(_map, _iter) \
//   ((_iter)==DEFITER ? DEFVAL : (_map)->nodes[_iter].key)
// #define RHIM_VGET(_map, _iter) \
//   ((_iter)==DEFITER ? map->def_val : (_map)->nodes[_iter].val)

// DECL_BEGIN(rhim_begin, struct rhim)
// DECL_NEXT(rhim_next, struct rhim)
// DECL_END(rhim_end, struct rhim)

// struct rhiconstpair rhim_current(const struct rhim* map) {
//   return RHIM_GET(map, map->iter);
// }

// const void* rhim_kcurrent(const struct rhim* map) {
//   return RHIM_KGET(map, map->iter);
// }

// void* rhim_vcurrent(const struct rhim* map) {
//   return RHIM_VGET(map, map->iter);
// }