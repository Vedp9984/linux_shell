#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <sys/wait.h>
#include "fg_bg.h"
#include "activity.h"
#include "header.h"
#include "signals.h"

extern Process *head;
extern fg_pid;
void fg(int pid)
{
    int pgid = getpgid(pid);
    int shellPid = getpgid(0);

    tcsetpgrp(0, pgid);
    Process *process = find_process_by_pid(pid);
    if (process == NULL)
    {
        printf("No such process found\n");
        return;
    }
    printf("Bringing process %d (%s) to foreground\n", pid, process->command);
    fg_pid = process->pid;

    kill(pid, SIGCONT);

    // Wait for the process to complete
    int status;
    waitpid(pid, &status, WUNTRACED);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    tcsetpgrp(0, shellPid);

    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);

    if (WIFSTOPPED(status))
    {
        strcpy(process->state, "Stopped");
    }
    else
    {
        strcpy(process->state, "Running");
    }
}
void bg(int pid)
{
    // Debug print to indicate function entry
    // printf("Entering bg function with PID: %d\n", pid);

    // Update the process list before performing the operation
    // update_process_info(head);
     update_process_list();

        // printf("Updated process list:\n");
        // Process *current = head;
        // while (current != NULL) {
        //     printf("PID: %d, Command: %s, State: %s\n", current->pid, current->command, current->state);
        //     current = current->next;
        // }

        // Find the process with the given PID
        Process *process = find_process_by_pid(pid);
    if (process == NULL)
    {
        printf("No such process found\n");
        return;
    }

    if (strcmp(process->state, "Stopped") == 0)
    {
        printf("Resuming process %d (%s) in background\n", pid, process->command);
        if (kill(pid, SIGCONT) == -1)
        {
            perror("Failed to send SIGCONT");
        }
        else
        {
            strcpy(process->state, "Running");
            printf("Process %d state updated to 'Running'\n", pid);
        }
    }
    else
    {
        printf("Process %d is already running\n", pid);
    }
}

void handle_fg_bg(char *command)
{

    char *cmd = strtok(command, " ");
    int pid = atoi(strtok(NULL, " "));

    if (strcmp(cmd, "fg") == 0)
    {
        fg(pid);
    }
    else if (strcmp(cmd, "bg") == 0)
    {
        bg(pid);
    }
    else
    {
        printf("Unknown command\n");
    }
}
