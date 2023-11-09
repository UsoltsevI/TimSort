#ifndef TIMSORT_H_INCLUDED
#define TIMSORT_H_INCLIDED

typedef int (*xcmp_t)(char *lhs, char *rhs, int X);

void timsort(char * *mem, int N, int K, int X);

void timsort_imp(char * *mem, int N, int K, int X, xcmp_t cmp, int l, int r);

void timsort_merge(char * *mem, int N, int K, int X, xcmp_t cmp, int l, int m, int r);

int compare_by_x_field(char *lhs, char *rhs, int X);

#endif //TIMSORT_H_INCLUDED