#ifndef TEST_H
#define TEST_H

#include <rhi.h>
#include <cstdio>
#include <chrono>
#include <random>
#include <thread>
#include <cstdlib>
#include <cstring>
#include <functional>

#if defined __GNUG__
# include <parallel/algorithm>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define IS_EMPTY(_node) ((_node).key==NULL)
#define NOT_EMPTY(_node) ((_node).key!=NULL)
#define HASHVAL_TO_INDEX(_hashval, _size) ((rhiuint)((_hashval)%(_size)))

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
  struct rhisnode* nodes;  /* An array of nodes. */
};

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
  void* null_val;  /* The value pair of NULL key. */
  struct rhimnode* nodes;  /* An array of nodes. */
};

#ifdef __cplusplus
}
#endif

#define TEST(_exp, _callback)  \
  do { \
    if( _exp ) \
      _callback; \
  } while(0)

#define ABORT(_msg) \
  do { \
    std::fprintf(stderr, "%s\n", _msg); \
    std::exit(EXIT_FAILURE); \
  } while(0)

#define HANDLE(_exp, _msg) \
  do { \
    if( _exp ) { \
      std::fprintf(stderr, "%s\n", _msg); \
      std::exit(EXIT_FAILURE); \
    } \
  } while(0)

#define ASSERT(_exp, _fmt, ...) \
  do { \
    if( _exp ) { \
      std::fprintf(stderr, _fmt, __VA_ARGS__); \
      std::fprintf(stderr, "\n"); \
    } \
  } while(0)

namespace utils {
  constexpr int SEED(7066463);
  constexpr int LOWER(0x1);
  constexpr int UPPER(0x2);
  constexpr int RANDOM(0x3);
  constexpr int UNIT_CASE(UPPER);
  constexpr int PERF_CASE(RANDOM);
  constexpr int UNIT_MIN(1);
  constexpr int UNIT_MAX(1);
  constexpr rhiuint UNIT_RESERVE(10);
  constexpr rhiuint UNIT_COUNT(16);
  constexpr int PERF_MIN(4);
  constexpr int PERF_MAX(6);
  constexpr rhiuint PERF_RESERVE(24000000);
  constexpr rhiuint PERF_COUNT(24000000);
  constexpr const char* CHARS(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  );

