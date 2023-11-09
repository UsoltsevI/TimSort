#ifndef ONEGREAD_H_INCLUDED
#define ONEGREAD_H_INCLUDED

#include <stdio.h>
#include <malloc.h>
#include <assert.h>

struct string {
    char *str;
    size_t len;
};

void read_strings(struct string * *data, size_t *num_data_elem, char * * buf, const char* name_file_input);

size_t read_strings_to_buf(char * *buf, const char* name_file_input);

int count_number_wrods_in_buf(char *buf,  const size_t num_buf_elem);

int convert_buf_to_strings_array(struct string * *data, char * buf, const size_t num_data_elem, const size_t num_buf_elem);

static char *next_correct_elem(char *buf, size_t *cur_buf_elem);

void write_strings(struct string *data, const size_t num_data_elem, const char* name_file_output);

static void write_one_str(const char *str, const size_t len, FILE *stream);

int str_cmp_strings(const struct string s1, const struct string s2);

int str_cmp_string_chars(const struct string s1, const char * s2);

void clean_strings(struct string * *data, char * *buf);

int my_pow(int a, const int b);

int convert_str_to_int(const struct string s, int *a);

void change_str_ending_buf(char *buf); 

#endif // ONEGREAD_H_INCLUDED
