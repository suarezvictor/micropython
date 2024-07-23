#warning implement allocators!

extern "C"
{
#include "py/gc.h"
}

void *operator new[](unsigned long size)
{
  return gc_alloc(size, false);
}

void operator delete[](void*p)
{
}

void *operator new(unsigned long size)
{
  return gc_alloc(size, false);
}

void operator delete(void*p)
{
}

