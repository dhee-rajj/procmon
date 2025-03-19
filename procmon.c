#include <stdio.h>
#include <dirent.h>
#include <ctype.h>

void print_process_info(const char *pid) {
    char path[256], line[256];
    FILE *file;

    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    file = fopen(path, "r");

    if (!file) return;

    printf("\nProcess ID: %s\n", pid);
    while(fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Name:", 5) == 0 || strncmp(line, "PPid:", 5) == 0) {
            printf("%s", line);
        }
    }
}

void list_processes() {
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            print_process_info(entry->d_name);
        }
    }

    closedir(dp);
}

int main() {
    printf("Listing Running Processes:\n");
    list_processes();
    return 0;
}