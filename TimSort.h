#ifndef TIMSORT_H_INCLUDED
#define TIMSORT_H_INCLIDED

void timsort(void* const mem, const int len, const int size_elem, int (*cmp)(void*, void*));

#endif //TIMSORT_H_INCLUDED