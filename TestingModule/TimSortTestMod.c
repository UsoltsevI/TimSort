#include "../timsort.h"
#include "../readstrings.h"
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

//I know that global varables is not good
//but I don't know how I can pass the comparison function the value for 
//the field number by witch to compare...
int MOD = 0; //a global variable for passing the value of the comparison function

//#define DEBUGONTST //to display array data on the screen

//turn it on if ypu want to check answers
#define CHECKON

int test_timsort(char *main_test_name, char *result_file_name);
static int read_test_timsort_data(const char *filename, int * *data, int * *answers, unsigned *N, int *m);
static void printf_data(const char* filename, int* const data, const unsigned N, const unsigned m);
static int check_answers(FILE* resultfile, int* const data, int* const answers, const unsigned N);

static int compare_by_mod(const void *lhs, const void* rhs);

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

    unsigned N = 0, m = 0;
    size_t num_tests = 0;
    unsigned buf_capacity = 0;
    struct string *tests_names = NULL;
    int *data = NULL;
    int *answers = NULL;
    char *buf = NULL;
    struct timespec ts_last, ts_current;
    double logn = 0, ex_time = 0;

    read_strings(&tests_names, &num_tests, &buf, main_test_name, is_symbol_words);
    change_str_ending_buf(buf, is_symbol_words);

    for (unsigned i = 0; i < num_tests; i++) {
        if (read_test_timsort_data(tests_names[i].str, &data, &answers, &N, &m)) {
            printf("read_test_timsort_data failure\n");
            fclose(result_file);
            return -1;
        }

        MOD = m;
#ifdef DEBUGONTST
        printf("data:\n");
        for (int i = 0; i < N; i++)
            printf("%d ", *(data + i));

        printf("\n");
#endif
        timespec_get(&ts_last, TIME_UTC);
        
        timsort(data, N, sizeof(int), &compare_by_mod);

        timespec_get(&ts_current, TIME_UTC);
    
#ifdef DEBUGONTST
        printf("data:\n");
        for (int i = 0; i < N; i++)
            printf("%d ", *(data + i));

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
        check_answers(result_file, data, answers, N);
#endif 

        fprintf(result_file, "\t------------------------------------------------\n");

        fprintf(result_file, "}\n\n");

        //printf_data("Tests/Test21.txt", data, N, m);

        free(data);
        free(answers);
    }

    fclose(result_file);

    clean_strings(&tests_names, &buf);

    return 0;
}

static int read_test_timsort_data(const char *filename, int * *data, int * *answers, unsigned *N, int *m) {
    int check = 0;
    FILE *testfile = fopen(filename, "r");

    if (testfile == NULL) {
        printf("failure while opening a file for reading\n");
        return -1;
    }

    check = fscanf(testfile, "%u", N);

    if (check != 1) {
        printf("failure while reading a file\n");
        fclose(testfile);
        return -1;
    }

    *data = (int *) calloc(*N, sizeof(int));
    *answers = (int *) calloc(*N, sizeof(int));

    if ((data == NULL) || (answers == NULL)) {
        printf("failure callocation\n");
        fclose(testfile);
        return -1;
    }

    for (int i = 0; i < (*N); i++)
        check += fscanf(testfile, "%d", *data + i);

    check += fscanf(testfile, "%u", m);
    

#ifdef CHECKON
    for (int i = 0; i < *N; i++)
        check += fscanf(testfile, "%d", *answers + i);
#endif

    fclose(testfile);

#ifdef CHECKON
    if (check != (*N) * 2 + 2) {
        printf("failure while reading a file (with checking)\n");
        return -1;
    }
#else 
    if (check != (*N) + 2) {
        printf("failure while reading a file\n");
        return -1;
    }
#endif

    return 0;
}

static void printf_data(const char* filename, int* const data, const unsigned N, const unsigned m) {
    FILE* fout = fopen(filename, "w");

    if (!fout) {
        printf("Faiulre opening file\n");
        return;
    }
    
    fprintf(fout, "%u\n", N);
    
    for (unsigned i = 0; i < N; i++)
        fprintf(fout, "%d ", *(data + i));

    fprintf(fout, "\n%d\n", m);

    fclose(fout);
}

static int check_answers(FILE* resultfile, int* const data, int* const answers, const unsigned N) {
    for (unsigned i = 0; i < N; i++) {
        if (*(data + i) != *(answers + i)) {
            fprintf(resultfile, "incorrect answer: *(data + %u) = %d, but *(answers + %u) == %d\n", i, *(data + i), i, *(answers + i));
            //return -1;
        }
    }

    fprintf(resultfile, "\tOK\n");

    return 0;
}

static int compare_by_mod(const void* lhs, const void* rhs) {
    int* lhs_c = (int*) lhs;
    int* rhs_c = (int*) rhs;
    return ((*lhs_c) % MOD) - ((*rhs_c) % MOD);
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
