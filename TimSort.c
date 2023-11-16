#include "TimSort.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#define min(a, b) (a < b ? a : b)
#define RUNON
//#define REALLOCSTACK //too slow
#define DEBUGONTS

static const int MIN_LEN_TO_TIMSORT = 64;
static const int RUN_THRESHHOLD_VAL = 7;

static void timsort_imp(void* const mem, const int len, const int size_elem, int (*cmp)(void*, void*));
static void timsort_merge(void* const mem, const int size_elem, const int left, const int mid, const int right, int (*cmp)(void*, void*));
static void timsort_run(void* mem, void* mem_first, void* mem_second, int* pos_first, int* pos_second,
                    int* i, const int last_accept_pos_first, const int last_accept_pos_second, const int size_elem, int (*cmp)(void*, void*));

static void inssort(void* const arr, const int first, const int last, const int size_elem, int (*cmp)(void*, void*));
static int moveright(void* const arr, void* const key, const int last, const int size_elem, int (*cmp)(void*, void*));
static int get_minrun(int n);

struct subarray {
    int beg;
    int size;
};

struct subarr_stack {
    struct subarray* data;
    int capacity;
    int size;
};


static void new_stack(struct subarr_stack* const stk, const int beg_capacity);
static void push_stack(struct subarr_stack* const stk, struct subarray* const new_elem);
static void pop_stack(struct subarr_stack* const stk, struct subarray* const out);
static void delete_stack(struct subarr_stack* const stk);

#if 0
static void realloc_stack(struct subarr_stack* stk, int new_capacity);
#endif 

#if 0
void timsort(char * *mem, int N, int K, int X) {
    timsort_imp(mem, N, K, X, &compare_by_x_field, 0, N - 1);
}
#endif

void timsort(void* const mem, const int len, const int size_elem, int (*cmp)(void*, void*)) {
    if (len <= MIN_LEN_TO_TIMSORT) {
        inssort(mem, 0, len, size_elem, cmp);
        return;
    } 

    timsort_imp(mem, len, size_elem, cmp);
}

static void timsort_imp(void* const mem, const int len, const int size_elem, int (*cmp)(void*, void*)) {
    int minrun = get_minrun(len);
    //printf("minrun = %d\n", minrun);
    printf("len = %d\n", len);

    int add_step = 0;
    int cur_pos = 0;
    int cur_checking_len = 0;
    int check = 0;

    struct subarr_stack stk = {};
    struct subarray subarr_x = {}; //the last one
    struct subarray subarr_y = {}; //the last but one
    struct subarray subarr_z = {}; //the last but two

    new_stack(&stk, len / minrun + 1);

    while (cur_pos < len) {
        add_step = cur_pos;
        cur_checking_len = 0;
        
        while ((add_step < len - 1) && //we need to subtract 1 because then we add 1
                (((check = cmp(mem + add_step * size_elem, mem + (add_step + 1) * size_elem)) < 0) || 
                (add_step - cur_pos < minrun))) {
        
            if (check < 0)
                cur_checking_len++;
    
            add_step++;
        }

        if (add_step == len - 1)
            add_step++;

        assert(add_step <= len);
        assert(cur_checking_len <= add_step - cur_pos);

        if (cur_checking_len < add_step - cur_pos) 
            inssort(mem, cur_pos, add_step, size_elem, cmp);

        printf("cur_pos = %d, add_step = %d\n", cur_pos, add_step);

        subarr_x.beg = cur_pos;
        subarr_x.size = add_step - cur_pos;
        cur_pos = add_step;

        if (stk.size > 2) { 
            pop_stack(&stk, &subarr_y); //y and z are the last elements in the stack
            pop_stack(&stk, &subarr_z); //x hasn't been added yet

            //this is the balance condition
            if ((subarr_z.size < subarr_y.size + subarr_x.size) || (subarr_y.size < subarr_x.size)) {
                printf("subarr_z.beg = %d, subarr_z.size = %d, subarr_y.beg = %d\n", subarr_z.beg, subarr_z.size, subarr_y.beg);
                assert(subarr_z.beg + subarr_z.size == subarr_y.beg); //it has to be by default
                assert(subarr_y.beg + subarr_y.size == subarr_x.beg);
                assert(subarr_x.beg + subarr_x.size <= len);

                if (subarr_x.size < subarr_z.size) {
                    //merge y and x arrays

                    timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);

                    subarr_y.size += subarr_x.size; //after merge x and y

                    push_stack(&stk, &subarr_z);
                    push_stack(&stk, &subarr_y);

                } else {
                    //merge y and z arrays

                    timsort_merge(mem, size_elem, subarr_z.beg, subarr_y.beg, subarr_y.beg + subarr_y.size - 1, cmp);

                    subarr_z.size += subarr_y.size;

                    push_stack(&stk, &subarr_z);
                    push_stack(&stk, &subarr_x);
                    
                }

            } else {
                push_stack(&stk, &subarr_x);
            }

        } else {
            push_stack(&stk, &subarr_x);
        }
    }

    while (stk.size > 1) { //add up all the remaining
        printf("here?\n");
        pop_stack(&stk, &subarr_x);
        pop_stack(&stk, &subarr_y);

        printf("subarr_c.beg = %d, subarr_x.size = %d\n", subarr_x.beg, subarr_x.size);
        assert(subarr_x.beg + subarr_x.size == len);

        timsort_merge(mem, size_elem, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);
    } 

    delete_stack(&stk);

    //that's it! The array has already sorted
}   

