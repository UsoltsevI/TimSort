#include "TimSort.h"
#include "ReadStrings.h"
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

//I know that global varables is not good
//but I don't know how I can pass the comparison function the value for 
//the field number by witch to compare...
int X_FIELD = 0; //a global variable for passing the value of the comparison function

//#define PT printf("L: %d\n", __LINE__);
//#define DEBUGONTST //to display array data on the screen

//#define CHECKON

int test_timsort(char *main_test_name, char *result_file_name);
unsigned read_tests_files_names(char *main_test_name, char * * *tests_names, unsigned *num_tests, char * *buf);
int read_test_timsort_data(const char *filename, char * *data, char * *answers, unsigned *N, unsigned *K, unsigned *X);
void printf_data(const char* filename, char* const data, const unsigned N, const unsigned K, const unsigned X);
int check_answers(FILE* resultfile, char* const data, char* const answers, const unsigned N, const unsigned K);

int compare_by_x_field(void *lhs, void* rhs);

static double diff(struct timespec start, struct timespec end);

const int MICROSEC_AS_NSEC = 1000;
const int SEC_AS_NSEC = 1000000000;

#define SEC_AS_MICROSEC (SEC_AS_NSEC / MICROSEC_AS_NSEC)

int main(int argv, char * *argc) {
    if (argv != 3) {
        printf("Incorrect enter\n");
        return -1;
    }

    printf("end: %d\n", test_timsort(argc[1], argc[2]));

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
    char *data = NULL;
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
#ifdef DEBUGONTST
        printf("data:\n");
        for (int i = 0; i < N * K; i++) {
            if (i % K == 0)
                printf("\nL%d: ", i / K);

            printf("%hhd ", *(data + i));

        }

        printf("\n");
#endif
        timespec_get(&ts_last, TIME_UTC);
        
        timsort(data, N, sizeof(char) * K, &compare_by_x_field);

        timespec_get(&ts_current, TIME_UTC);
    
#ifdef DEBUGONTST
        printf("\ndata:");
        for (int i = 0; i < N * K; i++) {
            if (i % K == 0)
                printf("\nL%d: ", i / K);

            printf("%hhd ", *(data + i));

        }
        printf("\n");
#endif
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

        //printf_data("Tests/Test30.txt", data, N, K, X);

        free(data);
        free(answers);
    }

    fclose(result_file);

    clean_strings(&tests_names, &buf);

    return 0;
}

int read_test_timsort_data(const char *filename, char * *data, char * *answers, unsigned *N, unsigned *K, unsigned *X) {
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

    *data = (char *) calloc((*N) * (*K), sizeof(char *));
    *answers = (char *) calloc(*N, sizeof(char));

    if ((data == NULL) || (answers == NULL)) {
        printf("failure callocation\n");
        fclose(testfile);
        return -1;
    }

    for (int i = 0; i < (*N) * (*K); i++)
            check += fscanf(testfile, "%hhd", (*data + i));

#ifdef CHECKON
    for (int i = 0; i < *N; i++)
        check += fscanf(testfile, "%hhd", *answers + i);
#endif

    fclose(testfile);

#ifdef CHECKON
    if (check != (*N) * ((*K) + 1) + 3) {
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

void printf_data(const char* filename, char* const data, const unsigned N, const unsigned K, const unsigned X) {
    FILE* fout = fopen(filename, "w");

    if (!fout) {
        printf("Faiulre opening file\n");
        return;
    }
    
    fprintf(fout, "%u %u %u", N, K, X);
    
    for (unsigned i = 0; i < N * K; i++) {
        if (i % K == 0)
            fprintf(fout, "\n");

        fprintf(fout, "%hhd ", *(data + i));
    }

    fclose(fout);
}

int check_answers(FILE* resultfile, char* const data, char* const answers, const unsigned N, const unsigned K) {
    for (unsigned i = 0; i < N; i++) {
        if (*(data + i * K + i % K) != *(answers + i)) {
            fprintf(resultfile, "incorrect answer: *(data + %u + %u) = %hhd, but *(answers + %u) == %hhd\n", i * K, i % K, *(data + i * K + i % K), i, *(answers + i));
            //return -1;
        }
    }

    fprintf(resultfile, "\tOK\n");

    return 0;
}

int compare_by_x_field(void* const lhs, void* const rhs) {
    char* lhs_c = (char*) lhs;
    char* rhs_c = (char*) rhs;
    return *(lhs_c + X_FIELD) - *(rhs_c + X_FIELD);
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
