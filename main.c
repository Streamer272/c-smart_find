#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int ends_with(const char *str, const char *prefix);

void find_in_file(const char *text, const char *filename, const unsigned short int log) {
    if (log) printf("Opening file \"%s\"\n", filename);

    FILE *f;
    f = fopen(filename, "r");

    if (f == NULL) {
        if (log) printf("Couldn't open file %s\n", filename);

        return;
    }

    int c, count = 0, line = 1, found_count = 0;
    while ((c = getc(f)) != EOF) {
        if (c == '\n') {
            line++;
        }

        if (count >= strlen(text)) {
            if (log) printf("\t");
            printf("Found \"%s\" on line %d in file \"%s\"\n", text, line, filename);

            found_count++;
            count = 0;
        }
        else if (c == (int) text[count]) {
            count++;
        }
        else if (count != 0 && c != (int) text[count]) {
            count = 0;
        }
    }

    if (log) printf("Found %d occurrences in %s\n", found_count, filename);

    fclose(f);
}

void find_in_dir(const char *text, const char *dir, const char *file_ext, const unsigned short int log) {
    DIR *dir_stream = opendir(dir);
    if (dir_stream == NULL) {
        if (log) printf("Couldn't open directory \"%s\"\n", dir);

        return;
    }

    struct dirent *directory_entry;
    while ((directory_entry = readdir(dir_stream)) != NULL) {
        if (strcmp(directory_entry->d_name, ".") == 0 || strcmp(directory_entry->d_name, "..") == 0) continue;
        if ((int) directory_entry->d_type == 4) {
            char full_path[256 + 1024 + 2];

            snprintf(full_path, sizeof(full_path), "%s/%s", dir, directory_entry->d_name);

            find_in_dir(text, full_path, file_ext, log);

            continue;
        }

        if (ends_with(directory_entry->d_name, file_ext) == 0) {
            char full_path[256 + 1024 + 2];

            snprintf(full_path, sizeof(full_path), "%s/%s", dir, directory_entry->d_name);

            find_in_file(text, full_path, log);
        }
    }

    closedir(dir_stream);
}

int starts_with(const char *str, const char *prefix) {
    return strncmp(prefix, str, strlen(prefix));
}

int ends_with(const char *str, const char *prefix) {
    unsigned long int strLen = strlen(str);
    unsigned long int prefixLen = strlen(prefix);

    for (int i = 0; i < strlen(prefix); ++i) {
        if (strcmp(&str[strLen - prefixLen + i], &prefix[i]) != 0) return 1;
    }

    return 0;
}

void show_help() {
    printf("Available commands:\n");
    printf("\tfind:\n");
    printf("\t\ttext      (t)   - Text you want to search for\n");
    printf("\t\tdir       (d)   - Text you want to search in\n");
    printf("\t\tfile-ext  (fe)  - File types you want to search in\n");
    printf("\t\tlog       (l)   - log mode\n");
    printf("\nExample command: \'sf -t=\"printf\" --dir=\".\" -fe=\".c\" --log\'\n");
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
    unsigned short int log = 0;

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
        else if (starts_with(argv[i], "-l") == 0 || starts_with(argv[i], "--log") == 0) {
            log = 1;

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

    find_in_dir(text, dir, file_ext, log);

    free(text);
    free(dir);
    free(file_ext);

    return EXIT_SUCCESS;
}