static void timsort_merge(void* const mem, const int size_elem, const int left, const int mid, const int right, int (*cmp)(void*, void*)) {
    assert(size_elem > 0);

#ifdef DEBUGONTS

    printf("merge: left = %d, mid = %d, right = %d\n", left, mid, right);
    printf("input left: ");

    for (int i = left; i < mid + 1; i++) {
        int* vali = (int*) (mem + i * size_elem);
        printf("%d ", *vali);
    }

    printf("\nright: ");

    for (int i = mid + 1; i < right + 1; i++) {
        int* vali = (int*) (mem + i * size_elem);
        printf("%d ", *vali);
    }
    
    printf("\n");

#endif 

    //in this realization I don't care about choosing the min of lest and right
    //we always choose the left one)
    void* mem_copy = calloc(mid - left + 1, size_elem);
    int pos_copy = 0;
    int pos_right = mid + 1;
    int run_detector = 0, last_choise = 0;

    memcpy(mem_copy, mem + left * size_elem, (mid - left + 1) * size_elem);

    for (int i = left; i < right + 1; i++) {
        //printf("pos_right = %d, right = %d, pos_copy = %d\n", pos_right, right, pos_copy);
        assert(pos_right <= right + 1);
        assert(pos_copy <= mid - left + 1);

        if ((pos_right > right) || ((pos_copy < mid - left + 1) && (cmp(mem_copy + pos_copy * size_elem, mem + pos_right * size_elem) < 0))) {
            //form left arr
            assert(pos_copy < mid - left + 1);

            memmove(mem + i * size_elem, mem_copy + pos_copy * size_elem, size_elem);

            pos_copy++;

#ifdef RUNON
            if (last_choise == 1) {
                last_choise = 0;
                run_detector = 0;

            } else {
                run_detector++;
                
                if (run_detector > RUN_THRESHHOLD_VAL)
                    timsort_run(mem, mem_copy, mem, &pos_copy, &pos_right, &i, mid - left + 1, right, size_elem, cmp);
            }
#endif  

        } else {
            //from right arr
            assert(pos_right < right + 1);

            memmove(mem + i * size_elem, mem + pos_right * size_elem, size_elem);

            pos_right++;

#ifdef RUNON
            if (last_choise == 0) {
                last_choise = 1;
                run_detector = 0;

            } else {
                run_detector++;

                if (RUN_THRESHHOLD_VAL)
                    timsort_run(mem, mem, mem_copy, &pos_right, &pos_copy, &i, right + 1, mid - left + 1, size_elem, cmp);
            }
#endif 
        }
    }
    
    free(mem_copy);
}

static void inssort(void* const arr, const int first, const int last, const int size_elem, int (*cmp)(void*, void*)) {
    int i = 0, pos = 0;
    void* key = calloc(1, size_elem);

    assert(first < last);

    for (i = first; i < last; i++) {        
        memcpy(key, arr + i * size_elem, size_elem);
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
                //arr[j] = arr[j - 1];

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

static void timsort_run(void* mem, void* mem_first, void* mem_second, int* pos_first, int* pos_second,
                    int* i, const int last_accept_pos_first, const int last_accept_pos_second, const int size_elem, int (*cmp) (void*, void*)) {

    int first_run_elem = *pos_first;

    if (*pos_second > last_accept_pos_second) //a special accelerating case
        *pos_first = last_accept_pos_first;

    while ((*pos_first < last_accept_pos_first) && (cmp(mem_first + (*pos_first) * size_elem, mem_second + (*pos_second) * size_elem) <= 0))
        (*pos_first)++;

    (*pos_first)--;
    
    printf("first_run_elem = %d, pos_first = %d\n", first_run_elem, *pos_first);

    if (*pos_first > first_run_elem) {
        (*i)++;

        memmove(mem + (*i) * size_elem, mem_first + first_run_elem * size_elem, (*pos_first - first_run_elem) * size_elem);

        *i += *pos_first - first_run_elem;
    }
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

#if REALLOCSTACK
    if (stk->size + 1 >= stk->capacity)
        realloc_stack(stk, (stk->capacity + 1) * 2);
#endif 

    stk->data[stk->size].beg = new_elem->beg;
    stk->data[stk->size].size = new_elem->size;

    stk->size++;
}

static void pop_stack(struct subarr_stack* const stk, struct subarray* const out) {
    assert(stk);
    assert(stk->size > 0);

#if REALLOCSTACK

    if (stk->size < (stk->capacity / 2))
        realloc_stack(stk, (stk->capacity / 2) + 1);

#endif 

    stk->size--;

    out->beg = stk->data[stk->size].beg;
    out->size = stk->data[stk->size].size;
}

static void delete_stack(struct subarr_stack* const stk) {
    free(stk->data);
}

#ifdef REALLOCSTACK

static void realloc_stack(struct subarr_stack* stk, int new_capacity) {
    assert(stk);
    assert(new_capacity >= 0);

    stk->data = realloc(stk->data, new_capacity * stk->size_elem);
    stk->capacity = new_capacity;

    assert(stk->data);
}

#endif