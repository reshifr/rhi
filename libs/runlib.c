#include <rhi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

size_t hash(const void* obj) {
  int ch;
  size_t h = 5381;
  const unsigned char* m = obj;
  while( (ch=*m++)!=0 )
    h = ((h<<5)+h)+ch;
  return h;
}

bool equal(const void* a, const void* b)
  { return strcmp(a, b)==0; }

int main(void) {
  struct rhim* map = rhim_init(&hash, &equal, RHI_SHRINK|RHI_EXTEND);
  if( map==NULL ) {
    fprintf(stderr, "Error: map initialization failed.\n");
    return EXIT_FAILURE;
  }
  rhim_insert(map, (void*)"I", (void*)"Success: It");
  rhim_insert(map, (void*)"am", (void*)"really");
  rhim_insert(map, (void*)"trying", (void*)"works.");
  printf("%s %s %s\n",
    (const char*)rhim_search(map, "I"),
    (const char*)rhim_search(map, "am"),
    (const char*)rhim_search(map, "trying")
  );
  return EXIT_SUCCESS;
}
