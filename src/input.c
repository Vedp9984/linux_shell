#include "header.h"

int job_number = 1;
// int background = 0;

#define MAX_CMD_LENGTH 1111

void handle_input(char *input)
{
  char new_input[MAX_CMD_LENGTH] = {0};
  int new_input_index = 0;

  for (int i = 0; input[i] != '\0'; i++)
  {
    if (input[i] == '&')
    {

      new_input[new_input_index++] = input[i];
      new_input[new_input_index++] = ';';
    }
    else
    {

      new_input[new_input_index++] = input[i];
    }
  }

  new_input[new_input_index] = '\0';
  char *command = strtok(new_input, ";");
  while (command)
  {

    command = trim_whitespace(command);

    if (strlen(command) == 0)
    {
      command = strtok(NULL, ";");
      continue;
    }

    // log_command(command);
    command = trim_whitespace(command);
    char *cmnd = (char *)malloc(strlen(command) + 2);
    strcpy(cmnd, command);
    cmnd = trim_whitespace(cmnd);
    int background = 0;
    if (strchr(command, '&'))
    {
      background = 1;
    }

    if (strchr(command, '|'))
    {

      execute_piped_commands(command);

     
    }
    else if (strchr(command, '>') || strchr(command, '<') || strstr(command, "cat"))
    {
      handle_redirection(command);
     
    }
    else if (strstr(command, "iMan"))
    {
      handle_iMan(command);
    }
    else if (strstr(command, "neonate"))
    {
      handle_neonate(command);
    }
    else if (strstr(command, "bg") || strstr(command, "fg"))
    {
      handle_fg_bg(command);
    }
    else if (strstr(command, "ping"))
    {
      handle_ping_command(command);
    }
    else if (strstr(command, "log") == command)
    {

      handle_log_command(command);

      free(cmnd);
      return;
    }
    else if (strcmp(command, "activities") == 0)
    {

      print_activities();
    }
    else
    {
      char *args[MAX_CMD_LENGTH / 2];
      int arg_count = 0;
      char *start = command;
      char *p = command;
      int in_quotes = 0;

      for (; *p; ++p)
      {
        if (*p == '"')
        {
          in_quotes = !in_quotes;
        }
        else if (!in_quotes && (*p == ' ' || *p == '\t'))
        {
          *p = '\0';
          if (*start)
          {
            args[arg_count++] = start;
          }
          start = p + 1;
        }
      }
      if (*start)
      {
        args[arg_count++] = start;
      }
      args[arg_count] = NULL;

      if (arg_count > 0)
      {
        if (strcmp(args[0], "hop") == 0)
        {
          hop(args + 1);
        }
        else if (strcmp(args[0], "reveal") == 0)
        {
          reveal(args[1], args[2] ? args[2] : ".");
        }
        else if (strcmp(args[0], "proclore") == 0)
        {
          int pid = args[1] ? atoi(args[1]) : 0;
          proclore(pid);
        }
        else if (strcmp(args[0], "seek") == 0)
        {
          
          seek(cmnd);
       
          return;
        }
        else
        {
          int y = execute_command(cmnd, background, &job_number);
          if (y == 0)
          {
            // printf("command: %s not found....\n", cmnd);
          }
          else
          {
            ;
          }
        }
      }
    }

    free(cmnd);

    command = strtok(NULL, ";");
  }
}
