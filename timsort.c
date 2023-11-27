#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "TimSort.h"

#include <assert.h>

#define RUNON
//#define DUMP

static const size_t MIN_LEN_TO_TIMSORT = 64;
static const size_t RUN_THRESHHOLD_VAL = 4;

struct subarray {
    size_t beg;
    size_t size;
};

struct subarr_stack {
    struct subarray* data;
    size_t capacity;
    size_t size;
};

static void timsort_imp(void * const mem, const size_t len, const size_t size_elem, int (*cmp)(const void *, const void *));
static void timsort_merge(void * const mem, const size_t size_elem, const size_t left, const size_t mid, const size_t right, int (*cmp)(const void *, const void *));
static void timsort_blance(void * const mem, const size_t size_elem, const size_t len, struct subarr_stack * const stk, int (*cmp)(const void *, const void *));
static void timsort_gallop(void * const mem, void * const mem_first, void * const mem_second, size_t *pos_first, size_t *pos_second,
                    size_t *i, const size_t last_accept_pos_first, const size_t last_accept_pos_second, const size_t size_elem, int (*cmp)(const void *, const void *));

static void inssort(void * const arr, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *));
static size_t moveright(void * const arr, void * const key, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *));
static size_t get_minrun(size_t n);

static void new_stack(struct subarr_stack * const stk, const size_t beg_capacity);
static void push_stack(struct subarr_stack * const stk, struct subarray * const new_elem);
static void pop_stack(struct subarr_stack * const stk, struct subarray * const out);

#define DEBUGON
#ifdef DEBUGON
#include <time.h>
static const int MICROSEC_AS_NSEC = 1000;
static const int SEC_AS_NSEC = 1000000000;

#define SEC_AS_MICROSEC (SEC_AS_NSEC / MICROSEC_AS_NSEC)

static int checkon(const void *mem, const size_t left, const size_t right, const size_t size_elem, int (*cmp) (const void *, const void *));

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

#endif

void timsort(void * const mem, const size_t len, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    if (len <= MIN_LEN_TO_TIMSORT) {
        inssort(mem, 0, len, size_elem, cmp);
        return;
    } 

    timsort_imp(mem, len, size_elem, cmp);
}

static void timsort_imp(void * const mem, const size_t len, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    size_t minrun = get_minrun(len);
    size_t add_step = 0, cur_pos = 0;
    size_t is_sorted = 0;

    struct subarr_stack stk;
    struct subarray subarr_x; 
    struct subarray subarr_y; 
#ifdef DUMP
    printf("minrun = %lu, len = %lu, size_elem = %lu\n", minrun, len, size_elem);
#endif
    new_stack(&stk, len / minrun + 1);   

    struct timespec tbeg, tend;
    double time_ins = 0, time_bal = 0, time_mer = 0;

    while (cur_pos < len) {
        add_step = cur_pos;
        is_sorted = 1;

        while ((add_step < len) && ((add_step > len - minrun) || (add_step - cur_pos < minrun) || (is_sorted))) {
            if (is_sorted) 
                if ((add_step + 1 < len) && (cmp(mem + add_step * size_elem, mem + (add_step + 1) * size_elem) > 0))
                    is_sorted = 0;
            
            add_step++;
        }
#ifdef TIME
        timespec_get(&tbeg, TIME_UTC);
#endif
        if (!is_sorted)
            inssort(mem + cur_pos * size_elem, 0, add_step - cur_pos, size_elem, cmp);
#ifdef TIME
        timespec_get(&tend, TIME_UTC);
        //printf("diff ins = %lf\n\n", diff(tbeg, tend));
        time_ins += diff(tbeg, tend);
#endif 
        subarr_x.beg = cur_pos;
        subarr_x.size = add_step - cur_pos;
        cur_pos = add_step; 

        //printf("%lu ", subarr_x.size);

        push_stack(&stk, &subarr_x);
#ifdef TIME
        timespec_get(&tbeg, TIME_UTC);
#endif 
        timsort_blance(mem, size_elem, len, &stk, cmp);
#ifdef TIME
        timespec_get(&tend, TIME_UTC);
        //printf("diff bal = %lf\n", diff(tbeg, tend));
        time_bal += diff(tbeg, tend);
#endif
    }
#ifdef DUMP
    printf("\nstack:\n");
    for (int i = 0; i < stk.size; i++)
        printf("%lu ", stk.data[i].size);

    printf("\n");
#endif
    while (stk.size > 1) { 
        pop_stack(&stk, &subarr_x);
        pop_stack(&stk, &subarr_y);
#ifdef TIME
        timespec_get(&tbeg, TIME_UTC);
#endif
        timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);
#ifdef TIME
        timespec_get(&tend, TIME_UTC);

        //printf("diff merge = %lf\n", diff(tbeg, tend));
        time_mer += diff(tbeg, tend);
#endif 
        subarr_y.size += subarr_x.size;

        push_stack(&stk, &subarr_y);
    } 
