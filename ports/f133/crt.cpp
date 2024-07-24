#include <stddef.h>
extern "C"
{
void *m_malloc(size_t);
void *m_free(void *);
}


static void *_malloc(size_t size)
{
 return m_malloc(size);
}

static void _free(void *p)
{
  m_free(p);
}

void *operator new[](size_t size)
{
  return _malloc(size);
}

void operator delete[](void*p)
{
  _free(p);
}

void *operator new(size_t size)
{
  return _malloc(size);
}

void operator delete(void*p)
{
  _free(p);
}

