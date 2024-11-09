#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <pwd.h>
#include "prompt.h"
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PATH 1024
#define RESET "\033[0m"

#define BRIGHT_BLUE "\033[1;34m"
#define YELLOW "\033[1;33m"

char home_directory[MAX_PATH];
void set_home_directory()
{
    if (getcwd(home_directory, sizeof(home_directory)) == NULL)
    {
        perror("getcwd");
        home_directory[0] = '\0';
    }
}

void display_prompt()
{
    struct utsname sys_info;
    if (uname(&sys_info) != 0)
    {
        perror("uname");
        return;
    }

    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return;
    }

    char display_path[MAX_PATH];
    char *home = home_directory;

    if (strcmp(cwd, home) == 0)
    {
        snprintf(display_path, sizeof(display_path), "~");
    }
    else if (strncmp(cwd, home, strlen(home)) == 0 && cwd[strlen(home)] == '/')
    {
        snprintf(display_path, sizeof(display_path), "~%s", cwd + strlen(home));
    }
    else
    {
        snprintf(display_path, sizeof(display_path), "%s", cwd);
    }

    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    char *username = pw->pw_name;

    if (username == NULL)
    {
        username = "unknown";
    }
    printf(BRIGHT_BLUE "<%s"RESET"@%s:" YELLOW "%s> " RESET, username, sys_info.nodename, display_path);
}
