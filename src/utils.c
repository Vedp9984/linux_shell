#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <linux/limits.h>
#include <ctype.h>
#include "utils.h"
#define PATH_MAX 4096
#include <ctype.h>
#include <string.h>

char *trim_whitespace(char *str)
{

    if (str == NULL)
    {
        return NULL;
    }

    while (isspace((unsigned char)*str))
    {
        str++;
    }

    if (*str == '\0')
    {
        return str;
    }

    char *end = str + strlen(str) - 1;

    while (end > str && isspace((unsigned char)*end))
    {
        end--;
    }

    *(end + 1) = '\0';

    return str;
}

void expand_tilde(char *path)
{
    if (path[0] == '~')
    {
        char home[PATH_MAX];
        if (getenv("HOME"))
        {
            snprintf(home, sizeof(home), "%s", getenv("HOME"));
            size_t home_len = strlen(home);
            size_t path_len = strlen(path);
            if (path_len > 1)
            {
                memmove(path + home_len, path + 1, path_len - 1);
                path[home_len] = '\0';
                strcat(path, path + home_len);
            }
            else
            {
                path[0] = '\0';
                strcat(path, home);
            }
        }
    }
}

void warp(const char *home, const char *target_directory, const char *command)
{
    char resolved_path[PATH_MAX];
    snprintf(resolved_path, sizeof(resolved_path), "%s/%s", home, target_directory);
    expand_tilde(resolved_path);

    printf("Resolved Path: %s\n", resolved_path);
    printf("Command: %s\n", command);
}
