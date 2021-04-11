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

#ifndef RHI_H
#define RHI_H

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**************************
 * Macros for Windows DLL *
 **************************/

#if (defined _WIN32 || defined _WIN64) && \
    (defined RHI_EXPORT || defined RHI_IMPORT)
# ifdef RHI_EXPORT
#  define RHI_API __declspec(dllexport)
# elif defined RHI_IMPORT
#  define RHI_API __declspec(dllimport)
# endif
#else
# define RHI_API
#endif

#ifndef _RHI_MACRO_DEFINED
#define _RHI_MACRO_DEFINED

/***********
 * Integer *
 ***********/

typedef int32_t rhiint;
typedef uint32_t rhiuint;

/**********************
 * Macros for integer *
 **********************/

/* Suffixes */
#define RHIINT_C INT32_C
#define RHIUINT_C UINT32_C

/* Maximum width */
#define RHIINT_WIDTH INT32_WIDTH
#define RHIUINT_WIDTH UINT32_WIDTH

/* Minimum value */
#define RHIINT_MIN UINT32_MIN

/* Maximum value */
#define RHIINT_MAX INT32_MAX
#define RHIUINT_MAX UINT32_MAX

/* Printing format specifiers */
#define PRIrhid PRId32
#define PRIrhii PRIi32
#define PRIrhio PRIo32
#define PRIrhiu PRIu32
#define PRIrhix PRIx32
#define PRIrhiX PRIX32

/* Scanning format specifiers */
#define SCNrhid SCNd32
#define SCNrhii SCNi32
#define SCNrhio SCNo32
#define SCNrhiu SCNu32
#define SCNrhix SCNx32

/*********
 * Modes *
 *********/

/**
 * \brief  Fixed mode
 * 
 * Set/map will not shrink and extend. If the number of
 * elements exceeds the minimum limit, Set/map size will not
 * shrink, and if the number of elements exceeds the maximum
 * limit, Set/map size will not extend.
 */
#define RHI_FIXED 0x00

/**
 * \brief  Shrink mode
 * 
 * Set/map will shrink. If the number of elements exceeds the
 * minimum limit, set/map size will shrink.
 */
#define RHI_SHRINK 0x01

/**
 * \brief  Extend mode
 * 
 * Set/map will extend. If the number of elements exceeds the
 * maximum limit, set/map size will extend.
 */
#define RHI_EXTEND 0x02

#endif /* _RHI_MACRO_DEFINED */

/*************************
 * Overridable functions *
 *************************/

/**
 * \brief   Hash function
 * 
 * Generate the hash value of key, the operation of a `NULL`
 * key will not call this function.
 * 
 * \param   key  Key to calculate
 * 
 * \return  Hash value from key calculation.
 */
typedef size_t (*rhihash)(const void* key);

/**
 * \brief   Equal comparator for keys
 * 
 * Is key equal or not? The operation of a `NULL` key will not
 * call this function.
 * 
 * \param   first_key   First key
 * \param   second_key  Second key
 * 
 * \return  Equal, `true` is returned. Not equal `false` is
 *          returned.
 */
typedef bool (*rhiequal)(const void* first_key, const void* second_key);

/**
 * \brief  Key destroyer
 * 
 * Destroy key when the delete function or destructor function
 * is called. The operation of a `NULL` key will not call this
 * function.
 * 
 * \param  key  Key to destroy
 */
typedef void (*rhikeyfree)(void* key);

/**
 * \brief  Value destroyer
 * 
 * Destroy value when the delete function or destructor
 * function is called.
 * 
 * \param  key  Value to destroy
 */
typedef void (*rhivalfree)(void* val);

/**************
 * Structures *
 **************/

/* Set */
struct rhis;

/* Map */
struct rhim;

/* Function container */
struct rhifunc {
  /**
   * \note  Required fields.
   */
  rhihash hash;
  rhiequal equal;

  /**
   * \note  If you don't want these functions to be used, set
   *        the field as `NULL`.
   */
  rhikeyfree keyfree;
  rhivalfree valfree;
};

/* Mutable key-value pair */
struct rhipair {
  void* key;
  void* val;
};

/* Immutable key-value pair */
struct rhiconstpair {
  /**
   * \note  The object pointed by this field can not be
   *        modified.
   */
  const void* key;
  void* val;
};

