#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "utils.h"
#include "redirection.h"
#define MAX_COMMANDS 128
#define MAX_CMD_LENGTH 1111

extern int execute_command(char *command, int background, int *job_number);
extern job_number;

void execute_piped_commands(char *command)
{
    char *commands[MAX_COMMANDS];
    int num_commands = 0;
    char *token = strtok(command, "|");

    while (token != NULL)
    {
        commands[num_commands++] = trim_whitespace(token);
        token = strtok(NULL, "|");
    }

    if (num_commands == 0 || strlen(commands[0]) == 0 || strlen(commands[num_commands - 1]) == 0)
    {
        fprintf(stderr, "mysh: Invalid use of pipe.\n");
        return;
    }

    for (int i = 1; i < num_commands; i++)
    {
        if (strlen(commands[i]) == 0)
        {
            fprintf(stderr, "mysh: Invalid use of pipe.\n");
            return;
        }
    }

    int pipe_fds[2];
    int input_fd = STDIN_FILENO;
    int status;

    for (int i = 0; i < num_commands; i++)
    {
        if (i < num_commands - 1)
        {
            if (pipe(pipe_fds) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            pipe_fds[0] = -1;
            pipe_fds[1] = -1;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            
            if (input_fd != STDIN_FILENO)
            {
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            if (pipe_fds[1] != -1)
            {
                dup2(pipe_fds[1], STDOUT_FILENO);
                close(pipe_fds[1]);
            }

           
            if (pipe_fds[0] != -1)
            {
                close(pipe_fds[0]);
            }

            char *cmd_copy = strdup(commands[i]);
            if (strchr(cmd_copy, '>') || strchr(cmd_copy, '<'))
            {
                handle_redirection(cmd_copy);
            }
            else
            {
                execute_command(cmd_copy, 0, &job_number);
            }
            free(cmd_copy);
            exit(EXIT_SUCCESS);
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else
        {
            

            if (pipe_fds[1] != -1)
            {
                close(pipe_fds[1]);
            }

            if (input_fd != STDIN_FILENO)
            {
                close(input_fd);
            }

            input_fd = pipe_fds[0];

            waitpid(pid, &status, 0);
            // if (status != 0)
            // {
            //     fprintf(stderr, "ERROR: Command failed with status %d\n", status);
            // }

            if (i == num_commands - 1 && input_fd != -1)
            {
                close(input_fd);
            }
        }
    }
}

