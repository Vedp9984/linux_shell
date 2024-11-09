#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "activity.h"

Process *head = NULL;

// Find process by PID
Process *find_process_by_pid(int pid)
{
    Process *current = head;
    while (current != NULL)
    {
        // printf("Checking process with PID: %d\n", current->pid); // Debug print
        if (current->pid == pid)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Add a new process to the list

void add_process(int pid, const char *command, const char *state)
{
    // printf("Adding process: PID=%d, Command=%s, State=%s\n", pid, command, state); // Debug print

    Process *new_process = (Process *)malloc(sizeof(Process));
    if (!new_process)
    {
        perror("Failed to allocate memory for new process");
        return;
    }
    new_process->pid = pid;
    strncpy(new_process->command, command, sizeof(new_process->command) - 1);
    new_process->command[sizeof(new_process->command) - 1] = '\0';
    if (state)
    {
        strncpy(new_process->state, state, sizeof(new_process->state) - 1);
    }
    else
    {
        strcpy(new_process->state, "Unknown");
    }
    new_process->state[sizeof(new_process->state) - 1] = '\0';
    new_process->next = head;
    head = new_process;

    // Print the list for debugging
    // Process *current = head;
    // printf("Current process list:\n");
    // while (current != NULL)
    // {
    //     printf("PID: %d, Command: %s, State: %s\n", current->pid, current->command, current->state);
    //     current = current->next;
    // }
}

// Update process information
int update_process_info(Process *process)
{
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/stat", process->pid);

    FILE *file = fopen(path, "r");
    if (!file)
    {
        if (errno == ENOENT)
        {
            // Process no longer exists
            strcpy(process->state, "Terminated");
            return 0;
        }
        else
        {
            perror("Failed to open stat file");
            return -1;
        }
    }

    char state;
    int result = fscanf(file, "%*d %*s %c", &state);
    fclose(file);

    if (result != 1)
    {
        fprintf(stderr, "Failed to read process state for PID %d\n", process->pid);
        return -1;
    }

    // switch (state)
    // {
    // case 'R':
    //     printf("Running\n");
    //     break;
    // case 'S':
    //     printf("Running\n");
    //     break;
    // case 'D':
    //     printf("UNINTERRUPTIBLE SLEEP\n");
    //     break;
    // case 'Z':
    //     printf("Running\n");
    //     break;
    // case 'T':
    //     printf("Stopped\n");
    //     break;
    // case 'X':
    //     printf("stopped\n");
    //     break;
    // default:
    //     printf("UNKNOWN STATE\n");
    //     break;
    //     return -1;
    // }

    // Update the process state based on the character
    if (state == 'T')
    {
        strcpy(process->state, "Stopped");
    }
    else
    {
        strcpy(process->state, "Running");
    }

    return 0;
}

// Update all processes in the list
void update_process_list()
{
    Process *current = head;
    while (current != NULL)
    {
        update_process_info(current);
        current = current->next;
    }
}

// Remove a process from the list
void remove_process(int pid)
{
    Process **current = &head;
    while (*current != NULL)
    {
        Process *entry = *current;
        if (entry->pid == pid)
        {
            *current = entry->next;
            free(entry);
            return;
        }
        current = &entry->next;
    }
}

// Compare two processes by command for sorting
int compare_processes(const void *a, const void *b)
{
    Process *processA = *(Process **)a;
    Process *processB = *(Process **)b;
    return strcmp(processA->command, processB->command);
}

// Get the status of a process from /proc/[pid]/status
void get_process_status(int pid, char *status, size_t size)
{
    char path[40], line[100], *p;
    FILE *statusf;

    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    statusf = fopen(path, "r");
    if (!statusf)
    {
        // perror("Failed to open status file");
        strncpy(status, "Unknown", size - 1);
        status[size - 1] = '\0';
        return;
    }

    while (fgets(line, sizeof(line), statusf))
    {
        if (strncmp(line, "State:", 6) == 0)
        {
            p = line + 7;
            while (isspace(*p))
                ++p;
            strncpy(status, p, 1);
            status[1] = '\0';
            break;
        }
    }
    fclose(statusf);
}

// Print all activities
void print_activities()
{
    int count = 0;
    Process *current = head;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }

    Process *processes[count];
    current = head;
    for (int i = 0; i < count; i++)
    {
        processes[i] = current;
        current = current->next;
    }

    qsort(processes, count, sizeof(Process *), compare_processes);

    char status[100];
    for (int i = 0; i < count; i++)
    {
        get_process_status(processes[i]->pid, status, sizeof(status));

        printf("%d : %s - ", processes[i]->pid, processes[i]->command);
        char state = status[0];
        switch (state)
        {
        case 'R':
        case 'S':
            printf("Running\n");
            break;
        case 'D':
            printf("UNINTERRUPTIBLE SLEEP\n");
            break;
        case 'Z':
            printf("Zombie\n");
            break;
        case 'T':
            printf("Stopped\n");
            break;
        case 'X':
            printf("Dead\n");
            break;
        default:
            // printf("Unknown\n");
            break;
        }
    }

    // printf("Total processes: %d\n", count); // Debug print
}
