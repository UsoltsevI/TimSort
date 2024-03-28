#ifndef TIMSORT_H_INCLUDED
#define TIMSORT_H_INCLUDED

#include <iostream>
#include <stack>
#include <cstring>

// #include <cassert>

template <typename T, typename Compare>
class timsortcls {
    struct subarray {
        size_t beg;
        size_t size;
    };

    static const size_t MIN_LEN_TO_MERGESORT = 64;

    enum {
        IS_INCREASES = 1,
        IS_DECREASES = -1,
        NOT_SORTED   = 0,
        UNDEF_MONOTON = 2,
    };

    static size_t moveright(T* arr, T key, size_t first, size_t last, Compare cmp) {
        for (size_t i = first; i < last; ++i) {
            if (!cmp(key, arr[i])) {
                std::copy(&arr[i], &arr[last], &arr[i + 1]);

                return i;
            }
        }

        return last;
    }

    static void inssort(T* arr, size_t first, size_t last, Compare cmp) {
        for (size_t i = first; i < last; ++i) { 
            T key = arr[i];
            size_t pos = moveright(arr, key, first, i, cmp);
            arr[pos] = key;
        }
    }

    static void timsort_merge(T* begin, size_t left, size_t mid, size_t right, Compare cmp) {
        //
        // --right;
        //
        size_t len_copy = mid - left;
        size_t pos_copy = 0, pos_right = mid;
        size_t first_run_elem = 0;
        bool cmp_res;

        //
        // --right;
        //

        T* arr_copy = new T[len_copy];

        std::copy(&begin[left], &begin[len_copy], arr_copy);

        cmp_res = cmp(arr_copy[pos_copy], begin[pos_right]);

        for (size_t i = left; i < right + 1; ++i) {
            if ((pos_right > right) || ((pos_copy < len_copy) && (!cmp_res))) {
                first_run_elem = pos_copy;

                if (pos_right > right) {
                    pos_copy = len_copy - 1;
                }

                ++pos_copy;

                while ((pos_copy < len_copy) && ((cmp_res = cmp(arr_copy[pos_copy], begin[pos_right])) == false)) {
                    ++pos_copy;
                }
            
                std::copy(&arr_copy[first_run_elem], &arr_copy[pos_copy], &begin[i]);
                i += pos_copy - first_run_elem - 1;

            } else {
                first_run_elem = pos_right;
    
                if (pos_copy >= len_copy) {
                    pos_right = right;
                }

                ++pos_right;

                while ((pos_right <= right) && (cmp_res = cmp(arr_copy[pos_copy], begin[pos_right]))) {
                    ++pos_right;
                }

                std::copy(&begin[first_run_elem], &begin[pos_right], &begin[i]);
                i += pos_right - first_run_elem - 1;
            }

            // cmp_res = cmp(copy[pos_copy], begin[pos_right]);
        }

        delete[] arr_copy;
    }

    static void timsort_blance(T* begin, const size_t len, std::stack<subarray>& stk, Compare cmp) {
        bool loop_on = true;

        // std::cout << "balance\n";

        while ((stk.size() > 2) && loop_on) {
            subarray subarr_x;
            subarray subarr_y;
            subarray subarr_z;

            subarr_x = stk.top(); stk.pop();
            subarr_y = stk.top(); stk.pop();
            subarr_z = stk.top(); stk.pop();

            if ((subarr_z.size < subarr_y.size + subarr_x.size) || (subarr_y.size < subarr_x.size)) {
                if ((subarr_x.size < subarr_z.size) && (subarr_y.size < subarr_z.size) && (subarr_z.size > subarr_y.size + subarr_x.size)) {
                    timsort_merge(begin, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);

                    subarr_y.size += subarr_x.size;

                    stk.push(subarr_z);
                    stk.push(subarr_y);

                } else {
                    timsort_merge(begin, subarr_z.beg, subarr_y.beg, subarr_y.beg + subarr_y.size - 1, cmp);
                    subarr_z.size += subarr_y.size; 

                    stk.push(subarr_z);
                    stk.push(subarr_x);
                }

            } else {
                stk.push(subarr_z);
                stk.push(subarr_y);
                stk.push(subarr_x);

                loop_on = false;
            }
        } 

        //
        // std::cout << "after balance\n";
        // for (size_t i = 0; i < len; ++i) {
        //     std::cout << begin[i] << ' ';
        // }
        // std::cout << std::endl;
        //
    }

