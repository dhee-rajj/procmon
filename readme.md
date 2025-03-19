# Linux Kernel Process Monitoring Tool (Phase 1)

## Overview
This document provides a step-by-step guide to building a **Linux process monitoring tool** in C using the `/proc` filesystem. The goal is to list running processes and extract details such as process ID (PID), name, and parent PID (PPID).

---

## **1️⃣ Listing Running Processes**

### **Code**
```c
#include <stdio.h>   // Standard I/O functions (printf, perror, fopen, fclose)
#include <dirent.h>  // Directory handling (opendir, readdir, closedir)
#include <ctype.h>   // Character checking (isdigit)

void list_processes() {
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            printf("Process ID: %s\n", entry->d_name);
        }
    }
    closedir(dp);
}

int main() {
    printf("Listing running processes:\n");
    list_processes();
    return 0;
}
```

### **Explanation**
- Opens the `/proc` directory using `opendir()`.
- Reads each entry using `readdir()`.
- If the entry name starts with a **digit**, it represents a process ID (PID).
- Prints the PID.
- Closes the directory using `closedir()`.

---

## **2️⃣ Fetching Process Details (Name & Parent PID)**

### **Code**
```c
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

void print_process_info(const char *pid) {
    char path[256], line[256];
    FILE *file;

    snprintf(path, sizeof(path), "/proc/%s/status", pid);
    file = fopen(path, "r");

    if (!file) return;
    
    printf("\nProcess ID: %s\n", pid);
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Name:", 5) == 0 || strncmp(line, "PPid:", 5) == 0) {
            printf("%s", line);
        }
    }
    fclose(file);
}

void list_processes() {
    struct dirent *entry;
    DIR *dp = opendir("/proc");

    if (!dp) {
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
    printf("Listing running processes with details:\n");
    list_processes();
    return 0;
}
```

### **Explanation**
- Constructs `/proc/<pid>/status` path using `snprintf()`.
- Opens the file and reads it line by line using `fgets()`.
- Extracts **process name** and **parent PID (PPID)** using `strncmp()`.
- Prints the information.

---

## **3️⃣ Summary of Key Functions**

| Function | Purpose |
|----------|---------|
| `opendir()` | Opens a directory (e.g., `/proc`) |
| `readdir()` | Reads entries inside a directory |
| `isdigit()` | Checks if a character is a digit (to identify PIDs) |
| `fopen()` | Opens a file for reading |
| `fgets()` | Reads a line from a file |
| `strncmp()` | Compares the beginning of a string |
| `snprintf()` | Safely formats a string |

---

## **4️⃣ Next Steps**
✅ Now that we can **list and extract process details**, we can add:
1️⃣ **Logging** (save process details to a file)
2️⃣ **Filtering** (ignore system daemons)
3️⃣ **Real-time process tracking** (detect new/terminated processes)

Let me know what you'd like to implement next! 🚀

