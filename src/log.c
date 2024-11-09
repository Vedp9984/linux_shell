#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "input.h"

#define MAX_LOG_SIZE 15
#define MAX_CMD_LENGTH 40
#define LOG_FILE "command_log.txt"

typedef struct
{
    char **commands;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

Queue *create_queue(int capacity)
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = 0;
    queue->size = 0;
    queue->rear = capacity - 1;
    queue->commands = (char **)malloc(queue->capacity * sizeof(char *));
    for (int i = 0; i < capacity; i++)
    {
        queue->commands[i] = (char *)malloc(MAX_CMD_LENGTH * sizeof(char));
    }
    return queue;
}

void destroy_queue(Queue *queue)
{
    for (int i = 0; i < queue->capacity; i++)
    {
        free(queue->commands[i]);
    }
    free(queue->commands);
    free(queue);
}

int is_full(Queue *queue)
{
    return (queue->size == queue->capacity);
}

int is_empty(Queue *queue)
{
    return (queue->size == 0);
}

void enqueue(Queue *queue, const char *cmd)
{
    if (is_full(queue))
    {
        queue->front = (queue->front + 1) % queue->capacity;
        queue->size--;
    }

    queue->rear = (queue->rear + 1) % queue->capacity;
    strncpy(queue->commands[queue->rear], cmd, MAX_CMD_LENGTH - 1);
    queue->commands[queue->rear][MAX_CMD_LENGTH - 1] = '\0';
    queue->size++;
}

char *dequeue(Queue *queue)
{
    if (is_empty(queue))
    {
        return NULL;
    }
    char *cmd = queue->commands[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return cmd;
}

char *peek(Queue *queue, int index)
{
    if (index < 0 || index >= queue->size)
    {
        return NULL;
    }
    int actual_index = (queue->front + index) % queue->capacity;
    return queue->commands[actual_index];
}

int queue_size(Queue *queue)
{
    return queue->size;
}

static Queue *log_queue = NULL;
static int log_initialized = 0;

void init_log()
{
    if (!log_initialized)
    {
        log_queue = create_queue(MAX_LOG_SIZE);

        FILE *file = fopen(LOG_FILE, "a+");
        if (file)
        {
            char line[MAX_CMD_LENGTH];
            while (fgets(line, sizeof(line), file))
            {

                if (queue_size(log_queue) > 15)
                {
                    dequeue(log_queue);
                }
                // if (strlen(line) > 0)
                // {
                //     enqueue(log_queue, line);
                // }
            }
            fclose(file);
        }

        log_initialized = 1;
    }
}

void log_command(const char *cmd)
{
    if (strstr(cmd, "log") != NULL)
    {
        return;
    }

    // Initialize log if not already done
    if (!log_initialized)
    {
        init_log();
    }

    int size = queue_size(log_queue);
    if (size > 0 && strcmp(peek(log_queue, size - 1), cmd) == 0)
    {
        return;
    }
    // printf("%s----pp",cmd);
    enqueue(log_queue, cmd);

    // Write the entire queue to the log file
    FILE *file = fopen(LOG_FILE, "w"); // Open for writing (clear previous contents)
    if (!file)
    {
        perror("Failed to open log file");
        return;
    }

    for (int i = 0; i < queue_size(log_queue); i++)
    {
        fprintf(file, "%s\n", peek(log_queue, i)); // Write each command as a new line
    }

    fclose(file);
}

void purge_log()
{
    if (!log_initialized)
    {
        init_log();
    }

    // Just clear the log file
    FILE *file = fopen(LOG_FILE, "w"); // Open for writing (clear contents)
    if (!file)
    {
        perror("Failed to open log file");
        return;
    }
    fclose(file);

    destroy_queue(log_queue);
    log_queue = create_queue(MAX_LOG_SIZE);
}

void display_log()
{
    if (!log_initialized)
    {
        init_log();
    }

    int size = queue_size(log_queue);
    for (int i = 0; i < size; i++)
    {
        printf("%d: %s\n", i + 1, peek(log_queue, i));
    }
}

void execute_log(int index)
{
    if (!log_initialized)
    {
        init_log();
    }

    int size = queue_size(log_queue);
    if (index < 1 || index > size)
    {
        fprintf(stderr, "ERROR: Invalid index %d\n", index);
        return;
    }

    char *cmd = peek(log_queue, index - 1);
    if (cmd)
    {
        printf("Executing command: %s\n", cmd);
        handle_input(cmd);
    }
}

void handle_log_command(const char *command)
{
    char *cmd_copy = strdup(command); // Create a modifiable copy of the command
    if (cmd_copy == NULL)
    {
        perror("Failed to duplicate command");
        return;
    }

    char *action = strtok(cmd_copy, " ");
    if (action == NULL)
    {
        fprintf(stderr, "ERROR: Invalid log command\n");
        free(cmd_copy);
        return;
    }

    // Handle the "log" command separately
    if (strcmp(action, "log") == 0)
    {
        // Check if there is a second token
        char *sub_action = strtok(NULL, " ");
        if (sub_action == NULL)
        {
            // If no second token, display the log
            display_log();
        }
        else if (strcmp(sub_action, "purge") == 0)
        {
            purge_log();
        }
        else if (strcmp(sub_action, "execute") == 0)
        {
            char *index_str = strtok(NULL, " ");
            if (index_str == NULL)
            {
                fprintf(stderr, "ERROR: No index specified for execute\n");
                free(cmd_copy);
                return;
            }
            int index = atoi(index_str);
            if (index <= 0)
            {
                fprintf(stderr, "ERROR: Invalid index %d\n", index);
                free(cmd_copy);
                return;
            }
            execute_log(index);
        }
        else
        {
            fprintf(stderr, "ERROR: Invalid log sub-command\n");
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Invalid log command\n");
    }

    free(cmd_copy);
}