#ifdef TIME
    printf("time_ins = %lf, time_bal = %lf, time_mer = %lf\n", time_ins, time_bal, time_mer);
#endif 
    free(stk.data);
}   

static void timsort_merge(void * const mem, const size_t size_elem, const size_t left, const size_t mid, const size_t right, int (*cmp)(const void *, const void *)) {
    size_t len_copy = mid - left;
    void *mem_copy = malloc(len_copy * size_elem);
    size_t pos_copy = 0, pos_right = mid;
    size_t run_detector = 0, last_choise = 0;

#ifdef DUMP
    printf("input data: \n");
    printf("left = %lu, mid = %lu, right = %lu\n", left, mid, right);
    for (int i = left; i < mid; i++) {
        int *ai = (int *) (mem + i * size_elem);
        printf("%d ", *ai);
    }
    printf("\n");
    for (int i = mid; i <= right; i++) {
        int *bi = (int *) (mem + i * size_elem);
        printf("%d ", *bi);
    }
    printf("\n");
#endif 

    memcpy(mem_copy, mem + left * size_elem, len_copy * size_elem);

    for (size_t i = left; i < right + 1; i++) {
        //printf("i=%lu\n", i);
        assert(i <= pos_right);
        if ((pos_right > right) || ((pos_copy < len_copy) && (cmp(mem_copy + pos_copy * size_elem, mem + pos_right * size_elem) <= 0))) {
            memcpy(mem + i * size_elem, mem_copy + pos_copy * size_elem, size_elem);

#ifdef RUNON
            if (last_choise == 1) {
                last_choise = 0;
                run_detector = 0;

            } else {
                run_detector++;
                
                if (run_detector > RUN_THRESHHOLD_VAL) {
                    size_t first_run_elem = pos_copy + 1;
#ifdef DUMP
                    printf("run left!\n");
                    printf("pos_copy = %lu\n", pos_copy);
                    printf("i = %lu\n", i);
#endif 
                    if (pos_right > right) {
#ifdef DUMP
                        printf("ver acc!\n");
#endif
                        //assert(0);
                        pos_copy = len_copy - 2;
#ifdef DUMP
                        printf("pos_copy = len_copy = %lu\n", pos_copy);
#endif
                    }

                    while ((pos_copy + 1 < len_copy) && (cmp(mem_copy + (pos_copy + 1) * size_elem, mem + pos_right * size_elem) <= 0)) {
#ifdef DUMP
                        printf("PLUS_PLUS!!!\n");
#endif
                        pos_copy++;
                    }

                    if (pos_copy >= first_run_elem) {
                        i++;
#ifdef DUMP
                        printf("MOVE!\n");
                        printf("%lu vs %lu\n", i + pos_copy - first_run_elem + 1, right);
#endif
                        assert(i + pos_copy - first_run_elem + 1 <= right);
                        //assert(i + pos_copy - first_run_elem + 1 < right);
                        memmove(mem + i * size_elem, mem_copy + first_run_elem * size_elem, (pos_copy - first_run_elem + 1) * size_elem);
                        i += pos_copy - first_run_elem;

                    }
#ifdef DUMP
                    printf("pos_copy - first_run_elem + 1 = %lu\n", pos_copy - first_run_elem + 1);
                    printf("pos_copy = %lu\n", pos_copy);
                    printf("i = %lu\n\n", i);
#endif

                    assert(i < right + 1);

                    last_choise = 1;
                    run_detector = 0;
                }
            }
#endif
            pos_copy++;

        } else {
            memcpy(mem + i * size_elem, mem + pos_right * size_elem, size_elem);

#ifdef RUNON
            if (last_choise == 0) {
                last_choise = 1;
                run_detector = 0;

            } else {
                run_detector++;

                if (run_detector > RUN_THRESHHOLD_VAL) {
                    size_t first_run_elem = pos_right + 1;
#ifdef DUMP
                    printf("run right!\n");
                    printf("pos_right = %lu\n", pos_right);
                    printf("i = %lu\n", i);
#endif 
                    if (pos_copy >= len_copy) {
#ifdef DUMP
                        printf("ver acc!\n");
#endif
                        pos_right = right;
                    }

                    assert(pos_right <= right);
                    assert(pos_copy <= len_copy);

                    while ((pos_right < right) && (cmp(mem_copy + pos_copy * size_elem, mem + (pos_right + 1) * size_elem) > 0)) {
                        assert(pos_right < right);
                        assert(pos_copy < len_copy);
#ifdef DUMP
                        printf("PLUS_PLUS!!!\n");
#endif
                        pos_right++;
                    }
                    
                    if (pos_right >= first_run_elem) {
#ifdef DUMP
                        printf("MOVE!\n");
                        printf("%lu vs %lu vs %lu\n", i, pos_right, right);
#endif
                        assert(i < pos_right);
                        assert(pos_right <= right);

                        memmove(mem + (i + 1) * size_elem, mem + first_run_elem * size_elem, (pos_right - first_run_elem + 1) * size_elem);
                        i += pos_right - first_run_elem + 1;
#ifdef DUMP
                        printf("pos_right - first_run_elem + 1 = %lu\n", pos_right - first_run_elem + 1);
#endif
                    }
#ifdef DUMP
                    printf("pos_right = %lu\n", pos_right);
                    printf("i = %lu\n\n", i);
#endif
                    last_choise = 0;
                    run_detector = 0;
                }
            }
#endif

            pos_right++;
        }
    }

#ifdef DUMP
    printf("output data:\n");
    for (int i = left; i <= right; i++) {
        int *bi = (int *) (mem + i * size_elem);
        printf("'%d' ", *bi);
    }
    printf("\n\n");
#endif
    assert(checkon(mem, left, right, size_elem, cmp));
    
    free(mem_copy);
}

