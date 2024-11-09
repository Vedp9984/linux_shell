#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "mysh.h"

#define MAX_LINE_LENGTH 256
#define MAX_TOKENS 20

void tokenize_equal(const char *command, char **tokens, int *num_tokens)
{
  char *command_copy = strdup(command);
  char *delimiter = "=";
  char *token = strtok(command_copy, delimiter);

  *num_tokens = 0;
  while (token != NULL && *num_tokens < MAX_TOKENS)
  {
    tokens[(*num_tokens)++] = strdup(token);
    token = strtok(NULL, delimiter);
  }
  free(command_copy);
}

void tokenize_parenthesis(const char *command, char **tokens, int *num_tokens)
{
  char *command_copy = strdup(command); // Use strdup to allocate and copy
  char *delimiter = "(";
  char *token = strtok(command_copy, delimiter);

  *num_tokens = 0;
  while (token != NULL && *num_tokens < MAX_TOKENS)
  {
    tokens[(*num_tokens)++] = strdup(token);
    token = strtok(NULL, delimiter);
  }
  free(command_copy);
}

void tokenize_space(const char *command, char **tokens, int *num_tokens)
{
  char *command_copy = strdup(command);
  char *delimiter = " ";
  char *token = strtok(command_copy, delimiter);
  *num_tokens = 0;
  while (token != NULL && *num_tokens < MAX_TOKENS)
  {
    tokens[(*num_tokens)++] = strdup(token);
    token = strtok(NULL, delimiter);
  }
  free(command_copy);
}

int myshrc_function(const char *command)
{

  // char *filename = "/home/ved_maurya/sem3/OperatingSystemNetwork/mini-project-1-Vedp9984/src/.myshrc";

  FILE *file = fopen(filename, "r");
  if (!file)
  {
    perror("Failed to open file");
    return -1;
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file))
  {
    line[strcspn(line, "\n")] = '\0';
    char *tokens[MAX_TOKENS] = {0};
    int num_tokens = 0;

    if (strchr(line, '='))
    {
      tokenize_equal(line, tokens, &num_tokens);
    }
    else if (strchr(line, '('))
    {
      tokenize_parenthesis(line, tokens, &num_tokens);
    }
    else
    {
      continue;
    }

    command = trim_whitespace(command);
    if (!command)
    {
      fprintf(stderr, "Error: command is NULL after trimming\n");
      fclose(file);
      return -1;
    }

    tokens[0] = trim_whitespace(tokens[0]);
    if (!tokens[0])
    {
      fprintf(stderr, "Error: tokens[0] is NULL after trimming\n");
      fclose(file);
      return -1;
    }

    if (strcmp(command, tokens[0]) == 0)
    {
      handle_input(tokens[1]);

      for (int i = 0; i < num_tokens; i++)
      {
        free(tokens[i]);
      }
      fclose(file);
      return 1;
    }
    else if (num_tokens > 1 && strcmp(tokens[1], ")") == 0)
    {
      char *token_command[MAX_TOKENS] = {0};
      int count = 0;

      if (strchr(command, ' '))
      {
        tokenize_space(command, token_command, &count);
      }
        
      token_command[0] = trim_whitespace(token_command[0]);
      if (!token_command[0])
      {
        // fprintf(stderr, "Error: token_command[0] is NULL after trimming\n");
        fclose(file);
        return 0;
      }

      if (strcmp(token_command[0], tokens[0]) == 0)
      {
        if (strcmp(token_command[0], "hop_seek") == 0)
        {
          char command1[MAX_LINE_LENGTH];
          snprintf(command1, sizeof(command1), "hop %s", token_command[1]);
          handle_input(command1);

          char command2[MAX_LINE_LENGTH];
          snprintf(command2, sizeof(command2), "seek %s", token_command[1]);
          handle_input(command2);
        }
        else
        {
          if (mkdir(token_command[1], 0755) == 0)
          {
            chdir(token_command[1]);
          }
          else
          {
            perror("Failed to create directory");
          }
        }

        for (int i = 0; i < count; i++)
        {
          free(token_command[i]);
        }
        for (int i = 0; i < num_tokens; i++)
        {
          free(tokens[i]);
        }
        fclose(file);
        return 1;
      }
    }
    for (int i = 0; i < num_tokens; i++)
    {
      free(tokens[i]);
    }
  }
  fclose(file);
  return 0;
}