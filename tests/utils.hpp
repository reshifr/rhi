#ifndef TEST_H
#define TEST_H

#include <rhi.h>
#include <cstdio>
#include <chrono>
#include <random>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <functional>

#define STD_EXECUTION_PAR_UNSEQ_IMPLEMENTED \
  ((defined(__GNUC__) && defined(__GNUG__)) && \
    (__GNUC__ >= 10 && __GNUG__ >= 10)) || \
  (defined(_MSC_VER) || _MSC_VER >= 1928)

#if STD_EXECUTION_PAR_UNSEQ_IMPLEMENTED
#include <execution>
#endif

extern "C" {

#define IS_EMPTY(_node) ((_node).key==NULL)
#define NOT_EMPTY(_node) ((_node).key!=NULL)
#define HASHVAL_TO_INDEX(_hashval, _size) ((rhiuint)((_hashval)%(_size)))

/***************
 * Node of set *
 ***************/

struct rhisnode {
  size_t hashval;  /* The hash value of the key. */
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
  size_t hashval;  /* The hash value of the key. */
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
  void* null_val;  /* The value pair of NULL key. */
  struct rhimnode* nodes;  /* An array of nodes. */
};

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

}

namespace utils {

constexpr auto SEED(7066463);
constexpr auto LOWER(0x1);
constexpr auto UPPER(0x2);
constexpr auto RANDOM(0x3);
constexpr auto UNIT_RESERVE(RHIUINT_C(10));
constexpr auto UNIT_COUNT(RHIUINT_C(16));
constexpr auto UNIT_SHRINK(RHIUINT_C(5));
constexpr auto UNIT_EXTEND(RHIUINT_C(5));
constexpr auto AUTO_RESERVE(RHIUINT_C(600'000));
constexpr auto AUTO_COUNT(RHIUINT_C(600'000));
constexpr auto AUTO_SHRINK(RHIUINT_C(300'000));
constexpr auto AUTO_EXTEND(RHIUINT_C(300'000));
constexpr auto CHARS("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

inline std::size_t hash(const void* obj) {
  auto len = std::strlen((const char*)obj);
#if SIZE_MAX == UINT64_MAX
  const auto r = 47;
  const auto m = UINT64_C(0xc6a4a7935bd1e995);
  std::uint64_t h = SEED^(len*m);
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

inline bool equal(const void* first_obj, const void* second_obj)
  { return std::strcmp((const char*)first_obj, (const char*)second_obj)==0; }
inline bool compare(const void* first_obj, const void* second_obj)
  { return std::strcmp((const char*)first_obj, (const char*)second_obj)<0; }

inline const char* mode(int mode) {
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
  mprint(table);
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
        empty ? RHIUINT_C(0) :
          HASHVAL_TO_INDEX(table->nodes[i].hashval, table->size)
      );
    }
  }
  std::printf("\n");
}

class timer {
  public:
    using nano = std::chrono::nanoseconds;
    using micro = std::chrono::microseconds;
    using mili = std::chrono::milliseconds;

  private:
    decltype(std::chrono::high_resolution_clock::now()) m_x, m_y;

  public:
    void start(void)
      { m_x = std::chrono::high_resolution_clock::now(); }

    template <typename Precision>
    std::int64_t result(void) {
      m_y = std::chrono::high_resolution_clock::now();
      return std::chrono::duration_cast<Precision>(m_y-m_x).count();
    }
};

template <int Case=RANDOM>
class objs {
  private:
    int m_min;
    int m_max;
    rhiuint m_uniqueness;
    std::vector<void*> m_list;
    
    std::vector<void*> gen(int min, int max, rhiuint n) {
      HANDLE((m_min=min)==0, "Error: Minimum cannot be zero.");
      HANDLE((m_max=max)==0, "Error: Maximum cannot be zero.");
      HANDLE(min>max, "Error: Minimum is greater than maximum.");
      std::vector<void*> rand_list;
      std::mt19937_64 rng(std::chrono::
        system_clock::now().time_since_epoch().count());
      std::uniform_int_distribution<int> rand_ch;
      std::uniform_int_distribution<int> rand_len(min, max);
      if constexpr( Case==utils::LOWER )
        rand_ch = std::uniform_int_distribution<int>(0, 25);
      if constexpr( Case==utils::UPPER )
        rand_ch = std::uniform_int_distribution<int>(26, 51);
      if constexpr( Case==utils::RANDOM )
        rand_ch = std::uniform_int_distribution<int>(0, 51);
      for(rhiuint i=0; i<n; ++i) {
        auto len = rand_len(rng);
        char* buf = new char[len+1]();
        for(int j=0; j<len; ++j)
          buf[j] = utils::CHARS[rand_ch(rng)];
        rand_list.push_back(buf);
      }
      return rand_list;
    }
    
    rhiuint uniqueness(std::vector<void*>& base_list) {
      std::vector<void*> sorted_list(base_list);
#if STD_EXECUTION_PAR_UNSEQ_IMPLEMENTED
      std::stable_sort(std::execution::par_unseq,
        sorted_list.begin(), sorted_list.end(), utils::compare);
      return (rhiuint)std::distance(sorted_list.begin(),
        std::unique(std::execution::par_unseq,
        sorted_list.begin(), sorted_list.end(), utils::equal));
#else
      std::stable_sort(sorted_list.begin(), sorted_list.end(), utils::compare);
      return (rhiuint)std::distance(sorted_list.begin(),
        std::unique(sorted_list.begin(), sorted_list.end(), utils::equal));
#endif
    }

  public:
    objs(int min, int max, rhiuint n) :
      m_min(min), m_max(max), m_list(gen(min, max, n))
      { m_uniqueness = uniqueness(m_list); }

    rhiuint size(void) const { return (rhiuint)m_list.size(); }
    rhiuint uniqueness(void) const { return m_uniqueness; }
    void shuffle(void) { std::reverse(m_list.begin(), m_list.end()); }

    void load(std::function<void (void* obj)> callback) const {
      for(auto obj : m_list)
        callback(obj);
    }

    void add(rhiuint n) {
      std::vector<void*> add_list = gen(m_min, m_max, n);
      m_list.insert(m_list.end(), add_list.begin(), add_list.end());
      m_uniqueness = uniqueness(m_list);
    }

    ~objs(void) {
#if STD_EXECUTION_PAR_UNSEQ_IMPLEMENTED
      std::for_each(std::execution::par_unseq, m_list.begin(),
        m_list.end(), [](void* obj) { delete[] (char*)obj; });
#else
      std::for_each(m_list.begin(),
        m_list.end(), [](void* obj) { delete[] (char*)obj; });
#endif
    }
};

}

namespace set {

void unit_init(void);
void unit_reserve(void);
void unit_insert(void);
void unit_search(void);
void auto_insert(void);
void perfom_insert(void);

}

#endif /* TEST_H */
