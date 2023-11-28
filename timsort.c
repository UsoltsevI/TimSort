#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "TimSort.h"

#include <assert.h>

#define RUNON
//#define DUMP

static const size_t MIN_LEN_TO_TIMSORT = 64;

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

static void inssort(void * const arr, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *));
static size_t moveright(void * const arr, void * const key, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *));
static size_t get_minrun(size_t n);

static void new_stack(struct subarr_stack * const stk, const size_t beg_capacity);
static void push_stack(struct subarr_stack * const stk, struct subarray * const new_elem);
static void pop_stack(struct subarr_stack * const stk, struct subarray * const out);

//#define TIME
#ifdef TIME
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

struct timespec tbeg, tend;
double time_ins = 0, time_bal = 0, time_mer = 0;

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

    new_stack(&stk, len / minrun + 1);   

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
        time_ins += diff(tbeg, tend);
#endif 
        subarr_x.beg = cur_pos;
        subarr_x.size = add_step - cur_pos;
        cur_pos = add_step; 

        push_stack(&stk, &subarr_x);

#ifdef TIME
        timespec_get(&tbeg, TIME_UTC);
#endif 
        timsort_blance(mem, size_elem, len, &stk, cmp);
#ifdef TIME
        timespec_get(&tend, TIME_UTC);
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

static int checkon(const void *mem, const size_t left, const size_t right, const size_t size_elem, int (*cmp) (const void *, const void *)) {
    for (size_t i = left; i < right; i++) 
        if (cmp(mem + i * size_elem, mem + (i + 1) * size_elem) > 0) {
            printf("erri = %lu\n", i);
            printf("erri - left = %lu\n", i - left);
            return 0;
        }

    return 1;
}

static void timsort_merge(void * const mem, const size_t size_elem, const size_t left, const size_t mid, const size_t right, int (*cmp)(const void *, const void *)) {
    size_t len_copy = mid - left;
    void *mem_copy = malloc(len_copy * size_elem);
    size_t pos_copy = 0, pos_right = mid;
    size_t first_run_elem = 0;

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
        if ((pos_right > right) || ((pos_copy < len_copy) && (cmp(mem_copy + pos_copy * size_elem, mem + pos_right * size_elem) <= 0))) {
            first_run_elem = pos_copy;
#ifdef DUMP
            printf("firtst run elem = %lu\n", first_run_elem);
            printf("i = %lu\n", i);
#endif  
            if (pos_right > right)
                pos_copy = len_copy - 1;
            
            pos_copy++;
            while ((pos_copy + 1 < len_copy) && (cmp(mem_copy + pos_copy * size_elem, mem + pos_right * size_elem) <= 0))
                pos_copy++;
        
            memmove(mem + i * size_elem, mem_copy + first_run_elem * size_elem, (pos_copy - first_run_elem) * size_elem);
            i += pos_copy - first_run_elem - 1;

            assert(i >= first_run_elem);
            assert(i + 1 >= pos_copy);

#ifdef DUMP
    printf("step data: \n");
    for (int j = left; j <= i; j++) {
        int *ai = (int *) (mem + j * size_elem);
        printf("%d ", *ai);
    }

    printf("\n");

    assert(checkon(mem, left, i, size_elem, cmp));

    printf("pos_copy = %lu\n", pos_copy);
    printf("i = %lu\n\n", i);

#endif 
        } else {
            first_run_elem = pos_right;

#ifdef DUMP
            printf("firtst run elem = %lu\n", first_run_elem);
            printf("i = %lu\n", i);
#endif  
            if (pos_copy >= len_copy)
                pos_right = right;

            pos_right++;
            while ((pos_right < right) && (cmp(mem_copy + pos_copy * size_elem, mem + pos_right * size_elem) > 0))
                pos_right++;
            
            memmove(mem + i * size_elem, mem + first_run_elem * size_elem, (pos_right - first_run_elem) * size_elem);
            i += pos_right - first_run_elem - 1;

#ifdef DUMP
    printf("step data: \n");
    for (int j = left; j <= i; j++) {
        int *ai = (int *) (mem + j * size_elem);
        printf("%d ", *ai);
    }

    printf("\n");

    assert(checkon(mem, left, i, size_elem, cmp));

    printf("pos_copy = %lu\n", pos_copy);
    printf("i = %lu\n\n", i);

#endif 
        }
    }

#ifdef DUMP
    printf("output data: \n");
    for (int i = left; i < right; i++) {
        int *ai = (int *) (mem + i * size_elem);
        printf("%d ", *ai);
    }

    printf("\n\n");
#endif 

    //assert(checkon(mem, left, right, size_elem, cmp));
    
    free(mem_copy);
}

static void timsort_blance(void * const mem, const size_t size_elem, const size_t len, struct subarr_stack * const stk, int (*cmp)(const void *, const void *)) {
    int loop_on = 1;

    while ((stk->size > 2) && (loop_on)) {
        struct subarray subarr_x;
        struct subarray subarr_y;
        struct subarray subarr_z;

        pop_stack(stk, &subarr_x);
        pop_stack(stk, &subarr_y);
        pop_stack(stk, &subarr_z);

        if ((subarr_z.size < subarr_y.size + subarr_x.size) || (subarr_y.size < subarr_x.size)) {
            if ((subarr_x.size < subarr_z.size) && (subarr_y.size < subarr_z.size) && (subarr_z.size > subarr_y.size + subarr_x.size)) {
#ifdef TIME
        timespec_get(&tbeg, TIME_UTC);
#endif
                timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);
#ifdef TIME
        timespec_get(&tend, TIME_UTC);
        time_mer += diff(tbeg, tend);
#endif 
                subarr_y.size += subarr_x.size;

                push_stack(stk, &subarr_z);
                push_stack(stk, &subarr_y);

            } else {
#ifdef TIME 
        timespec_get(&tbeg, TIME_UTC);
#endif
                timsort_merge(mem, size_elem, subarr_z.beg, subarr_y.beg, subarr_y.beg + subarr_y.size - 1, cmp);
#ifdef TIME
        timespec_get(&tend, TIME_UTC);
        time_mer += diff(tbeg, tend);
#endif 
                subarr_z.size += subarr_y.size; 

                push_stack(stk, &subarr_z);
                push_stack(stk, &subarr_x);
            }

            //timsort_blance(mem, size_elem, len, stk, cmp);

        } else {
            push_stack(stk, &subarr_z);
            push_stack(stk, &subarr_y);
            push_stack(stk, &subarr_x);
            loop_on = 0;
        }
    } 
}

static void inssort(void * const arr, const size_t first, const size_t last, const size_t size_elem, int (*cmp)(const void *, const void *)) {
    size_t i = 0, pos = 0;
    void *key = malloc(size_elem);

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
