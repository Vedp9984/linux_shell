// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/wait.h>
// #include <sys/time.h>
// #include "executor.h"
// #include "activity.h"
// #include "input.h"

// #define MAX_CMD_LENGTH 1024
// #define MAX_ARGV 64
// #define MAX_ARGV 100
// #define MAX_ARG_LEN 1024

// char *resolve_path(char *cmd)
// {
//     char *path_env = getenv("PATH");
//     if (!path_env)
//     {
//         return NULL;
//     }

//     char *path = strdup(path_env);
//     char *dir = strtok(path, ":");
//     static char full_path[MAX_CMD_LENGTH];

//     while (dir)
//     {
//         snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
//         if (access(full_path, X_OK) == 0)
//         {
//             free(path);
//             return full_path;
//         }
//         dir = strtok(NULL, ":");
//     }

//     free(path);
//     return NULL;
// }

// int execute_command(char *cmd, int background, int *job_number) {
//     // Copy cmd to keep the original command intact for process management
//     char cmd_copy[MAX_ARG_LEN];
//     strncpy(cmd_copy, cmd, sizeof(cmd_copy) - 1);
//     cmd_copy[sizeof(cmd_copy) - 1] = '\0';

//     pid_t pid = fork();
//     if (pid == 0) {
//         char *argv[MAX_ARGV];
//         int i = 0;
//         argv[i++] = strtok(cmd, " ");
//         char *token = strtok(NULL, " ");
//         while (token && i < MAX_ARGV - 1) {
//             argv[i++] = token;
//             token = strtok(NULL, " ");
//         }
//         argv[i] = NULL;

//         char *path = resolve_path(argv[0]);
//         if (path) {
//             execv(path, argv);
//             perror("execv");    // If execv fails
//             exit(EXIT_FAILURE); // Ensure child exits if execv fails
//         } else {
//             fprintf(stderr, "mysh: %s: is not a valid command\n", argv[0]);
//             exit(EXIT_FAILURE);
//         }
//     } else if (pid < 0) {
//         // Forking error
//         perror("Fork failed");
//         return 0; // Indicate failure
//     } else {
//         if (background) {
//             printf("[%d] %d\n", (*job_number)++, pid);
//             add_process(pid, cmd_copy, "Running");
//             return 1;
//         } else {
//             struct timeval start, end;
//             gettimeofday(&start, NULL);
//             int status;
//             waitpid(pid, &status, 0);

//             gettimeofday(&end, NULL);
//             long seconds = end.tv_sec - start.tv_sec;
//             if (seconds > 2) {
//                 printf("Foreground process '%s' took %lds to complete\n", cmd, seconds);
//             }

//             update_process_state(pid, "Stopped");

//             if (WIFEXITED(status)) {
//                 if (WEXITSTATUS(status) != 0) {
//                     fprintf(stderr, "Process with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
//                 }
//                 remove_process(pid);
//                 return 1;
//             } else {
//                 remove_process(pid);
//                 return 0;
//             }
//         }
//     }
// }

// void handle_background_processes()
// {
//     int status;
//     pid_t pid;
//     while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
//     {
//         if (WIFEXITED(status))
//         {
//             printf("Process %d exited normally\n", pid);
//         }
//         else if (WIFSIGNALED(status))
//         {
//             printf("Process %d was terminated by signal %d\n", pid, WTERMSIG(status));
//         }
//     }
// }

#include "header.h"

#define MAX_CMD_LENGTH 1024
#define MAX_ARGV 100
#define MAX_ARG_LEN 1024
extern fg_pid;

char *resolve_path(char *cmd)
{
    char *path_env = getenv("PATH");
    if (!path_env)
    {
        return NULL;
    }
    char *path = strdup(path_env);
    char *dir = strtok(path, ":");
    static char full_path[MAX_CMD_LENGTH];

    while (dir)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);
        if (access(full_path, X_OK) == 0)
        {
            free(path);
            return full_path;
        }
        dir = strtok(NULL, ":");
    }

    free(path);
    return NULL;
}

