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
 * the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to
 * do so, subject to the following conditions:
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

#ifndef RHI_H
#define RHI_H

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************
 * Windows import and export *
 *****************************/

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

/**************
 * Data types *
 **************/

typedef int32_t rhiint;  /* Signed integer data type. */
typedef uint32_t rhiuint;  /* Unsigned integer data type. */

/**
 * Suffix macros.
 */
#define RHIINT_C INT32_C
#define RHIUINT_C UINT32_C

/**
 * Maximum width macros.
 */
#define RHIINT_WIDTH INT32_WIDTH
#define RHIUINT_WIDTH UINT32_WIDTH

/**
 * Minimum value macros.
 */
#define RHIINT_MIN UINT32_MIN

/**
 * Maximum value macros.
 */
#define RHIINT_MAX INT32_MAX
#define RHIUINT_MAX UINT32_MAX

/**
 * Printing format specifiers.
 */
#define PRIrhid PRId32
#define PRIrhii PRIi32
#define PRIrhio PRIo32
#define PRIrhiu PRIu32
#define PRIrhix PRIx32
#define PRIrhiX PRIX32

/**
 * Scanning format specifiers.
 */
#define SCNrhid SCNd32
#define SCNrhii SCNi32
#define SCNrhio SCNo32
#define SCNrhiu SCNu32
#define SCNrhix SCNx32

/*********
 * Modes *
 *********/

/**
 * \brief  Fixed mode.
 * 
 * The table size is fixed and will not change.
 */
#define RHI_FIXED 0x00

/**
 * \brief  Shrink mode.
 * 
 * The table will shrink when the number of elements reaches
 * the minimum limit.
 */
#define RHI_SHRINK 0x01

/**
 * \brief  Extend mode.
 * 
 * The table will be extended when the number of elements
 * reaches the maximum limit.
 */
#define RHI_EXTEND 0x02

#endif /* _RHI_MACRO_DEFINED */

/************************
 * Overridden functions *
 ************************/

/**
 * \brief   Hash function.
 * 
 * Generate the hash value of the key. If the key is NULL,
 * then this function is not called.
 * 
 * \param   key  Key
 * 
 * \return  Hash value
 */
typedef size_t (*rhihash)(const void* key);

/**
 * \brief   Equal function.
 * 
 * Compare two keys, equal or not. If the key is NULL, then
 * this function is not called.
 * 
 * \param   first_key   First key
 * \param   second_key  Second key
 * 
 * \return  Equal, true is returned. Not equal, false is
 *          returned.
 */
typedef bool (*rhiequal)(const void* first_key, const void* second_key);

/**
 * \brief  Key destroyer function.
 * 
 * Destroy keys when replacing, deleting, or destructing them.
 * If the key is NULL, then this function is not called.
 * 
 * \param  key  Key
 */
typedef void (*rhikeyfree)(void* key);

/**
 * \brief  Value destroyer function.
 * 
 * Destroy values when replacing, deleting, or destructing
 * them.
 * 
 * \param  val  Value
 */
typedef void (*rhivalfree)(void* val);

/*******************
 * Data structures *
 *******************/


struct rhis;  /* Set structure. */
struct rhim;  /* Map structure. */

/**
 * Mutable pair.
 */
struct rhipair {
  void* key;
  void* val;
};

/**
 * Immutable pair.
 */
struct rhiconstpair {
  const void* key;
  void* val;
};

/* ===== Set ===== */

/****************************
 * Initialization functions *
 ****************************/

/**
 * \brief   Create a table.
 * 
 * The table will be initialized at default size.
 * 
 * \param   hash     Hash function
 * \param   equal    Equal function
 * \param   free     Key destroyer function. If not needed,
 *                   set the argument as NULL.
 * \param   mode     Mode
 * 
 * \return  On success, pointer of dictionary is returned. On
 *          failure, NULL is returned.
 */
RHI_API struct rhis* rhis_init(
  rhihash hash,
  rhiequal equal,
  rhikeyfree free,
  int mode
);

/**
 * \brief   Reserve a table.
 * 
 * The table will be initialized to the specified size, the
 * size of which is set >= specified size. The maximum table
 * size (RHI_PRIME enabled) is 1,546,188,225 + 1 (null key),
 * and the default is 1,546,188,226 + 1.
 * 
 * \param   hash     Hash function
 * \param   equal    Equal function
 * \param   free     Key destroyer function. If not needed,
 *                   set the argument as NULL.
 * \param   size     Reserved size
 * \param   mode     Mode
 * 
 * \return  On success, pointer of dictionary is returned. On
 *          failure, NULL is returned.
 */
