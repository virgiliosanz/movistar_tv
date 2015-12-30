#include "core/all.h"

void *
memdup(const void *src, size_t sz)
{
	void *mem = malloc(sz);
	return mem ? memcpy(mem, src, sz) : NULL;
}
