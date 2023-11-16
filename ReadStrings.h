#ifndef ONEGREAD_H_INCLUDED
#define ONEGREAD_H_INCLUDED

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
//#include <string.h>

struct string {
    char *str;
    size_t len;
};

void read_strings(struct string* * data, size_t* num_data_elem, char* * buf, const char* name_file_input, int (*is_cor_symb) (char));

size_t read_strings_to_buf(char* * buf, const char* name_file_input);

int count_number_strings_in_buf(char* buf,  const size_t num_buf_elem, int (*is_cor_symb) (char));

int convert_buf_to_strings_array(struct string* * data, char* buf, const size_t num_data_elem, const size_t num_buf_elem, int (*is_cor_symb) (char));

static char *next_correct_elem(char* buf, size_t* cur_buf_elem, int (*is_cor_symb) (char));

void write_strings(struct string* data, const size_t num_data_elem, const char* name_file_output);

static void write_one_str(const struct string str, const size_t len, FILE* stream);

int str_cmp_strings(const struct string s1, const struct string s2);

int str_cmp_string_chars(const struct string s1, const char* s2);

void clean_strings(struct string* * data, char* * buf);

int convert_str_to_int(const struct string s, int l, int r, int* a);

int my_pow(int a, const int b);

void change_str_ending_buf(char* buf, int (*is_cor_symb) (char));

int is_symbol_words(char a);

int is_symbol_lines(char a);

#endif // ONEGREAD_H_INCLUDED