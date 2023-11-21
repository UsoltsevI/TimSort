#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void generate_sets(FILE* file_gen, const int num_elem_in_set, const int num_stes, const int filed_num);

int main(int argc, char* * argv) {
    if (argc != 5) {
        printf("Usage: %s <num_elem_in_set> <num_sets> <field_num> <name_file>\n", argv[0]);
        return 1;
    }

    char* endptr;

    int num_sets = strtol(argv[1], &endptr, 10);

    if (endptr == argv[1]) {
        printf("<%s> cannot be converted to int\n", argv[1]);
        return 1;
    }

    int num_elem_in_set = strtol(argv[2], &endptr, 10);

    if (endptr == argv[2]) {
        printf("<%s> cannot be converted to int\n", argv[2]);
        return 1;
    }

    int field_num = strtol(argv[3], &endptr, 10);

    if (endptr == argv[3]) {
        printf("<%s> cannot be converted to int\n", argv[3]);
        return 1;
    }

    FILE* file_gen = fopen(argv[4], "w");

    if (file_gen == NULL) {
        perror("ERROR: ");
        return 1;
    }

    generate_sets(file_gen, num_elem_in_set, num_sets, field_num);

    fclose(file_gen);

    return 0;
}

void generate_sets(FILE* file_gen, const int num_elem_in_set, const int num_stes, const int field_num) {
    fprintf(file_gen, "%d %d %d\n", num_stes, num_elem_in_set, field_num);

    for (int i = 0; i < num_stes; i++) {
        for (int j = 0; j < num_elem_in_set; j++)
            fprintf(file_gen, "%d ",(int) (random() % 10));

        fprintf(file_gen, "\n");
    }
}
