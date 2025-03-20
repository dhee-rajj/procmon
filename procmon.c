#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SLEEP_TIME 2 // Time to wait for recheking processes

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
    printf("\n");
}

void get_process_list(char process_list[][32], int *count) {
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (!dp) {
        perror("opendir");
        return;
    }

    *count = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            strcpy(process_list[*count], entry->d_name);
            (*count)++;
        }
    }
    closedir(dp);
}

void monitor_processes() {
    char old_list[1024][32], new_list[1024][32];
    int old_count = 0, new_count = 0;

    get_process_list(old_list, &old_count);
    printf("Monitoring processes... Press Ctrl+C to stop.\n\n");

    while(1) {
        sleep(SLEEP_TIME);
        get_process_list(new_list, &new_count);
                
        // Check for new processes
        for (int i = 0; i < new_count; i++) {
            int found = 0;
            for (int j = 0; j < old_count; j++) {
                if (strcmp(new_list[i], old_list[j]) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (!found) {
                    printf("[NEW] Process Started: PID %s\n", new_list[i]);
                    print_process_info(new_list[i]);
                }
            }
        }

        //check for terminated processes
        for (int i = 0; i < old_count; i++) {
            int found = 0;
            for (int j = 0; j < new_count; j++) {
                if (strcmp(old_list[i], new_list[i]) ==0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("[TERMINATED] Process Ended: PID %s\n\n", 
                old_list[i]);
                print_process_info(old_list[i]); // info not printed??
            }
        }

        memcpy(old_list, new_list, sizeof(new_list));
        old_count = new_count;
    }
}


// void list_processes() {
//     struct dirent *entry;
//     DIR *dp = opendir("/proc");

//     if (dp == NULL) {
//         perror("opendir");
//         return;
//     }

//     while ((entry = readdir(dp)) != NULL) {
//         if (isdigit(entry->d_name[0])) {
//             print_process_info(entry->d_name);
//         }
//     }

//     closedir(dp);
// }




int main() {
    printf("Listing Running Processes:\n");
    monitor_processes();
    return 0;
}