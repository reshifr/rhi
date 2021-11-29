# rhi [![build](https://github.com/reshifr/rhi/actions/workflows/build.yml/badge.svg?branch=master)](https://github.com/reshifr/rhi/actions/workflows/build.yml) [![test](https://github.com/reshifr/rhi/actions/workflows/test.yml/badge.svg?branch=master)](https://github.com/reshifr/rhi/actions/workflows/test.yml)

## Implementation of linear probing hash table using LCFS hashing

This library is a C implementation of set and map data structure with raw pointer-type key and value.

<img src="https://github.com/reshifr/rhi/blob/master/docs/algorithm-illustration.jpg?raw=true" alt="Algorithm Illustration" width="640"/>

### Features

(...)

### Usage

(...)

```c
struct rhis* set = rhis_set(hash, equal, RHI_SHRINK|RHI_EXTED);
if( rhis_search(set, "Hello") )
  printf("Hello World");
```
