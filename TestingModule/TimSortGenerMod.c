#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static void generate_data(FILE* file_gen, const int num_elem, const int mod, const int max_num);

int main(int argc, char* * argv) {
    if (argc != 5) {
        printf("Usage: %s <num_elem> <mod> <max_num> <name_file>\n", argv[0]);
        return 1;
    }

    char* endptr;

    int num_elem = strtol(argv[1], &endptr, 10);

    if (endptr == argv[1]) {
        printf("<%s> cannot be converted to int\n", argv[1]);
        return 1;
    }

    int mod = strtol(argv[2], &endptr, 10);

    if (endptr == argv[2]) {
        printf("<%s> cannot be converted to int\n", argv[2]);
        return 1;
    }

    int max_num = strtol(argv[3], &endptr, 10);

    if (endptr == argv[3]) {
        printf("<%s> cannot be converted to int\n", argv[3]);
        return 1;
    }

    FILE* file_gen = fopen(argv[4], "w");

    if (file_gen == NULL) {
        perror("ERROR: ");
        return 1;
    }

    generate_data(file_gen, num_elem, mod, max_num);

    fclose(file_gen);

    return 0;
}

static void generate_data(FILE* file_gen, const int num_elem, const int mod, const int max_num) {
    fprintf(file_gen, "%d\n", num_elem);

    for (int i = 0; i < num_elem; i++)
        fprintf(file_gen, "%d ",(int) (random() % max_num));

    fprintf(file_gen, "\n%d\n", mod);
}
