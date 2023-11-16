#include <stdio.h>
#include "TimSort.h"

int cmp_int(void* a, void* b);

int main() {
    int len = 200;
    int data[200] = {   1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        1, 2, 5, 6, 1, 1, 1, 7, 8, 90,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3,
                        11, 8, 9, 900, 0, 3, 4, 6, -10, -3};

    timsort(data, len, sizeof(int), &cmp_int);
    
    printf("output: ");

    for (int i = 0; i < len; i++) {
        if (i % 10 == 0)
            printf("\n");

        printf("%d ", data[i]);
    }
    
    printf("\n");

    return 0;
}

int cmp_int(void* a, void* b) {
    int* ai = (int*) a;
    int* bi = (int*) b;

    if (*ai < *bi)
        return -1;
    
    return 0;
}

#if 0
int compare_by_x_field(char *lhs, char *rhs, int X) { //oh, not here, please
    return *(lhs + X) - *(rhs + X);
}
#endif 