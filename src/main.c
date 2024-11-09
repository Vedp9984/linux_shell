#include <stdio.h>
#include "header.h"
#include "prompt.h"
#include "executor.h"

char filename[200];

int fg_pid = -1;

void child_handler(int sig)
{
    (void)sig;
    int saved_errno = errno;
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (WIFEXITED(status))
        {
            printf("Process %d exited normally with status %d\n", pid, WEXITSTATUS(status));
            // remove_process(pid);
        }
        else if (WIFSIGNALED(status))
        {
            printf("Process %d was terminated by signal %d\n", pid, WTERMSIG(status));
        }
        else if (WIFSTOPPED(status))
        {
            printf("Process %d was stopped by signal %d\n", pid, WSTOPSIG(status));
            break;
        }
        else
        {
            printf("Process %d exited abnormally\n", pid);
        }
        fflush(stdout);
        // remove_process(pid);
        // Remove the process from the linked list
            Process *prev = NULL;
            Process *current = head;

            while (current != NULL && current->pid != pid)
            {
                prev = current;
                current = current->next;
            }

            if (current != NULL)
            {
                if (prev == NULL)
                {
                    head = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                free(current);
            }
        }

        errno = saved_errno;
    }

    int main()
    {

        char cwd[128];
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("Failed to get current working directory");
            return 1;
        }

        char *rel_path = "/src/.myshrc";
        strcpy(filename, cwd);
        strcat(filename, rel_path);

        set_home_directory();
        char input[1110];

        struct sigaction sa;
        sa.sa_handler = child_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;

        if (sigaction(SIGCHLD, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(1);
        }

        signal(SIGTSTP, handle_sigstop);
        signal(SIGINT, sig_c);

        while (1)
        {
            display_prompt();
            if (fgets(input, sizeof(input), stdin))
            {
                input[strcspn(input, "\n")] = 0;
                char *cm = (char *)malloc(sizeof(char) * strlen(input) + 3);
                strcpy(cm, input);
                cm = trim_whitespace(cm);
                log_command(cm);
                free(cm);

                int x = myshrc_function(input);
                if (x)
                {
                    continue;
                }
                else
                {
                    handle_input(input);
                }
            }
            else
            {
                printf("\n");
                printf("\n_____Exiting the shell. Have a great day!\n.....\n.\n.....\n");
                exit(0);
            }
        }

        return 0;
    }
