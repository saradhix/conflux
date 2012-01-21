#include "memory.h"

void *flux_malloc(int size)
{
return malloc(size);
}

void flux_free(void *p)
{
free(p);
}
