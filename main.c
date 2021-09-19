#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void find_in_file(const char *text, const struct dirent *directory_entry, const unsigned short int strict) {
    FILE *f;
    f = fopen(directory_entry->d_name, "r");

    if (f == NULL) {
        if (strict) printf("Couldn't open file %s, continuing...\n", directory_entry->d_name);

        return;
    }

    fclose(f);
}

void find_in_dir(const char *text, const char *dir, const char *file_ext, const unsigned short int strict) {
    DIR *dir_stream = opendir(dir);
    if (dir_stream == NULL) {
        if (strict) printf("Couldn't open directory %s, continuing...\n", dir);

        return;
    }

    struct dirent *directory_entry;
    while ((directory_entry = readdir(dir_stream)) != NULL) {
        if (strcmp(directory_entry->d_name, ".") == 0 || strcmp(directory_entry->d_name, "..") == 0) continue;
        if ((int) directory_entry->d_type == 4) {
            char subdir[256 + 1024 + 2];

            snprintf(subdir, sizeof(subdir), "%s/%s", dir, directory_entry->d_name);

            find_in_dir(text, subdir, file_ext, strict);

            continue;
        }

        if (strstr(directory_entry->d_name, file_ext) != NULL) {
            find_in_file(text, directory_entry, strict);
        }
    }

    closedir(dir_stream);
}

int starts_with(const char *str, const char *prefix) {
    return strncmp(prefix, str, strlen(prefix));
}

void show_help() {
    printf("Available commands:\n");
    printf("\tfind:\n");
    printf("\t\ttext        (t)     - Text you want to search for\n");
    printf("\t\tdir         (d)     - Text you want to search in\n");
    printf("\t\tfile-ext    (fe)    - File types you want to search in\n");
    printf("\t\tstrict      (s)     - Strict mode\n");
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        show_help();

        return EXIT_SUCCESS;
    }

    // ~4 kb for each
    char *text = malloc(sizeof(char) * 1024),
        *dir = malloc(sizeof(char) * 1024),
        *file_ext = malloc(sizeof(char) * 1024);
    unsigned short int strict = 0;

    if (text == NULL || dir == NULL || file_ext == NULL) {
        perror("Couldn't allocate memory");

        return EXIT_FAILURE;
    }

    // resetting the memory
    memset(text, 0, sizeof(char) * 1024);
    memset(dir, 0, sizeof(char) * 1024);
    memset(file_ext, 0, sizeof(char) * 1024);

    dir[0] = *".\0";

    for (int i = 0; i < argc; i++) {
        if (starts_with(argv[i], "-") != 0) continue;

        char *ptr = NULL;
        if (starts_with(argv[i], "-t") == 0 || starts_with(argv[i], "--text") == 0) ptr = text;
        else if (starts_with(argv[i], "-d") == 0 || starts_with(argv[i], "--dir") == 0) ptr = dir;
        else if (starts_with(argv[i], "-fe") == 0 || starts_with(argv[i], "--file-ext") == 0) ptr = file_ext;
        else if (starts_with(argv[i], "-s") == 0 || starts_with(argv[i], "--strict") == 0) {
            strict = 1;

            continue;
        }
        else {
            perror("Invalid flag, check for available command bellow\n");
            show_help();

            return EXIT_FAILURE;
        }

        for (int j = 0; j < strlen(argv[i]); j++) {
            if (strncmp(&argv[i][j], "=", 1) == 0) {
                strcpy(ptr, &argv[i][j + 1]);

                break;
            }
        }
    }

    find_in_dir(text, dir, file_ext, strict);

    free(text);
    free(dir);
    free(file_ext);

    return EXIT_SUCCESS;
}