static void timsort_blance(void * const mem, const size_t size_elem, const size_t len, struct subarr_stack * const stk, int (*cmp)(const void *, const void *)) {
    if (stk->size > 2) { 
        struct subarray subarr_x;
        struct subarray subarr_y;
        struct subarray subarr_z;

        pop_stack(stk, &subarr_x);
        pop_stack(stk, &subarr_y);
        pop_stack(stk, &subarr_z);

        if ((subarr_z.size < subarr_y.size + subarr_x.size) || (subarr_y.size < subarr_x.size)) {
            if ((subarr_x.size < subarr_z.size) && (subarr_y.size < subarr_z.size) && (subarr_z.size > subarr_y.size + subarr_x.size)) {
                timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);

                subarr_y.size += subarr_x.size;

                push_stack(stk, &subarr_z);
                push_stack(stk, &subarr_y);

            } else {
                timsort_merge(mem, size_elem, subarr_z.beg, subarr_y.beg, subarr_y.beg + subarr_y.size - 1, cmp);

                subarr_z.size += subarr_y.size; 

                push_stack(stk, &subarr_z);
                push_stack(stk, &subarr_x);
            }

            timsort_blance(mem, size_elem, len, stk, cmp);

        } else {
            push_stack(stk, &subarr_z);
            push_stack(stk, &subarr_y);
            push_stack(stk, &subarr_x);
        }
    } 
}

static void inssort(void * const arr, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    size_t i = 0, pos = 0;
    void *key = malloc(size_elem);
    /*printf("last%ld - first = %ld\n", last, last - first);
    printf("arr = %p\n", arr);
    printf("arr + first * size_elem = %p\n", arr + first * size_elem);
    char *ch = (char *) arr;
    printf("%d %d %d\n", *ch, *(ch + 1), *(ch + 2));
    printf("%d %d %d\n", *(ch + size_elem * first), *(ch + size_elem * first + 1), *(ch + size_elem * first + 2));*/

    for (i = first; i < last; i++) { 
        memcpy(key, arr + i * size_elem, size_elem);
        pos = moveright(arr, key, first, i, size_elem, cmp);
        memcpy(arr + pos * size_elem, key, size_elem);
    }

    free(key);
}

static size_t moveright(void * const arr, void * const key, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    size_t ise = 0;

    for (size_t i = first; i < last; i++) {
        ise = i * size_elem;

        if (cmp(key, arr + ise) < 0) {     
            memmove(arr + ise + size_elem, arr + ise, size_elem * (last - i));

            return i;
        }
    }
    
    return last;
}

static size_t get_minrun(size_t n) {
    size_t r = 0;

    while (n >= 64) {
        r |= (n & 1u);
        n >>= 1u;
    }

    return r + n;
}

static void new_stack(struct subarr_stack * const stk, const size_t beg_capacity) {
    stk->capacity = beg_capacity;
    stk->size = 0;
    stk->data = (struct subarray *) malloc(beg_capacity * sizeof(struct subarray));
}

static void push_stack(struct subarr_stack * const stk, struct subarray * const new_elem) {
    stk->data[stk->size].beg  = new_elem->beg;
    stk->data[stk->size].size = new_elem->size;
    stk->size++;
}

static void pop_stack(struct subarr_stack * const stk, struct subarray * const out) {
    stk->size--;
    out->beg  = stk->data[stk->size].beg;
    out->size = stk->data[stk->size].size;
}

static int checkon(const void *mem, const size_t left, const size_t right, const size_t size_elem, int (*cmp) (const void *, const void *)) {
    for (size_t i = left; i < right; i++) 
        if (cmp(mem + i * size_elem, mem + (i + 1) * size_elem) > 0) {
            printf("erri = %lu\n", i);
            printf("erri - left = %lu\n", i - left);
            return 0;
        }

    return 1;
}
