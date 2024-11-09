

#include "redirection.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"

extern int execute_command(char *command, int background, int *job_number);
extern int job_number;

void handle_redirection(char *command)
{
    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;
    char *cmd_part = NULL;

    char *cmd_copy = strdup(command);
    char *ptr = cmd_copy;

    char *token = strtok(ptr, " \t");
    while (token != NULL)
    {
        if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " \t");
            if (token)
            {
                output_file = trim_whitespace(token);
                append = 0;
            }
        }
        else if (strcmp(token, ">>") == 0)
        {
            token = strtok(NULL, " \t");
            if (token)
            {
                output_file = trim_whitespace(token);
                append = 1;
            }
        }
        else if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " \t");
            if (token)
            {
                input_file = trim_whitespace(token);
            }
        }
        else
        {
            if (cmd_part == NULL)
            {
                cmd_part = strdup(token);
            }
            else
            {
                char *temp = realloc(cmd_part, strlen(cmd_part) + strlen(token) + 2);
                if (temp)
                {
                    cmd_part = temp;
                    strcat(cmd_part, " ");
                    strcat(cmd_part, token);
                }
            }
        }
        token = strtok(NULL, " \t");
    }

    // printf("Command: '%s'\n", cmd_part);
    // printf("Input file: '%s'\n", input_file ? input_file : "None");
    // printf("Output file: '%s'\n", output_file ? output_file : "None");
    // printf("Append mode: %d\n", append);

    pid_t pid = fork();
    if (pid == 0)
    {
        // Child process

        // Handle input redirection
        if (input_file)
        {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd < 0)
            {
                perror("No such input file found!");
                exit(EXIT_FAILURE);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        // Handle output redirection
        if (output_file)
        {
            int flags = O_CREAT | O_WRONLY;
            if (append)
            {
                flags |= O_APPEND;
            }
            else
            {
                flags |= O_TRUNC;
            }
            int output_fd = open(output_file, flags, 0644);
            if (output_fd < 0)
            {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        if (execute_command(cmd_part, 0, &job_number) == 0)
        {
            // fprintf(stderr, "Command: %s not found..\n", cmd_part);
            ;
        }

        free(cmd_part);
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        // Fork failed
        perror("Fork failed");
    }
    else
    {

        wait(NULL);
    }

    free(cmd_copy);
}
