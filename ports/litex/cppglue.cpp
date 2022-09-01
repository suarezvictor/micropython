extern "C" {
#warning malloc does not yet work in micropython, using gc heap
#include "py/gc.h"
}

void* operator new(size_t size) {
   void *p = gc_alloc(size, false);
   return p;
}

void operator delete(void *p) {
   return gc_free(p);
}
