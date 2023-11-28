#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "../timsort.h"

void timsort_int();
void timsort_vs_qsort();
int cmp_int(const void *a, const void *b);

static double diff(struct timespec start, struct timespec end);

static const int MICROSEC_AS_NSEC = 1000;
static const int SEC_AS_NSEC = 1000000000;

#define SEC_AS_MICROSEC (SEC_AS_NSEC / MICROSEC_AS_NSEC)

int main(int argc, char * * argv) {
    if (argc != 2) {
        printf("Usage: %s -tim_int or %s -tim_vs_q\n", argv[0], argv[0]);
        return -1;
    }

    if (!strcmp("-tim_int", argv[1]))
        timsort_int();

    if (!strcmp("-tim_vs_q", argv[1]))
        timsort_vs_qsort();

    return 0;
}

void timsort_int() {
    size_t N = 0, check = 0;

    check += scanf("%lu", &N);

    int *data = (int *) calloc(N, sizeof(int));

    for (int i = 0; i < N; i++)
        check += scanf("%d", &data[i]);

    timsort(data, N, sizeof(int), &cmp_int);

//#define DUMP
#ifdef DUMP
    for (int i = 0; i < N; i++)
        printf("%d ", data[i]);
#endif
    free(data);
}

void timsort_vs_qsort() {
    size_t N = 0, check = 0;
    struct timespec ts_last, ts_current;
    double timsort_time, qsort_time;

    check += scanf("%lu", &N);

    int *data = (int *) calloc(N, sizeof(int));

    for (int i = 0; i < N; i++)
        check += scanf("%d", &data[i]);

    timespec_get(&ts_last, TIME_UTC);
    timsort(data, N, sizeof(int), &cmp_int);
    timespec_get(&ts_current, TIME_UTC);
    timsort_time = diff(ts_last, ts_current);

    printf("timsort_time = %lf\n", timsort_time); 

    timespec_get(&ts_last, TIME_UTC);
    qsort(data, N, sizeof(int), &cmp_int);
    timespec_get(&ts_current, TIME_UTC);
    qsort_time = diff(ts_last, ts_current);

    printf("qsort_time = %lf\n", qsort_time);

    free(data);
}

int cmp_int(const void *a, const void *b) {
    int *ai = (int *) a;
    int *bi = (int *) b;

    return (*ai) - (*bi);
}

static double diff(struct timespec start, struct timespec end) {
    struct timespec temp;

    if (end.tv_nsec - start.tv_nsec < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = SEC_AS_NSEC + end.tv_nsec - start.tv_nsec;

    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    double msec = temp.tv_sec * SEC_AS_MICROSEC + temp.tv_nsec / MICROSEC_AS_NSEC;
    
    return msec / SEC_AS_MICROSEC;
}
