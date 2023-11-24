#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "TimSort.h"

static const size_t MIN_LEN_TO_TIMSORT = 64;
static const size_t RUN_THRESHHOLD_VAL = 7;

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
static size_t moveright(void * const arr, void * const key, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *));
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

    printf("minrun = %lu, len = %lu\n", minrun, len);

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

        timespec_get(&tbeg, TIME_UTC);
        if (!is_sorted)
            inssort(mem, cur_pos, add_step, size_elem, cmp);
        timespec_get(&tend, TIME_UTC);
        //printf("diff ins = %lf\n", diff(tbeg, tend));
        time_ins += diff(tbeg, tend);

        subarr_x.beg = cur_pos;
        subarr_x.size = add_step - cur_pos;
        cur_pos = add_step;

        printf("%lu ", subarr_x.size);

        push_stack(&stk, &subarr_x);

        timespec_get(&tbeg, TIME_UTC);
        timsort_blance(mem, size_elem, len, &stk, cmp);
        timespec_get(&tend, TIME_UTC);
        //printf("diff bal = %lf\n", diff(tbeg, tend));
        time_bal += diff(tbeg, tend);
    }

    printf("\nstack:\n");
    for (int i = 0; i < stk.size; i++)
        printf("%lu ", stk.data[i].size);

    printf("\n");

    while (stk.size > 1) { 
        pop_stack(&stk, &subarr_x);
        pop_stack(&stk, &subarr_y);

        timespec_get(&tbeg, TIME_UTC);
        timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);
        timespec_get(&tend, TIME_UTC);
        //printf("diff merge = %lf\n", diff(tbeg, tend));
        time_mer += diff(tbeg, tend);

        subarr_y.size += subarr_x.size;

        push_stack(&stk, &subarr_y);
    } 

    printf("time_ins = %lf, time_bal = %lf, time_mer = %lf\n", time_ins, time_bal, time_mer);

    free(stk.data);
}   

static void timsort_merge(void * const mem, const size_t size_elem, const size_t left, const size_t mid, const size_t right, int (*cmp)(const void *, const void *)) {
    void *mem_copy = malloc((mid - left + 1) * size_elem);
    size_t pos_copy = 0, pos_right = mid + 1;
    size_t run_detector = 0, last_choise = 0;

    memcpy(mem_copy, mem + left * size_elem, (mid - left + 1) * size_elem);

    for (size_t i = left; i < right + 1; i++) {
        if ((pos_right > right) || ((pos_copy < mid - left + 1) && (cmp(mem_copy + pos_copy * size_elem, mem + pos_right * size_elem) <= 0))) {
            memcpy(mem + i * size_elem, mem_copy + pos_copy * size_elem, size_elem);

            if (last_choise == 1) {
                last_choise = 0;
                run_detector = 0;

            } else {
                run_detector++;
                
                if (run_detector > RUN_THRESHHOLD_VAL) {
                    timsort_gallop(mem, mem_copy, mem, &pos_copy, &pos_right, &i, mid - left, right, size_elem, cmp);
                    last_choise = 1;
                }
            }

            pos_copy++;

        } else {
            memcpy(mem + i * size_elem, mem + pos_right * size_elem, size_elem);

            if (last_choise == 0) {
                last_choise = 1;
                run_detector = 0;

            } else {
                run_detector++;

                if (run_detector > RUN_THRESHHOLD_VAL) {
                    timsort_gallop(mem, mem, mem_copy, &pos_right, &pos_copy, &i, right, mid - left, size_elem, cmp);
                    last_choise = 0;
                }
            }

            pos_right++;
        }
    }
    
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
                timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size, cmp);

                subarr_y.size += subarr_x.size;

                push_stack(stk, &subarr_z);
                push_stack(stk, &subarr_y);

            } else {
                timsort_merge(mem, size_elem, subarr_z.beg, subarr_y.beg, subarr_y.beg + subarr_y.size, cmp);

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

static void timsort_gallop(void * const mem, void * const mem_first, void * const mem_second, size_t *pos_first, size_t *pos_second,
                    size_t *i, const size_t last_accept_pos_first, const size_t last_accept_pos_second, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    size_t first_run_elem = *pos_first + 1;

    if (*pos_second > last_accept_pos_second)
        *pos_first = last_accept_pos_first;

    while ((*pos_first  + 1 < last_accept_pos_first) && (cmp(mem_first + (*pos_first + 1) * size_elem, mem_second + (*pos_second) * size_elem) <= 0))
        (*pos_first)++;

    if (*pos_first > first_run_elem) {
        (*i)++;
        memmove(mem + (*i) * size_elem, mem_first + first_run_elem * size_elem, (*pos_first - first_run_elem + 1) * size_elem);
        *i += *pos_first - first_run_elem;
    }
}

static void inssort(void * const arr, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    size_t i = 0, pos = 0;
    void *key = malloc(size_elem);

    for (i = first; i < last; i++) { 
        memcpy(key, arr + i * size_elem, size_elem);
        pos = moveright(arr, key, i, size_elem, cmp);
        memcpy(arr + pos * size_elem, key, size_elem);
    }

    free(key);
}

static size_t moveright(void * const arr, void * const key, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    for (size_t i = 0; i < last; i++) {
        if (cmp(key, arr + i * size_elem) < 0) {     
            memmove(arr + (i + 1) * size_elem, arr + i * size_elem, size_elem * (last - i));

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