#include "readstrings.h"

static char *next_correct_elem(char* buf, size_t* cur_buf_elem, int (*is_cor_symb) (char));
static void write_one_str(const struct string str, const size_t len, FILE* stream);

size_t read_strings(struct string* * data, size_t* num_data_elem, char* * buf, const char* name_file_input, int (*is_cor_symb) (char)) {
    size_t num_buf_elem = read_strings_to_buf(buf, name_file_input);

    *num_data_elem = count_number_strings_in_buf(*buf, num_buf_elem, is_cor_symb);
    
    convert_buf_to_strings_array(data, *buf, *num_data_elem, num_buf_elem, is_cor_symb);

    return num_buf_elem;
}

size_t read_strings_to_buf(char* * buf, const char* name_file_input) {
    FILE *rfile = fopen(name_file_input, "r");

    if (!rfile) {
        fprintf(stderr, "Failure to open <%s> file in func <%s>: ", name_file_input, __func__);
        perror("fopen returned: ");
        return -1;
    }

    fseek(rfile, 0, SEEK_END);

    size_t file_size = (size_t) ftell(rfile);
    size_t num_char_read = 0;

    *buf = (char*) calloc(file_size + 1, sizeof(char));  

    if (!(*buf)) {
        fprintf(stderr, "Failed allocation at func <%s> for buf\n", __func__);
        return -1;
    }

    fseek(rfile, 0, SEEK_SET);

    num_char_read = fread(*buf, sizeof(char), file_size, rfile);

    fclose(rfile);

    if (num_char_read != file_size)
        fprintf(stderr, "In func <%s> (number of characters read = %lu) != (file size = %lu)\n", __func__, num_char_read, file_size);
     
    *(*buf + num_char_read + 1) = 0;

    return num_char_read;
}

int count_number_strings_in_buf(char* buf,  const size_t num_buf_elem, int (*is_cor_symb) (char)) {
    if (!buf) {
        fprintf(stderr, "NULL argument in func <%s>\n", __func__);
        return -1;
    }

    int result = 0;

    for (int i = 1; i <= num_buf_elem; i++) {
        if ((!is_cor_symb(buf[i])) && (is_cor_symb(buf[i - 1])))
            result++;
    }

    return result;
}

int convert_buf_to_strings_array(struct string* * data, char* buf, const size_t num_data_elem, const size_t num_buf_elem, int (*is_cor_symb) (char)) {
    if (!buf) {
        fprintf(stderr, "NULL argument in func <%s>\n", __func__);
        return -1;
    }

    *data = (struct string*) calloc(num_data_elem + 1, sizeof(struct string));  

    if (!(*data)) {
        fprintf(stderr, "Failed allocation at func <%s> for data\n", __func__);
        return -1;
    }

    size_t cur_buf_elem = 0, cur_data_elem = 0; 

    (*(*data + 0)).str = next_correct_elem(buf, &cur_buf_elem, is_cor_symb);

    char *last_sep = (*(*data + 0)).str;
    size_t len = 0;

    while (cur_buf_elem <= num_buf_elem) {
        if (!is_cor_symb(buf[cur_buf_elem])) {
            if (len > 0) {
                (*(*data + cur_data_elem)).str = last_sep;
                (*(*data + cur_data_elem)).len = len;
                cur_data_elem++;
            }

            last_sep = &buf[cur_buf_elem + 1];
            len = 0;

        } else {
            len++;
        }

        cur_buf_elem++;
    }

    (*(*data + cur_data_elem)).len = &buf[num_buf_elem - 1] - last_sep;

    return 0;
}

static char *next_correct_elem(char* const buf, size_t* cur_buf_elem, int (*is_cor_symb) (char)) {
    while (!is_cor_symb(buf[*cur_buf_elem]))
        (*cur_buf_elem)++;
    
    return (buf + *cur_buf_elem);
}

int write_strings(struct string* data, const size_t len, size_t num_data_elem, const char* name_file_output) {
    if (!data) {
        fprintf(stderr, "NULL arguments in func <%s>\n", __func__);
        return -1;
    }

    FILE *file_output = fopen(name_file_output, "w");

    if (!file_output) {
        fprintf(stderr, "Failure to open <%s> file in func <%s>: ", name_file_output, __func__);
        perror("fopen returned: ");
        return -1;
    }

    fprintf(file_output, "DATA: \n");
    fprintf(file_output, "str:");

    for (size_t i = 0; i < len + 3; i++)
        fprintf(file_output, "%c", ' ');

    fprintf(file_output, "len:\n");

    for (size_t i = 0; i < num_data_elem; i++) { 
        write_one_str(data[i], len, file_output);
        fprintf(file_output, "%10lu\n", data[i].len);
    }

    fclose(file_output);

    return 0;
}

static void write_one_str(const struct string str, const size_t len, FILE* stream) {
    size_t i = 0;

    for (; i < str.len; i++)
        fputc(str.str[i], stream);

    for (; i < len; i++) 
        fputc(' ', stream);
}

int str_cmp_strings(const struct string s1, const struct string s2) {
    if ((!s1.str) || (!s2.str)) {
        fprintf(stderr, "NULL argumnet in func <%s>\n", __func__);
        return 0;
    }

    size_t i = 0;

    while ((i < s1.len) && (i < s2.len)) {
        if (s1.str[i] != s2.str[i]) 
            return (s1.str[i] - s2.str[i]);

        i++;
    }
    
    return s1.str[i] - s2.str[i];
}

int str_cmp_string_chars(const struct string s1, const char* s2) {
    if ((!s1.str) || (!s2)) {
        fprintf(stderr, "NULL argumnet in func <%s>\n", __func__);
        return 0;
    }

    size_t i = 0;

    while ((i < s1.len) && (s2[i] != '\n') && (s2[i] != '\0')) {
        if (s1.str[i] != s2[i]) 
            return (s1.str[i] - s2[i]);

        i++;
    }

    return s1.str[i] - s2[i];
}

void clean_strings(struct string* * data, char* * buf) {
    free(*data);
    free(*buf);

    *data = NULL;
    *buf  = NULL;
}

int convert_str_to_int(const struct string s, int l, int r, int* a) {
    size_t i = r;
    int result = 0;
    int sen_deg = 1;

    for (; i > l; i--) {
        if (!(('0' <= s.str[i]) && (s.str[i] <= '9')))
            return -1;

        result += (s.str[i] - '0') * sen_deg;

        sen_deg *= 10;
    }

    if (s.str[i] == '-') {
        result *= (-1);

    } else {
        if (!(('0' <= s.str[i]) && (s.str[i] <= '9')))
            return -1;
        
        result += (s.str[i] - '0') * sen_deg;
    }

    *a = result;

    return 0;
}

int my_pow(int a, const int b) {
    if (b == 0) return 1;

    int result = a;

    for (int i = 1; i < b; i++)
        result *= a;
    
    return result;
}

void change_str_ending_buf(char* buf, int (*is_cor_symb) (char)) {
    if (!buf) {
        fprintf(stderr, "NULL arguments in func <%s>\n", __func__);
        return;
    }

    for (size_t i = 0; buf[i] != '\0'; i++)
        if (!is_cor_symb(buf[i]))
            buf[i] = '\0';
}

int is_symbol_words(char a) { //ASC2
    if ((a < '!') || (a == ' '))
        return 0;

    return 1;
}

int is_symbol_lines(char a) { //ASC2
    if ((a < '!') && (a != ' '))
        return 0;

    return 1;
}
