#ifndef TEST_H
#define TEST_H

#include <rhi.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define SEED 28731
#define BUFSIZE 0xFFFF
#define LOWER 0x01
#define UPPER 0x02

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

struct objs {
  rhiuint count;
  void* objs[];
};

/* Objs */
struct objs* objs_init(int min, int max, rhiuint count, int mode);
void objs_free(struct objs* objs);
void objs_destroy(struct objs* objs);

/* Set */
void set_print(struct rhis* set);
void set_mprint(struct rhis* set);

/* Map */
void map_print(struct rhim* map);
void map_mprint(struct rhim* map);

/* Misc */
size_t murmur_hash(const void* key);
bool equal(const void* key1, const void* key2);

#include "set.h"

#endif /* TEST_H */

