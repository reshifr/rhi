#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rhi.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/**
 * Utils macros
 */
#define SEED 28731
#define LOWER 0x01
#define UPPER 0x02
#define BUFSIZE 0xFFFF

/**
 * Consistency tests macros
 */
#define CONS_MIN_OBJLEN 1
#define CONS_MAX_OBJLEN 1
#define CONS_OBJCASE UPPER
#define CONS_RESERVE_SIZE 10
#define CONS_NUM_KEYS 14

/**
 * Performance tests macros
 */
#define PERF_MIN_OBJLEN 16
#define PERF_MAX_OBJLEN 32
#define PERF_OBJCASE (LOWER|UPPER)
#define PERF_RESERVE_SIZE 3000000
#define PERF_NUM_KEYS 10000000

#define TEST(_callback, _exp)  \
  if( _exp ) \
    _callback

#define ABORT(_msg) \
  do { \
    fprintf(stderr, "%s\n", _msg); \
    exit(EXIT_FAILURE); \
  } while(0)

#define HANDLE(_exp, _msg) \
  do { \
    if( _exp ) { \
      fprintf(stderr, "%s\n", _msg); \
      exit(EXIT_FAILURE); \
    } \
  } while(0)

#define ASSERT(_exp, _fmt, ...) \
  do { \
    if( _exp ) { \
      fprintf(stderr, _fmt, __VA_ARGS__); \
      fprintf(stderr, "\n"); \
    } \
  } while(0)

/**********************
 * Function container *
 **********************/

struct funcs {
  rhihash hash;
  rhiequal equal;
  rhikeyfree keyfree;
  rhivalfree valfree;
};

/*******************
 * List of objects *
 *******************/

struct objs {
  rhiuint count;
  void* objs[];
};

/**
 * Generate objects with minimum length (min), maximum length
 * (max), count (count), with case (mode).
 */
struct objs* objs_init(int min, int max, rhiuint count, int mode);

/**
 * Make objs duplication.
 */
struct objs* objs_dup(const struct objs* objs);

/**
 * Destroy objs.
 */
void objs_free(struct objs* objs);

/**
 * Destroy objs[] and objs.
 */
void objs_destroy(struct objs* objs);

/*********
 * Clock *
 *********/

/**
 * Get the current time in milliseconds.
 */
long get_clock(void);

/*******
 * Set *
 *******/

/**
 * Print dictionary.
 */
void set_print(struct rhis* set);

/**
 * Print only dictionary metadata.
 */
void set_mprint(struct rhis* set);

/*******
 * Map *
 *******/

/**
 * Print dictionary.
 */
void map_print(struct rhim* map);

/**
 * Print only dictionary metadata.
 */
void map_mprint(struct rhim* map);

/*****************
 * Miscellaneous *
 *****************/

/**
 * String hash function.
 */
size_t murmur_hash(const void* obj);

/**
 * String equal function.
 */
bool equal(const void* first_obj, const void* second_obj);

/*************
 * Set tests *
 *************/

void set_init(void);
void set_reserve(void);
void set_insert(void);
void set_replace(void);
void set_kreplace(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_H */
