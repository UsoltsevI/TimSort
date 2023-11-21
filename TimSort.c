#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "TimSort.h"

//#define RUNOFF
//#define DEBUGON

static const int MIN_LEN_TO_TIMSORT = 64;
static const int RUN_THRESHHOLD_VAL = 7;

struct subarray {
    int beg;
    int size;
};

struct subarr_stack {
    struct subarray* data;
    int capacity;
    int size;
};

static void timsort_imp(void* const mem, const int len, const int size_elem, int (*cmp)(void*, void*));
static void timsort_merge(void* const mem, const int size_elem, const int left, const int mid, const int right, int (*cmp)(void*, void*));
static void timsort_blance(void * const mem, const int size_elem, const int len, struct subarr_stack* const stk, int (*cmp)(void*, void*));
static void timsort_gallop(void* mem, void* mem_first, void* mem_second, int* pos_first, int* pos_second,
                    int* i, const int last_accept_pos_first, const int last_accept_pos_second, const int size_elem, int (*cmp)(void*, void*));

static void inssort(void* const arr, const int first, const int last, const int size_elem, int (*cmp)(void*, void*));
static int moveright(void* const arr, void* const key, const int last, const int size_elem, int (*cmp)(void*, void*));
static int get_minrun(int n);

static void new_stack(struct subarr_stack* const stk, const int beg_capacity);
static void push_stack(struct subarr_stack* const stk, struct subarray* const new_elem);
static void pop_stack(struct subarr_stack* const stk, struct subarray* const out);

void timsort(void* const mem, const int len, const int size_elem, int (*cmp)(void*, void*)) {
    if (len <= MIN_LEN_TO_TIMSORT) {
        inssort(mem, 0, len, size_elem, cmp);

        return;
    } 

    timsort_imp(mem, len, size_elem, cmp);
}

static void timsort_imp(void* const mem, const int len, const int size_elem, int (*cmp)(void*, void*)) {
    int minrun = get_minrun(len);
    int add_step = 0;
    int cur_pos = 0;
    //int cur_checking_len = 0;
    int is_sorted = 0;

    struct subarr_stack stk = {};
    //the last three structures in the stack;
    struct subarray subarr_x = {}; 
    struct subarray subarr_y = {}; 
    struct subarray subarr_z = {}; 

    //we have no need more than (len/minrun + 1) elements in stack
    //and reallocations in stack is too slow
    //that's why we ones callocate stack for the maximum 
    //possible amount
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

        assert(add_step <= len);

        if (!is_sorted)
            inssort(mem, cur_pos, add_step, size_elem, cmp);

        subarr_x.beg = cur_pos;
        subarr_x.size = add_step - cur_pos;
        cur_pos = add_step;

        push_stack(&stk, &subarr_x);

        timsort_blance(mem, size_elem, len, &stk, cmp);
    }

#ifdef DEBUGON
    printf("\nlen = %d\n", len);
    printf("minrun = %d\n", minrun);
    printf("stack:\n");

    for (int i = 0; i < stk.size; i++) 
        printf("%d ", stk.data[i].size);
    
    printf("\n");
#endif 

    while (stk.size > 1) { //add up all the remaining
        pop_stack(&stk, &subarr_x);
        pop_stack(&stk, &subarr_y);

        assert(subarr_x.beg + subarr_x.size == len);

        timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);

        subarr_y.size += subarr_x.size;

        push_stack(&stk, &subarr_y);
    } 

    free(stk.data);

    //that's it! The array has already sorted
}   

static void timsort_merge(void* const mem, const int size_elem, const int left, const int mid, const int right, int (*cmp)(void*, void*)) {
    assert(size_elem > 0);

    //in this realization I don't care about choosing the min of left and right
    //we always choose the left one)
    void* mem_copy = calloc(mid - left + 1, size_elem);
    int pos_copy = 0;
    int pos_right = mid + 1;
    int run_detector = 0, last_choise = 0;

    memcpy(mem_copy, mem + left * size_elem, (mid - left + 1) * size_elem);

    for (int i = left; i < right + 1; i++) {
        assert(pos_right <= right + 1);
        assert(pos_copy <= mid - left + 1);

        if ((pos_right > right) || ((pos_copy < mid - left + 1) && (cmp(mem_copy + pos_copy * size_elem, mem + pos_right * size_elem) <= 0))) {
            //form left (copied) arr
            assert(pos_copy < mid - left + 1);

            memmove(mem + i * size_elem, mem_copy + pos_copy * size_elem, size_elem);

#ifndef RUNOFF
            if (last_choise == 1) {
                last_choise = 0;
                run_detector = 0;

            } else {
                run_detector++;
                
                if (run_detector > RUN_THRESHHOLD_VAL) {
                    timsort_gallop(mem, mem_copy, mem, &pos_copy, &pos_right, &i, mid - left, right, size_elem, cmp);

                    last_choise = 1; //timsort_gallop works as long as last_choise == 0
                }
            }
#endif  
            pos_copy++;

        } else {
            //from right arr
            assert(pos_right < right + 1);

            memmove(mem + i * size_elem, mem + pos_right * size_elem, size_elem);

#ifndef RUNOFF
            if (last_choise == 0) {
                last_choise = 1;
                run_detector = 0;

            } else {
                run_detector++;

                if (run_detector > RUN_THRESHHOLD_VAL) {
                    timsort_gallop(mem, mem, mem_copy, &pos_right, &pos_copy, &i, right, mid - left, size_elem, cmp);

                    last_choise = 0; //timsort_gallop works as long as last_choise == 1
                }
            }
#endif
            pos_right++;

        }
    }
    
    free(mem_copy);
}

