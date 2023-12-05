#ifndef TIMSORT_H_INCLUDED
#define TIMSORT_H_INCLIDED

#include <stdio.h>

void timsort(void * const mem, const size_t len, const size_t size_elem, int (*cmp)(const void *, const void *));

void inssort(void *arr, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *));

#endif //TIMSORT_H_INCLUDED