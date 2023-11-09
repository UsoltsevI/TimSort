#include "ReadStrings.h"

void read_strings(struct string * *data, size_t *num_data_elem, char * * buf, const char* name_file_input) {
    size_t num_buf_elem = read_strings_to_buf(buf, name_file_input);

    *num_data_elem = count_number_wrods_in_buf(*buf, num_buf_elem);
    
    convert_buf_to_strings_array(data, *buf, *num_data_elem, num_buf_elem);
}

size_t read_strings_to_buf(char * *buf, const char* name_file_input) {
    if (name_file_input == NULL) {
        printf("name_file_input == NULL\n");
        return -1;
    }

    FILE *ReadableFile = fopen(name_file_input, "r");

    fseek(ReadableFile, EOF, SEEK_END);

    size_t num_buf_elem = (size_t) ftell(ReadableFile) + 1;   

    *buf = (char *) calloc(num_buf_elem + 2, sizeof(char));  

    if (*buf == NULL) {
        printf("Failed reallocation at func %s for buf\n", __func__);
        return -1;
    }

    fseek(ReadableFile, 0, SEEK_SET);

    fread(*buf, sizeof(char), num_buf_elem, ReadableFile);

    fclose(ReadableFile);
     
    *(*buf + num_buf_elem) = '\n';
    *(*buf + num_buf_elem + 1) = 0;

    return num_buf_elem;
}

int count_number_wrods_in_buf(char *buf,  const size_t num_buf_elem) {
    int result = 0;

    for (int i = 0; i < num_buf_elem; i++) {
        if ((buf[i] == '\n') && (buf[i + 1] != '\n') && (buf[i + 1] != ' ') && (buf[i + 1] != '\0'))
            result++;
        
        if ((buf[i] == ' ') && (buf[i + 1] != '\n') && (buf[i + 1] != ' ') && (buf[i + 1] != '\0'))
            result++;

        if (buf[i] == '\0')
            return ++result;
    }

    return ++result;
}

int convert_buf_to_strings_array(struct string * *data, char * buf, const size_t num_data_elem, const size_t num_buf_elem) {
    *data = (struct string *) calloc(num_data_elem + 1, sizeof(struct string));  

    if (*data == NULL)
        return -1;

    (*(*data + 0)).str = (char *) buf;

    char *last_sep = (*(*data + 0)).str;
    size_t cur_buf_elem = 0, cur_data_elem = 0; 

    while (cur_buf_elem < num_buf_elem) {
        if ((buf[cur_buf_elem] == '\n') || (buf[cur_buf_elem] == ' ')) {
            (*(*data + cur_data_elem)).len = (unsigned long) (&buf[cur_buf_elem - 1] - last_sep);

            last_sep = &buf[cur_buf_elem];

            cur_data_elem++;
            (*(*data + cur_data_elem)).str = next_correct_elem(buf, &cur_buf_elem);
        }

        cur_buf_elem++;
    }

    (*(*data + cur_data_elem)).len = &buf[num_buf_elem - 1] - last_sep;

    return 0;
}

static char *next_correct_elem(char * const buf, size_t *cur_buf_elem) {
    while ((buf[*cur_buf_elem] == '\n') || (buf[*cur_buf_elem] == ' '))
        (*cur_buf_elem)++;
    
    return (buf + *cur_buf_elem);
}

void write_strings(struct string *data, const size_t num_data_elem, const char* name_file_output) {
    printf("Output process started...\n");

    FILE *file_output = fopen(name_file_output, "w");

    fprintf(file_output, "DATA: \n");
    fprintf(file_output, "str:              len:\n");

    for (size_t i = 0; i < num_data_elem; i++) { 
        write_one_str(data[i].str, 10, file_output);
        fprintf(file_output, "%10lu\n", data[i].len);
    }

    fclose(file_output);

    printf("Output process finished\n");
}

static void write_one_str(const char *str, const size_t len, FILE *stream) {
    size_t i = 0;
    for (; (str[i] != '\n') && (str[i] != ' ') && (str[i] != '\0'); i++)
        fputc(str[i], stream);

    for (; i < len; i++) 
        fputc(' ', stream);
}

int str_cmp_strings(const struct string s1, const struct string s2) {
    assert(s1.str != NULL);
    assert(s2.str != NULL);

    size_t i = 0;

    while ((i < s1.len) && (i < s2.len)) {
        if (s1.str[i] != s2.str[i]) 
            return (s1.str[i] - s2.str[i]);

        i++;
    }
    
    return s1.str[i] - s2.str[i];
}

int str_cmp_string_chars(const struct string s1, const char * s2) {
    assert(s1.str != NULL);
    assert(s2     != NULL);

    size_t i = 0;

    while ((i < s1.len) && (s2[i] != '\n') && (s2[i] != '\0')) {
        if (s1.str[i] != s2[i]) 
            return (s1.str[i] - s2[i]);

        i++;
    }

    return s1.str[i] - s2[i];
}

void clean_strings(struct string * *data, char * *buf) {
    free(*data);
    free(*buf);

    *data = NULL;
    *buf  = NULL;
}

int convert_str_to_int(const struct string s, int *a) {
    size_t i = 0;
    int result = 0;

    for (; i < s.len - 1; i++) {
        if (!(('0' <= s.str[i]) && (s.str[i] <= '9')))
            return -1;

        result += (s.str[i] - '0') * my_pow(10, s.len - i - 1);
    }

    if (s.str[i] == '-') {
        result *= (-1);

    } else {
        result += s.str[i] - '0';
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

void change_str_ending_buf(char *buf) {
    for (size_t i = 0; buf[i] != '\0'; i++)
        if ((buf[i] == '\n') || (buf[i] == ' ') || (buf[i] == '\r'))
            buf[i] = '\0';
}