char **custom_tokenizer(const char *cmd, int *arg_count)
{
    char **argv = (char **)malloc(MAX_ARGV * sizeof(char *));
    if (!argv)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    const char *delim = " \t";

    int count = 0;
    int in_quotes = 0;
    char buffer[MAX_ARG_LEN];
    int buf_index = 0;

    for (int i = 0; cmd[i] != '\0'; ++i)
    {
        if (cmd[i] == '"' && (i == 0 || cmd[i - 1] != '\\'))
        {
            in_quotes = !in_quotes;
            continue;
        }
        if (!in_quotes && strchr(delim, cmd[i]))
        {
            if (buf_index > 0)
            {
                buffer[buf_index] = '\0';
                argv[count] = strdup(buffer);
                if (!argv[count])
                {
                    perror("strdup");
                    // Free previously allocated memory before exiting
                    for (int j = 0; j < count; ++j)
                    {
                        free(argv[j]);
                    }
                    free(argv);
                    exit(EXIT_FAILURE);
                }
                count++;
                buf_index = 0;
            }
        }
        else
        {
            if (buf_index < MAX_ARG_LEN - 1) // Avoid buffer overflow
            {
                buffer[buf_index++] = cmd[i];
            }
        }
    }
    if (buf_index > 0)
    {
        buffer[buf_index] = '\0';
        argv[count] = strdup(buffer);
        if (!argv[count])
        {
            perror("strdup");
            // Free previously allocated memory before exiting
            for (int j = 0; j < count; ++j)
            {
                free(argv[j]);
            }
            free(argv);
            exit(EXIT_FAILURE);
        }
        count++;
    }
    argv[count] = NULL;
    *arg_count = count;

    return argv;
}

void free_argv(char **argv, int arg_count)
{
    for (int i = 0; i < arg_count; ++i)
    {
        free(argv[i]);
    }
    free(argv);
}

int execute_command(char *cmd, int background, int *job_number)
{
 
    char cmd_copy[MAX_ARG_LEN];
    strncpy(cmd_copy, cmd, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';

    if (background)
    {
        char *ampersand = strchr(cmd_copy, '&');
        if (ampersand)
        {
            *ampersand = '\0';
        }
    }

    int arg_count;
    char **argv = custom_tokenizer(cmd_copy, &arg_count);
    if (argv == NULL)
    {
        fprintf(stderr, "Tokenizing command failed\n");
        return 0;
    }

    pid_t pid = fork();
    if (pid == 0)
    {
        // if (background)
        // {
        //     setsid();
        // }

        if (background && strstr(cmd_copy, "vim"))
        {
            freopen("/dev/null", "r", stdin);  // Redirect stdin from /dev/null
            freopen("/dev/null", "w", stdout); // Redirect stdout to /dev/null
            freopen("/dev/null", "w", stderr); // Redirect stderr to /dev/null
        }

        char *path = resolve_path(argv[0]);
        if (path)
        {

            if (background)
            {
                setpgrp();
                execv(path, argv);
                add_process(pid, cmd_copy,"Running");
                perror("execv");
                exit(EXIT_FAILURE);
                return 1;
            }
            else
            {
                fg_pid = pid;
                setsid();
                execv(path, argv);
                perror("execv");
                exit(EXIT_FAILURE);
                return 1;
            }
        }else{
            return 0;
        }
       
    }
    else if (pid < 0)
    {
        // Forking error
        perror("Fork failed");
        free_argv(argv, arg_count);
        return 0;
    }
    else
    {
        if (background)
        {

            printf("[%d] %d\n", (*job_number)++, pid);
            add_process(pid, cmd_copy, "Running");
            free_argv(argv, arg_count);
            return 1;
        }
        else
        {

            fg_pid = pid;
            struct timeval start, end;
            gettimeofday(&start, NULL);
            int status;
            waitpid(pid, &status, WUNTRACED);

            gettimeofday(&end, NULL);
            long seconds = end.tv_sec - start.tv_sec;
            if (seconds > 2)
            {
                ;
            }
            // update_process_state(pid, "Stopped");
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) != 0)
                {
                    fprintf(stderr, "Process with PID %d exited with status %d\n", pid, WEXITSTATUS(status));
                }
                remove_process(pid);
                free_argv(argv, arg_count);
                return 1;
            }
            else
            {
                // remove_process(pid);
                free_argv(argv, arg_count);
                return 0;
            }
        }
    }
}