static void timsort_blance(void* const mem, const int size_elem, const int len, struct subarr_stack* const stk, int (*cmp)(void*, void*)) {
    if (stk->size > 2) { 
        struct subarray subarr_x;
        struct subarray subarr_y;
        struct subarray subarr_z;

        pop_stack(stk, &subarr_x);
        pop_stack(stk, &subarr_y); //y and z are the last elements in the stack
        pop_stack(stk, &subarr_z); //x hasn't been added yet

        if ((subarr_z.size < subarr_y.size + subarr_x.size) || (subarr_y.size < subarr_x.size)) { //this is the balance condition
            assert(subarr_z.beg + subarr_z.size == subarr_y.beg); //it has to be by default
            assert(subarr_y.beg + subarr_y.size == subarr_x.beg);
            assert(subarr_x.beg + subarr_x.size <= len);

            if ((subarr_x.size < subarr_z.size) && (subarr_y.size < subarr_z.size) && (subarr_z.size > subarr_y.size + subarr_x.size)){ //merge y and x arrays
                timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size, cmp);

                subarr_y.size += subarr_x.size; //after merge x and y we have to change structures too

                push_stack(stk, &subarr_z);
                push_stack(stk, &subarr_y);

            } else { //merge y and z arrays
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

static void timsort_gallop(void* mem, void* mem_first, void* mem_second, int* pos_first, int* pos_second,
                    int* i, const int last_accept_pos_first, const int last_accept_pos_second, const int size_elem, int (*cmp) (void*, void*)) {

    int first_run_elem = *pos_first + 1;

    if (*pos_second > last_accept_pos_second) //a special accelerating case
        *pos_first = last_accept_pos_first;

    while ((*pos_first  + 1 < last_accept_pos_first) && (cmp(mem_first + (*pos_first + 1) * size_elem, mem_second + (*pos_second) * size_elem) <= 0))
        (*pos_first)++;

    if (*pos_first > first_run_elem) {
        (*i)++;

        memmove(mem + (*i) * size_elem, mem_first + first_run_elem * size_elem, (*pos_first - first_run_elem + 1) * size_elem);

        *i += *pos_first - first_run_elem;
    }
}

static void inssort(void* const arr, const int first, const int last, const int size_elem, int (*cmp)(void*, void*)) {
    int i = 0, pos = 0;
    void* key = calloc(1, size_elem);

    assert(first < last);

    for (i = first; i < last; i++) { 
        memmove(key, arr + i * size_elem, size_elem);

        pos = moveright(arr, key, i, size_elem, cmp);

        memcpy(arr + pos * size_elem, key, size_elem);
    }

    free(key);
}

static int moveright(void* const arr, void* const key, const int last, const int size_elem, int (*cmp)(void*, void*)) {
    for (int i = 0; i < last; i++) {
        if (cmp(key, arr + i * size_elem) < 0) {
            for (int j = last; j > i; j--)
                memcpy(arr + j * size_elem, arr + (j - 1) * size_elem, size_elem);

            return i;
        }
    }
    
    return last;
}

static int get_minrun(int n) { //copied from wikipedia
    int r = 0;

    while (n >= 64) {
        r |= (n & 1u);
        n >>= 1u;
    }

    return r + n;
}

static void new_stack(struct subarr_stack* const stk, const int beg_capacity) {
    assert(stk);
    assert(beg_capacity >= 0);

    stk->capacity = beg_capacity;
    stk->size = 0;
    stk->data = (struct subarray*) calloc(beg_capacity, sizeof(struct subarray));

    assert(stk->data);
}

static void push_stack(struct subarr_stack* const stk, struct subarray* const new_elem) {
    assert(stk);

    stk->data[stk->size].beg = new_elem->beg;
    stk->data[stk->size].size = new_elem->size;

    stk->size++;
}

static void pop_stack(struct subarr_stack* const stk, struct subarray* const out) {
    assert(stk);
    assert(stk->size > 0);

    stk->size--;

    out->beg = stk->data[stk->size].beg;
    out->size = stk->data[stk->size].size;
}