RHI_API struct rhis* rhis_reserve(
  rhihash hash,
  rhiequal equal,
  rhikeyfree free,
  rhiuint size,
  int mode
);

/***********************
 * Insertion functions *
 ***********************/

/**
 * \brief   Insert the key in the table.
 * 
 * Insertion failed due to:
 *  - The key has been inserted
 *  - When the mode is not set with RHI_EXTEND and the maximum
 *    limit of elements is reached.
 * 
 * \param   set  Table
 * \param   key  Key
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
RHI_API bool rhis_insert(struct rhis* set, void* key);

/********************
 * Search functions *
 ********************/

/**
 * \brief   Search the key in the table.
 * 
 * Search failed because the given key is not in the table.
 * 
 * \param   set  Table
 * \param   key  Key
 * 
 * \return  On success, true is returned. On failure, false is
 *          returned.
 */
RHI_API bool rhis_search(const struct rhis* set, const void* key);

/**
 * \brief   Search the key in the table.
 * 
 * Search failed because the given key is not in the table.
 * 
 * \param   set  Table
 * \param   key  Key
 * 
 * \return  On success, the searched key is returned. On
 *          failure, NULL is returned.
 */
RHI_API const void* rhis_ksearch(const struct rhis* set, const void* key);

// /*************************
//  * Replacement functions *
//  *************************/

// /**
//  * \brief   Replace the key into the dictionary
//  * 
//  * If keyfree is not set as NULL, the old key is replaced by
//  * the given key. The old key was destroyed by keyfree.
//  * 
//  * Replacement failed due to:
//  *  - Unique key insertion when the mode is not set with
//  *    RHI_EXTEND and the maximum limit of elements is reached.
//  *  - On rare condition, memory allocation may fail when the
//  *    dictionary is extended.
//  * 
//  * \param   set  Dictionary
//  * \param   key  Key
//  * 
//  * \return  On success, true is returned. On failure, false is
//  *          returned.
//  */
// RHI_API bool rhis_replace(struct rhis* set, void* key);

// /**
//  * \brief   Replace the key into the dictionary
//  * 
//  * The old key is replaced by the given key.
//  * 
//  * Replacement failed due to:
//  *  - Unique key insertion when the mode is not set with
//  *    RHI_EXTEND and the maximum limit of elements is reached.
//  *  - On rare condition, memory allocation may fail when the
//  *    dictionary is extended.
//  * 
//  * \param   set  Dictionary
//  * \param   key  Key
//  * 
//  * \return  On success, the old key is returned. On failure,
//  *          NULL is returned.
//  */
// RHI_API void* rhis_kreplace(struct rhis* set, void* key);

// /**********************
//  * Deletion functions *
//  **********************/

// /**
//  * \brief   Delete the key from the dictionary
//  * 
//  * If keyfree is not set as NULL, the given key would be
//  * destroyed by keyfree. Deletion failed because the given key
//  * is not in the dictionary.
//  * 
//  * \param   set  Dictionary
//  * \param   key  Key
//  * 
//  * \return  On success, true is returned. On failure, false is
//  *          returned.
//  */
// RHI_API bool rhis_delete(struct rhis* set, void* key);

// /**
//  * \brief   Delete the key from the dictionary
//  * 
//  * Delete failed because the given key is not in the
//  * dictionary.
//  * 
//  * \param   set  Dictionary
//  * \param   key  Key
//  * 
//  * \return  On success, the old key is returned. On failure,
//  *          NULL is returned.
//  */
// RHI_API void* rhis_kdelete(struct rhis* set, void* key);

// /***************************
//  * Miscellaneous functions *
//  ***************************/

// RHI_API rhiuint rhis_count(const struct rhis* set);
// RHI_API void rhis_free(struct rhis* set);

// /******************************
//  * Forward traverse functions *
//  ******************************/

// RHI_API void rhis_begin(struct rhis* set);
// RHI_API void rhis_next(struct rhis* set);
// RHI_API bool rhis_end(const struct rhis* set);
// RHI_API const void* rhis_current(const struct rhis* set);

// /*****************************
//  * Random traverse functions *
//  *****************************/

// RHI_API rhiuint* rhis_iters(const struct rhis* set);
// RHI_API const void* rhis_get(const struct rhis* set, rhiuint iter);

// /* ===== Map ===== */

// /************************
//  * Initialize functions *
//  ************************/

// /**
//  * \brief   Create dictionary
//  * 
//  * Dictionary is initialized to the default size.
//  * 
//  * \param   hash     Hash function
//  * \param   equal    Equal function
//  * \param   keyfree  Key destroyer function. If not needed,
//  *                   set the argument as NULL.
//  * \param   valfree  Value destroyer function. If not needed,
//  *                   set the argument as NULL.
//  * \param   mode     Mode
//  * 
//  * \return  On success, pointer of dictionary is returned. On
//  *          failure, NULL is returned.
//  */
// RHI_API struct rhim* rhim_init(rhihash hash,
//   rhiequal equal, rhikeyfree keyfree, rhivalfree valfree, int mode);

