#include "../timsort.h"
#include "../readstrings.h"
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

static int X_FIELD = 0;

//#define CHECKON
//#define PRINTON

static int test_timsort(char *main_test_name, char *result_file_name);
static unsigned read_tests_files_names(char *main_test_name, char * * *tests_names, unsigned *num_tests, char * *buf);
static int read_test_timsort_data(const char *filename, char * * *data, char * *answers, unsigned *N, unsigned *K, unsigned *X);
static int check_answers(FILE* resultfile, char * *data, char * const answers, const unsigned N, const unsigned K);
int compare_by_x_field_two(void* a, void* b);

static double diff(struct timespec start, struct timespec end);

const int MICROSEC_AS_NSEC = 1000;
const int SEC_AS_NSEC = 1000000000;

#define SEC_AS_MICROSEC (SEC_AS_NSEC / MICROSEC_AS_NSEC)

int main(int argv, char * *argc) {
    if (argv != 3) {
        printf("Incorrect enter\n");
        return -1;
    }

    printf("%d\n", test_timsort(argc[1], argc[2]));

    return 0;
}

static int test_timsort(char *main_test_name, char *result_file_name) {
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
    double logn = 0, ex_time = 0;

    read_strings(&tests_names, &num_tests, &buf, main_test_name, is_symbol_words);
    change_str_ending_buf(buf, is_symbol_words);

    for (unsigned i = 0; i < num_tests; i++) {
        if (read_test_timsort_data(tests_names[i].str, &data, &answers, &N, &K, &X)) {
            printf("read_test_timsort_data failure\n");
            fclose(result_file);
            return -1;
        }

        X_FIELD = X;

#ifdef PRINTON
        printf("before:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < K; j++)
                printf("%hhd ", *(*(data + i) + j));
            
            printf("\n");
        }
#endif

        timespec_get(&ts_last, TIME_UTC);

        timsort(data, N, sizeof(char*), &compare_by_x_field_two);

        timespec_get(&ts_current, TIME_UTC);
    
        fprintf(result_file, "TEST FROM FILE %s: {\n", tests_names[i].str);

        logn = log2(N);
        ex_time = diff(ts_last, ts_current);

        fprintf(result_file, "\t------------------------------------------------\n");
        fprintf(result_file, "\tN = %u       | Log(N) = %lf\n", N, logn);
        fprintf(result_file, "\t------------------------------------------------\n");
        fprintf(result_file, "\tExecution time: %lf\n", ex_time);
        fprintf(result_file, "\t------------------------------------------------\n");
        fprintf(result_file, "\tTime * 10^6 / (N*Log(N)) = %lf\n", ex_time * 1000000 / (N * logn));
        fprintf(result_file, "\t------------------------------------------------\n");
#ifdef CHECKON
        check_answers(result_file, data, answers, N, K);
#endif
        fprintf(result_file, "\t------------------------------------------------\n");

        fprintf(result_file, "}\n\n");

#ifdef PRINTON
        printf("after:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < K; j++)
                printf("%hhd ", *(*(data + i) + j));
            
            printf("\n");
        }
#endif 

        free(data);
        free(answers);
    }

    fclose(result_file);

    clean_strings(&tests_names, &buf);

    return 0;
}

static int read_test_timsort_data(const char *filename, char * * *data, char * *answers, unsigned *N, unsigned *K, unsigned *X) {
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

#ifdef CHECKON
    for (int i = 0; i < *N; i++)
        check += fscanf(testfile, "%hhd", *answers + i);
#endif 
    fclose(testfile);

#ifdef CHECKON
    if (check != (*N) * (*K + 1) + 3) {
        printf("failure while reading a file\n");
        return -1;
    }
#else
    if (check != (*N) * (*K) + 3) {
        printf("failure while reading a file\n");
        return -1;
    }
#endif

    return 0;
}

static int check_answers(FILE* resultfile, char * *data, char * const answers, const unsigned N, const unsigned K) {
    for (unsigned i = 0; i < N; i++) {
        if (*(*(data + i) + (i % K)) != *(answers + i)) {
            fprintf(resultfile, "incorrect answer: *(data + %u) + %u = %hhd, but *(answers + %u) == %hhd\n", i, i % K, *(*(data + i) + (i % K)), i, *(answers + i));
            //return -1;
        }
    }

    fprintf(resultfile, "\tOK\n");

    return 0;
}

int compare_by_x_field_two(void* a, void* b) {
    char** ac = (char**) a;
    char** bc = (char**) b;

    return (*(*ac + X_FIELD)) - (*(*bc + X_FIELD));
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
