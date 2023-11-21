#include <stdio.h>
#include <malloc.h>
#include "../TimSort.h"

int MOD = 0;

int compare_by_mod(void* a, void* b) {
    int* ai = (int*) a;
    int* bi = (int*) b;

    return ((*ai) % MOD) - ((*bi) % MOD);
}

int main() {
    int N = 0, check = 0;

    check += scanf("%d", &N);

    int *data = (int*) calloc(N, sizeof(int));

    for (int i = 0; i < N; i++)
        check += scanf("%d", &data[i]);
    
    check += scanf("%d", &MOD);

    if (check != N + 2)
        printf("failure while reading data\n");

    timsort(data, N, sizeof(int), &compare_by_mod);

    for (int i = 0; i < N; i++)
        printf("%d ", data[i]);
    
    printf("\n");

    free(data);

    return 0;
}