/* ===== Set ===== */

/*********************
 * Initial functions *
 *********************/

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
RHI_API struct rhis* rhis_init(const struct rhifunc* func, int mode);

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
RHI_API struct rhis* rhis_reserve(const struct rhifunc* func, rhiuint size, int mode);

/********************
 * Insert functions *
 ********************/

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
RHI_API bool rhis_insert(struct rhis* set, void* key);

/*********************
 * Replace functions *
 *********************/

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
 * -----------------------------------------------------------
 * \return  On success, `true` is returned. On failure,
 *          `false` is returned.
 */
RHI_API bool rhis_replace(struct rhis* set, void* key);
RHI_API void* rhis_kreplace(struct rhis* set, void* key);

// --------------------------------------------------------------------
// -----------------------------------------------------------

/**************************
 * Search functions (set) *
 **************************/

RHI_API bool rhis_search(const struct rhis* set, const void* key);
RHI_API const void* rhis_ksearch(const struct rhis* set, const void* key);

/**************************
 * Delete functions (set) *
 **************************/

RHI_API bool rhis_delete(struct rhis* set, void* key);

/*********************************
 * Miscellaneous functions (set) *
 *********************************/

RHI_API rhiuint rhis_count(const struct rhis* set);
RHI_API void rhis_free(struct rhis* set);

/************************************
 * Forward traverse functions (set) *
 ************************************/

RHI_API void rhis_begin(struct rhis* set);
RHI_API void rhis_next(struct rhis* set);
RHI_API bool rhis_end(struct rhis* set);
RHI_API const void* rhis_current(const struct rhis* set);

/***********************************
 * Random traverse functions (set) *
 ***********************************/

RHI_API rhiuint* rhis_iters(const struct rhis* set);
RHI_API const void* rhis_get(const struct rhis* set, rhiuint iter);

/* ===== Map ===== */

/***************************
 * Initial functions (map) *
 ***************************/

RHI_API struct rhim* rhim_init(const struct rhifunc* func, int mode);
RHI_API struct rhim* rhim_reserve(const struct rhifunc* func, rhiuint size, int mode);



/**************************
 * Insert functions (map) *
 **************************/

RHI_API bool rhim_insert(struct rhim* map, void* key, void* val);


/*********************
 * Replace functions *
 *********************/

RHI_API bool rhim_replace(struct rhim* map, void* key, void* val);
RHI_API void* rhim_vreplace(struct rhim* map, void* key, void* val);
RHI_API struct rhipair rhim_kvreplace(struct rhim* map, void* key, void* val);


/**************************
 * Search functions (map) *
 **************************/

RHI_API bool rhim_search(const struct rhim* map, const void* key);
RHI_API void* rhim_vsearch(const struct rhim* map, const void* key);
RHI_API struct rhiconstpair rhim_kvsearch(const struct rhim* map, const void* key);


/**************************
 * Delete functions (map) *
 **************************/

RHI_API bool rhim_delete(struct rhim* map, void* key);
RHI_API void* rhim_vdelete(struct rhim* map, void* key);
RHI_API struct rhipair rhim_kvdelete(struct rhim* map, void* key);


/*********************************
 * Miscellaneous functions (map) *
 *********************************/

RHI_API rhiuint rhim_count(const struct rhim* map);
RHI_API void rhim_free(struct rhim* map);


/************************************
 * Forward traverse functions (map) *
 ************************************/

RHI_API void rhim_begin(struct rhim* map);
RHI_API void rhim_next(struct rhim* map);
RHI_API bool rhim_end(struct rhim* map);
RHI_API struct rhiconstpair rhim_current(const struct rhim* map);
RHI_API const void* rhim_kcurrent(const struct rhim* map);
RHI_API void* rhim_vcurrent(const struct rhim* map);


/***********************************
 * Random traverse functions (map) *
 ***********************************/

RHI_API rhiuint* rhim_iters(const struct rhim* map);
RHI_API struct rhiconstpair rhim_get(const struct rhim* map, rhiuint iter);
RHI_API const void* rhim_kget(const struct rhim* map, rhiuint iter);
RHI_API void* rhim_vget(const struct rhim* map, rhiuint iter);

#ifdef __cplusplus
}
#endif

#endif /* RHI_H */