  inline std::size_t hash(const void* obj) {
    auto len = std::strlen((const char*)obj);
  #if SIZE_MAX == UINT64_MAX
    const auto r = 47;
    const auto m = UINT64_C(0xc6a4a7935bd1e995);
    std::uint64_t h = utils::SEED^(len*m);
    const auto* data0 = (const std::uint64_t*)obj;
    const auto* end = data0+(len>>3);
    while( data0!=end ) {
      auto k = *data0++;
      k *= m;
      k ^= k>>r;
      k *= m;
      h ^= k;
      h *= m;
    }
    const auto* data1 = (const std::uint8_t*)data0;
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
    b7: h ^= (std::uint64_t)data1[6]<<48;
    b6: h ^= (std::uint64_t)data1[5]<<40;
    b5: h ^= (std::uint64_t)data1[4]<<32;
    b4: h ^= (std::uint64_t)data1[3]<<24;
    b3: h ^= (std::uint64_t)data1[2]<<16;
    b2: h ^= (std::uint64_t)data1[1]<<8;
    b1: h ^= (std::uint64_t)data1[0];
    b0:
      h *= m;
      h ^= h>>r;
      h *= m;
      h ^= h>>r;
  #else
    const auto r = 24;
    const auto m = UINT32_C(0x5bd1e995);
    std::uint32_t h = SEED^len;
    const auto* data = (const std::uint8_t*)obj;
    while( len>=4 ) {
      auto k = *(std::uint32_t*)data;
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
    b3: h ^= (std::uint32_t)data[2]<<16;
    b2: h ^= (std::uint32_t)data[1]<<8;
    b1: h ^= (std::uint32_t)data[0];
    b0:
      h *= m;
      h ^= h>>13;
      h *= m;
      h ^= h>>15;
  #endif
    return h;
  }

  inline bool equal(const void* first_obj, const void* second_obj) {
    return std::strcmp((const char*)first_obj, (const char*)second_obj)==0;
  }

  static inline const char* mode(int mode) {
    if( mode==RHI_FIXED )
      return "FIXED";
    if( mode==RHI_SHRINK )
      return "SHRINK";
    if( mode==RHI_EXTEND )
      return "EXTEND";
    return "ALL";
  }

  template <class Table>
  void mprint(Table table) {
    std::printf(
      "\n"
      "curr_range=%" PRIu8 " begin_range=%" PRIu8 " mode=%s\n"
      "size=%" PRIrhiu " min=%" PRIrhiu " "
      "max=%" PRIrhiu " occupied=%" PRIrhiu "\n\n",
      table->curr_range, table->begin_range, mode(table->mode),
      table->size, table->min, table->max, table->occupied
    );
  }

  template <class Table>
  void print(Table table) {
    utils::mprint(table);
    auto null = !table->has_null_inserted;
    if constexpr( std::is_same_v<Table, struct rhis*> ) {
      std::printf(
        "|%s|    #|0x%.16" PRIx64 "|%5s|    #|\n\n",
        null ? "-" : "O", UINT64_C(0), ""
      );
      for(rhiuint i=0; i<table->size; ++i) {
        auto empty = IS_EMPTY(table->nodes[i]);
        std::printf(
          "|%s|%5" PRIrhiu "|0x%.16" PRIx64 "|%5s|%5" PRIrhiu "|\n",
          empty ? "-" : "O", i,
          empty ? UINT64_C(0) : (std::uint64_t)table->nodes[i].hashval,
          empty ? "" : (const char*)table->nodes[i].key,
          empty ? RHIUINT_C(0) :
            HASHVAL_TO_INDEX(table->nodes[i].hashval, table->size)
        );
      }
    } else {
      std::printf(
        "|%s|    #|0x%.16" PRIx64 "|%5s|%5s|    #|\n\n",
        null ? "-" : "O", UINT64_C(0), "", null ? "" :
          table->null_val==NULL ? "" : (const char*)table->null_val
      );
      for(rhiuint i=0; i<table->size; ++i) {
        auto empty = IS_EMPTY(table->nodes[i]);
        std::printf(
          "|%s|%5" PRIrhiu "|0x%.16" PRIx64 "|%5s|%5s|%5" PRIrhiu "|\n\n",
          empty ? "-" : "O", i,
          empty ? UINT64_C(0) : (std::uint64_t)table->nodes[i].hashval,
          empty ? "" : (const char*)table->nodes[i].key,
          empty ? "" :
            table->nodes[i].val==NULL ? "" : (const char*)table->nodes[i].val,
          empty ? RHIUINT_C(0) : (std::uint64_t)
            HASHVAL_TO_INDEX(table->nodes[i].hashval, table->size)
        );
      }
    }
    printf("\n");
  }

  class timer {
    public:
      using nano = std::chrono::nanoseconds;
      using micro = std::chrono::microseconds;
      using mili = std::chrono::milliseconds;

    private:
      decltype(std::chrono::high_resolution_clock::now()) x, y;

    public:
      void start(void) {
        timer::x = std::chrono::high_resolution_clock::now();
      }

      template <typename Precision>
      std::int64_t result(void) {
        timer::y = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<Precision>(timer::y-timer::x).count();
      }
  };

  template <int Case=utils::RANDOM, bool Free=true>
  class objs {
    public:
      rhiuint uniqueness;

    private:
      std::vector<void*> list;

    public:
      objs(int min, int max, rhiuint count) {
        HANDLE(min==0, "Error: Minimum cannot be zero.");
        HANDLE(max==0, "Error: Maximum cannot be zero.");
        HANDLE(min>max, "Error: Minimum is greater than maximum.");
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937_64 rng(seed);
        std::uniform_int_distribution<int> rand_ch;
        std::uniform_int_distribution<int> rand_len(min, max);
        if constexpr( Case==utils::LOWER )
          rand_ch = std::uniform_int_distribution<int>(0, 25);
        if constexpr( Case==utils::UPPER )
          rand_ch = std::uniform_int_distribution<int>(26, 51);
        if constexpr( Case==utils::RANDOM )
          rand_ch = std::uniform_int_distribution<int>(0, 51);
        for(rhiuint i=0; i<count; ++i) {
          auto len = rand_len(rng);
          char* buf = new char[len+1]();
          for(int j=0; j<len; ++j)
            buf[j] = utils::CHARS[rand_ch(rng)];
          objs::list.push_back(buf);
        }
        auto sorted(objs::list);
#if defined __GNUG__
        __gnu_parallel::stable_sort(sorted.begin(), sorted.end(),
          [](const void* first_obj, const void* second_obj) {
            return std::strcmp((const char*)first_obj,
              (const char*)second_obj)<0;
          });
#else
        std::stable_sort(sorted.begin(), sorted.end(),
          [](const void* first_obj, const void* second_obj) {
            return std::strcmp((const char*)first_obj,
              (const char*)second_obj)<0;
          });
#endif
        objs::uniqueness = (rhiuint)std::distance(
          sorted.begin(),
          std::unique(sorted.begin(), sorted.end(),
            [](const void* first_obj, const void* second_obj) {
              return utils::equal(first_obj, second_obj);
            })
        );
      }

      void reverse(void) {
        std::reverse(objs::list.begin(), objs::list.end());
      }

      void load(std::function<void (void* obj)> callback) const {
        for(auto obj : objs::list)
          callback(obj);
      }

      ~objs(void) {
        if constexpr( Free ) {
#if defined __GNUG__
          __gnu_parallel::for_each(objs::list.begin(), objs::list.end(),
            [](void* obj) {
              delete[] (char*)obj;
            });
#else
          for(auto obj : objs::list)
            delete[] (char*)obj;
#endif
        }
      }
  };
}

namespace set {
  void init(void);
  void reserve(void);
  void insert(void);
}

namespace map {
  void init(void);
  void reserve(void);
  void insert(void);
}

#endif /* TEST_H */
