#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "activity.h"
#include "executor.h"
extern int fg_pid;
extern Process *head;
char curprocess[256];

void sig_c(int signum)
{

    if (fg_pid != -1)
    {
        kill(fg_pid, SIGINT);
        fg_pid = -1;
    }
    printf("\n");
}

void handle_sigstop(int sig)
{
    if (fg_pid != -1)
    {
        if (kill(fg_pid, SIGSTOP) == -1)
        {
            perror("kill");
            return;
        }
        strcpy(curprocess,"foregroundProcess");
        // printf("%s ",curprocess);
        // printf("pid-->is%d\n",fg_pid);
        add_process(fg_pid,curprocess,"stopped");
        fg_pid = -1;
    }
    

    printf("\n");
}

void ping_command(pid_t pid, int signal_number)
{
    signal_number %= 32;
    if (kill(pid, signal_number) == -1)
    {
        perror("kill");
        return;
    }
    printf("Sent signal %d to process with pid %d\n", signal_number, pid);
    update_process_info(head);
}

void handle_ping_command(char *cmd)
{
    char *token = strtok(cmd, " ");
    token = strtok(NULL, " ");
    if (!token)
    {
        fprintf(stderr, "Usage: ping <pid> <signal_number>\n");
        return;
    }
    int pid = atoi(token);

    token = strtok(NULL, " ");
    if (!token)
    {
        fprintf(stderr, "Usage: ping <pid> <signal_number>\n");
        return;
    }
    int signal = atoi(token);

    ping_command(pid, signal);
}