    static void reverse(T* begin, size_t first, size_t last) {
        for (size_t i = 0; i <= (last - first) / 2; ++i) {
            T c = begin[i + first];
            begin[i + first] = begin[last - i];
            begin[last - i] = c;
        }
    }

    static size_t get_minrun(size_t n) {
        size_t r = 0;

        while (n >= MIN_LEN_TO_MERGESORT) {
            r |= (n & 1u);
            n >>= 1u;
        }

        return r + n;
    }

    static void timsort_imp(T* begin, size_t len, Compare cmp) {
        size_t minrun = get_minrun(len);
        size_t add_step = 0, cur_pos = 0;
        int is_sorted = UNDEF_MONOTON;

        std::stack<subarray> stk;
        subarray subarr_x; 
        subarray subarr_y; 

        while (cur_pos < len) {
            add_step = cur_pos;
            is_sorted = UNDEF_MONOTON;

            while ((add_step < len) && ((add_step > len - minrun) || (add_step - cur_pos < minrun) || (is_sorted != NOT_SORTED))) {
                if ((is_sorted != NOT_SORTED) && (add_step + 1 < len)) {
                    // bool check_cmp = cmp(begin[add_step], begin[add_step + 1]);
                    // bool check_cmp_res = cmp(begin[add_step + 1], begin[add_step]);

                    // if (is_sorted == UNDEF_MONOTON) {
                    //     if (check_cmp_res) {
                    //         is_sorted = IS_INCREASES;

                    //     } else if (check_cmp) {
                    //         is_sorted = IS_DECREASES;
                    //     } 

                    // } else if ((is_sorted == IS_INCREASES) && (check_cmp)) {
                    //     is_sorted = NOT_SORTED;
                        
                    // } else if ((is_sorted == IS_DECREASES) && (check_cmp_res)) {
                    //     is_sorted = NOT_SORTED;
                    // }
                    is_sorted = NOT_SORTED;
                }
                
                ++add_step;
            }

            if (is_sorted == IS_DECREASES) {
                reverse(begin, cur_pos, add_step - 1);
            }

            if (is_sorted == NOT_SORTED) {
                inssort(&begin[cur_pos], 0, add_step - cur_pos, cmp);
            }

            subarr_x.beg = cur_pos;
            subarr_x.size = add_step - cur_pos;
            cur_pos = add_step; 

            stk.push(subarr_x);

            timsort_blance(begin, len, stk, cmp);
        }

        while (stk.size() > 1) { 
            subarr_x = stk.top(); stk.pop();
            subarr_y = stk.top(); stk.pop();

            // std::cout << subarr_x.beg << ' ' << subarr_x.size << std::endl;
            // std::cout << subarr_y.beg << ' ' << subarr_y.size << std::endl;

            timsort_merge(begin, subarr_y.beg, subarr_x.beg, subarr_x.beg + subarr_x.size - 1, cmp);

            subarr_y.size += subarr_x.size;

            stk.push(subarr_y);

            //
            // std::cout << "after merge p\n";
            // for (size_t i = 0; i < len; ++i) {
            //     std::cout << begin[i] << ' ';
            // }
            // std::cout << std::endl;
            //
        } 
    }   
        
public:
    static void timsort(T* begin, size_t len, Compare cmp) {
        if (len <= MIN_LEN_TO_MERGESORT) {
            inssort(begin, 0, len, cmp);
            return;
        } 

        timsort_imp(begin, len, cmp);
    }
};

#endif // TIMSORT_H_INCLUDED