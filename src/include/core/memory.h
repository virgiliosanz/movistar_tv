#ifndef __memutl_h__
#define __memutl_h__

void *memdup(const void *src, size_t sz);
#define ALLOC_INIT(type, ...)   \
        (type *)memdup((type[]){ __VA_ARGS__ }, sizeof(type))
/*
 * Sample usage:
 *
 * struct foobar *foobar = ALLOC_INIT(struct foobar, {
 *      .field = value,
 *      .other_field = other_value,
 *      .yet_another_field = yet_another_value
 * });
 */


#endif
