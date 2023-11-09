#include "TimSort.h"
#include "ReadStrings.h"
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

int test_timsort(char *main_test_name, char *result_file_name);
unsigned read_tests_files_names(char *main_test_name, char * * *tests_names, unsigned *num_tests, char * *buf);
int read_test_timsort_data(const char *filename, char * * *data, char * *answers, unsigned *N, unsigned *K, unsigned *X);
int check_answers(FILE* resultfile, char * *data, char * const answers, const unsigned N, const unsigned K);

int main(int argv, char * *argc) {
    if (argv != 3) {
        printf("Incorrect enter\n");
        return -1;
    }

    printf("%d\n", test_timsort(argc[1], argc[2]));

    return 0;
}

int test_timsort(char *main_test_name, char *result_file_name) {
    FILE *result_file = fopen(result_file_name, "w");
    if (result_file == NULL) {
        printf("failure when opening file for writing\n");
        return -1;
    }

    unsigned N = 0, K = 0, X = 0;
    size_t num_tests = 0;
    unsigned buf_capacity = 0;
    struct string *tests_names = NULL;
    char * *data = NULL;
    char *answers = NULL;
    char *buf = NULL;
    struct timespec ts_last, ts_current;
    double nlogn = 0;

    read_strings(&tests_names, &num_tests, &buf, main_test_name);
    change_str_ending_buf(buf);

    for (unsigned i = 0; i < num_tests; i++) {
        if (read_test_timsort_data(tests_names[i].str, &data, &answers, &N, &K, &X)) {
            printf("read_test_timsort_data failure\n");
            fclose(result_file);
            return -1;
        }

        timespec_get(&ts_last, TIME_UTC);

        timsort(data, N, K, X);

        timespec_get(&ts_current, TIME_UTC);
    
        fprintf(result_file, "TEST FROM FILE %s: {\n", tests_names[i].str);

        nlogn = N * log2(N);

        fprintf(result_file, "\t------------------------------------------------\n");
        fprintf(result_file, "\tN = %u       | N*Log(N) = %lf\n", N, nlogn);
        fprintf(result_file, "\t------------------------------------------------\n");
        fprintf(result_file, "\tExecution time: %lu.%.09ld \n", ts_current.tv_sec - ts_last.tv_sec, ts_current.tv_nsec - ts_last.tv_nsec);
        fprintf(result_file, "\t------------------------------------------------\n");
        fprintf(result_file, "\tTime / nlogn = %lf\n", ts_current.tv_nsec - ts_last.tv_nsec / nlogn);
        fprintf(result_file, "\t------------------------------------------------\n");

        check_answers(result_file, data, answers, N, K);
        fprintf(result_file, "\t------------------------------------------------\n");

        fprintf(result_file, "}\n\n");

        free(data);
        free(answers);
    }

    fclose(result_file);

    clean_strings(&tests_names, &buf);

    return 0;
}

int read_test_timsort_data(const char *filename, char * * *data, char * *answers, unsigned *N, unsigned *K, unsigned *X) {
    int check = 0;
    FILE *testfile = fopen(filename, "r");

    if (testfile == NULL) {
        printf("failure while opening a file for reading\n");
        return -1;
    }

    check = fscanf(testfile, "%u%u%u", N, K, X);

    if (check != 3) {
        printf("failure while reading a file\n");
        fclose(testfile);
        return -1;
    }

    *data = (char * *) calloc(*N, sizeof(char *));
    *answers = (char *) calloc(*N, sizeof(char));

    if ((data == NULL) || (answers == NULL)) {
        printf("failure callocation\n");
        fclose(testfile);
        return -1;
    }

    for (int i = 0; i < *N; i++) {
        *(*data + i) = (char *) calloc(*K, sizeof(char));

        for (int j = 0; j < *K; j++)
            check += fscanf(testfile, "%hhd", *(*data + i) + j);
    }

    for (int i = 0; i < *N; i++)
        check += fscanf(testfile, "%hhd", *answers + i);

    fclose(testfile);

    if (check != (*N) * (*K + 1) + 3) {
        printf("failure while reading a file\n");
        return -1;
    }

    return 0;
}

int check_answers(FILE* resultfile, char * *data, char * const answers, const unsigned N, const unsigned K) {
    for (unsigned i = 0; i < N; i++) {
        if (*(*(data + i) + (i % K)) != *(answers + i)) {
            fprintf(resultfile, "incorrect answer: *(data + %u) + %u = %hhd, but *(answers + %u) == %hhd\n", i, i % K, *(*(data + i) + (i % K)), i, *(answers + i));
            //return -1;
        }
    }

    fprintf(resultfile, "\tOK\n");

    return 0;
}
