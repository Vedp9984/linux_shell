#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <ctype.h>
#include <sys/time.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "neonate.h"
#include "input.h"

struct termios orig_termios;

void die(const char *s)
{
    perror(s);
    exit(1);
}
int compare_ints(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

void disableRmode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

void enableRmode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRmode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int give_latest_pid()
{
    
    DIR *proc_dir;
    struct dirent *entry;
    int pids[1024]; 
    int pid_count = 0;

    proc_dir = opendir("/proc");
    if (proc_dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(proc_dir)) != NULL)
    {

        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0]))
        {
            int pid = atoi(entry->d_name);
            pids[pid_count++] = pid;
        }
    }

    closedir(proc_dir);

    qsort(pids, pid_count, sizeof(int), compare_ints);

    return pids[pid_count - 1];
}

int isKeypassed()
{
    struct timeval tv = {0, 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0;
}

void execute_neonate(int time_arg)
{
    if (time_arg <= 0)
    {
        fprintf(stderr, "Error: time_arg must be a positive integer\n");
        exit(EXIT_FAILURE);
    }

    enableRmode();

    int PID = fork();

    if (PID < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    else if (PID == 0)
    {
      
        int last_pid = -1;
        while (1)
        {
            int current_pid = give_latest_pid();
            if (current_pid > last_pid)
            {
                printf("%d\n", current_pid);
                last_pid = current_pid;
                fflush(stdout);
            }
            sleep(time_arg);
        }
        exit(0);
    }
    else
    {
        
        char ch;
        while (1)
        {
            if (isKeypassed())
            {
                read(STDIN_FILENO, &ch, 1);
                if (ch == 'x')
                {
                    kill(PID, SIGTERM);
                    wait(NULL);
                    break;
                }
            }
        }
    } 
    disableRmode();
}

void handle_neonate(char *cmd)
{
    char *token = strtok(cmd, " ");
    if (token == NULL)
        return;

    if (strcmp(token, "neonate") == 0)
    {
        token = strtok(NULL, " ");
        if (token == NULL || strcmp(token, "-n") != 0)
        {
            fprintf(stderr, "Usage: neonate -n <time_arg>\n");
            return;
        }

        token = strtok(NULL, " ");
        if (token == NULL)
        {
            fprintf(stderr, "Usage: neonate -n <time_arg>\n");
            return;
        }

        int time_arg = atoi(token);
        execute_neonate(time_arg);
    }
    else
    {
        printf("Command not recognized: %s\n", token);
    }
}
