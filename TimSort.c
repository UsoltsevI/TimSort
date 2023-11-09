#include "TimSort.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>

void timsort(char * *mem, int N, int K, int X) {
    timsort_imp(mem, N, K, X, &compare_by_x_field, 0, N - 1);
}

void timsort_imp(char * *mem, int N, int K, int X, xcmp_t cmp, int l, int r) {
    int m = 0;

    if (l >= r) 
        return;
    
    m = (l + r) / 2;

    timsort_imp(mem, N, K, X, cmp, l, m);
    timsort_imp(mem, N, K, X, cmp, m + 1, r);
    timsort_merge(mem, N, K, X, cmp, l, m, r);
}

void timsort_merge(char * *mem, int N, int K, int X, xcmp_t cmp, int l, int m, int r) {
    char * *mem_first = (char * *) calloc(m - l + 1, sizeof(char *));
    char * *mem_second = (char * *) calloc(r - m, sizeof(char *));
    int p1 = 0, p2 = 0;

    for (int i = 0; i < m - l + 1; i++)
        *(mem_first + i) = *(mem + l + i);

    for (int i = 0; i < r - m; i++)
        *(mem_second + i) = *(mem + m + 1 + i);

    for (int i = l; i < r + 1; i++) {
        if ((p2 >= r - m) || ((p1 < m - l + 1) && (cmp(*(mem_first + p1), *(mem_second + p2), X) <= 0))) {
            *(mem + i) = *(mem_first + p1);
            p1++;

        } else {
            *(mem + i) = *(mem_second + p2);
            p2++;
        }
    } 

    free(mem_first);
    free(mem_second); 
}

int compare_by_x_field(char *lhs, char *rhs, int X) {
    return *(lhs + X) - *(rhs + X);
}