// /**
//  * \brief   Reserve dictionary according to size
//  * 
//  * Dictionary is initialized to the specified size, the size
//  * of which is set >= specified size. The maximum dictionary
//  * size (RHI_PRIME_METHOD enabled) is 1546188225, and the
//  * default is 1546188226.
//  * 
//  * \param   hash     Hash function
//  * \param   equal    Equal function
//  * \param   keyfree  Key destroyer function. If not needed,
//  *                   set the argument as NULL
//  * \param   valfree  Value destroyer function. If not needed,
//  *                   set the argument as NULL.
//  * \param   size     Reserved size
//  * \param   mode     Mode
//  * 
//  * \return  On success, pointer of dictionary is returned. On
//  *          failure, NULL is returned.
//  */
// RHI_API struct rhim* rhim_reserve(rhihash hash, rhiequal equal,
//   rhikeyfree keyfree, rhivalfree valfree, rhiuint size, int mode);

// /***********************
//  * Insertion functions *
//  ***********************/

// /**
//  * \brief   Insert the key and value into the dictionary
//  * 
//  * Insertion failed due to:
//  *  - The key has been inserted.
//  *  - When the mode is not set with RHI_EXTEND and the maximum
//  *    limit of elements is reached.
//  *  - On rare condition, memory allocation may fail when the
//  *    dictionary is extended.
//  * 
//  * \param   map  Dictionary
//  * \param   key  Key
//  * \param   val  Value
//  * 
//  * \return  On success, true is returned. On failure, false is
//  *          returned.
//  */
// RHI_API bool rhim_insert(struct rhim* map, void* key, void* val);

// /********************
//  * Search functions *
//  ********************/

// /**
//  * \brief   Search the key in the dictionary
//  * 
//  * Search failed because the given key is not in the
//  * dictionary.
//  * 
//  * \param   map  Dictionary
//  * \param   key  Key
//  * 
//  * \return  On success, true is returned. On failure, false is
//  *          returned.
//  */
// RHI_API bool rhim_search(const struct rhim* map, const void* key);

// /**
//  * \brief   Search the key in the dictionary
//  * 
//  * Search failed because the given key is not in the
//  * dictionary.
//  * 
//  * \param   map  Dictionary
//  * \param   key  Key
//  * 
//  * \return  On success, the value is returned. On failure,
//  *          NULL is returned.
//  */
// RHI_API void* rhim_vsearch(const struct rhim* map, const void* key);
// RHI_API struct rhiconstpair rhim_kvsearch(const struct rhim* map, const void* key);

// /*************************
//  * Replacement functions *
//  *************************/

// RHI_API bool rhim_replace(struct rhim* map, void* key, void* val);
// RHI_API void* rhim_vreplace(struct rhim* map, void* key, void* val);
// RHI_API struct rhipair rhim_kvreplace(struct rhim* map, void* key, void* val);


// /**************************
//  * Delete functions (map) *
//  **************************/

// RHI_API bool rhim_delete(struct rhim* map, void* key);
// RHI_API void* rhim_vdelete(struct rhim* map, void* key);
// RHI_API struct rhipair rhim_kvdelete(struct rhim* map, void* key);


// /*********************************
//  * Miscellaneous functions (map) *
//  *********************************/

// RHI_API rhiuint rhim_count(const struct rhim* map);
// RHI_API void rhim_free(struct rhim* map);


// /************************************
//  * Forward traverse functions (map) *
//  ************************************/

// RHI_API void rhim_begin(struct rhim* map);
// RHI_API void rhim_next(struct rhim* map);
// RHI_API bool rhim_end(const struct rhim* map);
// RHI_API struct rhiconstpair rhim_current(const struct rhim* map);
// RHI_API const void* rhim_kcurrent(const struct rhim* map);
// RHI_API void* rhim_vcurrent(const struct rhim* map);


// /***********************************
//  * Random traverse functions (map) *
//  ***********************************/

// RHI_API rhiuint* rhim_iters(const struct rhim* map);
// RHI_API struct rhiconstpair rhim_get(const struct rhim* map, rhiuint iter);
// RHI_API const void* rhim_kget(const struct rhim* map, rhiuint iter);
// RHI_API void* rhim_vget(const struct rhim* map, rhiuint iter);

#ifdef __cplusplus
}
#endif

#endif /* RHI_H */
