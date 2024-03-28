#include <iostream>
#include <functional>
#include <algorithm>
#include <ctime>
#include <cstring>
#include "timsort.hpp"

void timsort_int();
void timsort_vs_qsort();

static double diff(struct timespec start, struct timespec end);

static const int MICROSEC_AS_NSEC = 1000;
static const int SEC_AS_NSEC = 1000000000;

#define SEC_AS_MICROSEC (SEC_AS_NSEC / MICROSEC_AS_NSEC)

int main(int argc, char * * argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " -tim_int or " << argv[0] << " -tim_vs_q\n";
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

    std::cin >> N;

    int *data = new int[N];

    for (int i = 0; i < N; i++)
        std::cin >> data[i];

    struct timespec ts_last, ts_current;
    double timsort_time;

    timespec_get(&ts_last, TIME_UTC);
    timsortcls<int, std::greater<int>>::timsort(data, N, std::greater<int>());
    timespec_get(&ts_current, TIME_UTC);

    timsort_time = diff(ts_last, ts_current);
    std::cout << "timsort execution time = " << timsort_time << std::endl; 

    for (int i = 0; i < N; i++)
        std::cout << data[i] << ' ';
    
    std::cout << std::endl;

    delete[] data;
}

int cmp_int(const void *a, const void *b) {
    int *ai = (int *) a;
    int *bi = (int *) b;

    return (*ai) - (*bi);
}

void timsort_vs_qsort() {
    size_t N = 0, check = 0;
    struct timespec ts_last, ts_current;
    double timsort_time, qsort_time;

    std::cin >> N;

    int* data1 = new int[N];
    int* data2 = new int[N];

    std::cout << __LINE__ << std::endl;

    for (int i = 0; i < N; i++) {
        std::cin >> data1[i];
        data2[i] = data1[i];
    }

    std::cout << __LINE__ << std::endl;

    timespec_get(&ts_last, TIME_UTC);
    timsortcls<int, std::greater<int>>::timsort(data1, N, std::greater<int>());
    timespec_get(&ts_current, TIME_UTC);
    timsort_time = diff(ts_last, ts_current);

    std::cout << __LINE__ << std::endl;

    std::cout << "timsort_time = " << timsort_time << std::endl; 

    timespec_get(&ts_last, TIME_UTC);
    // std::sort<int, std::greater<int>>(data2, N, std::greater<int>());
    std::sort(data2, data2 + N);
    timespec_get(&ts_current, TIME_UTC);
    qsort_time = diff(ts_last, ts_current);

    std::cout << "qsort_time = " << qsort_time << std::endl;

    std::cout << __LINE__ << std::endl;

    delete[] data1;
    delete[] data2;
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
