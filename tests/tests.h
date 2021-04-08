#include "utils.h"

#define CONS_MIN_OBJLEN 3
#define CONS_MAX_OBJLEN 5
#define CONS_OBJCASE UPPER
#define CONS_RESERVE_SIZE 10
#define CONS_NUM_KEYS 16

#define PERF_MIN_OBJLEN 16
#define PERF_MAX_OBJLEN 32
#define PERF_OBJCASE (LOWER|UPPER)
#define PERF_RESERVE_SIZE 3000000
#define PERF_NUM_KEYS 10000000

void set_init(struct rhifunc* func);
void set_reserve(struct rhifunc* func);
void set_insert(struct rhifunc* func);
