#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define SLEEP_TIME 2 // Time to wait for recheking processes
#define LOG_FILE "process_log.txt"

typedef struct {
    char pid[32];
    char PPid[32];
    char name[256];
    char uid[32];
} ProcessInfo;

void log_event(const char *event, const ProcessInfo *proc) {
    FILE *log_file = fopen(LOG_FILE, "a");
    
    if (!log_file) {
        perror("Error opening log file");
        return;
    }

    const char *proc_type = (atoi(proc->uid) == 0 || atoi(proc->uid) < 1000) ? "SYSTEM" : "USER";

    fprintf(log_file, "[%s] [%s] PID: %s (%s) | PPID: %s | UID: %s\n",
        event, proc_type, proc->pid, proc->name, proc->PPid, proc->uid);   

    fclose(log_file);
}

void print_process_event(const char *event, const ProcessInfo *proc) {
    const char *proc_type = (atoi(proc->uid) == 0 || atoi(proc->uid) < 1000) ? "SYSTEM" : "USER";

    printf("[%s] [%s] PID: %s (%s) | PPID: %s | UID: %s\n",
           event, proc_type, proc->pid, proc->name, proc->PPid, proc->uid);
}

void get_process_info(const char *pid, char *name, char *PPid, char *uid) {
    char path[256], line[256];
    FILE *file;

    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    file = fopen(path, "r");

    if (!file) {
        strcpy(name, "Unknown");
        strcpy(PPid, "N/A");
        strcpy(uid, "N/A");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name:\t%255s", name);
        }
        if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line, "PPid:\t%31s", PPid);
        }
        if (strncmp(line, "Uid:", 4) == 0) {
            sscanf(line,"Uid:\t%31s", uid);
        }
    }
    fclose(file);
}

void get_process_list(ProcessInfo process_list[], int *count) {
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (!dp) {
        perror("opendir");
        return;
    }

    *count = 0;
    while ((entry = readdir(dp)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            strcpy(process_list[*count].pid, entry->d_name);
            get_process_info(process_list[*count].pid, process_list[*count].name, 
                process_list[*count].PPid, process_list[*count].uid);
            (*count)++;
        }
    }
    closedir(dp);
}



void monitor_processes() {
    ProcessInfo old_list[1024], new_list[1024];
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
                if (strcmp(new_list[i].pid, old_list[j].pid) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (!found) {
                    print_process_event("NEW", &new_list[i]);
                    log_event("NEW", &new_list[i]);
                }
            }
        }

        //check for terminated processes
        for (int i = 0; i < old_count; i++) {
            int found = 0;
            for (int j = 0; j < new_count; j++) {
                if (strcmp(old_list[i].pid, new_list[i].pid) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                print_process_event("TERMINATED", &old_list[i]);
                log_event("TERMINATED", &old_list[i]);
            }
        }

        memcpy(old_list, new_list, sizeof(new_list));
        old_count = new_count;
    }
}

int main() {
    printf("Listing Running Processes:\n");
    monitor_processes();
    return 0;
}