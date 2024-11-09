#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "prompt.h"
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <limits.h>

void hop(char **args)
{
    static char prev_dir1[PATH_MAX] = "";
    static char prev_dir2[PATH_MAX] = "";
    char *path = args[0];
    char cwd[PATH_MAX];
    char new_path[PATH_MAX];
    char resolved_path[PATH_MAX];
    char home_dir[PATH_MAX];

   
    if (getcwd(prev_dir1, sizeof(prev_dir1)) == NULL)
    {
        perror("getcwd");
        return;
    }

    if (path == NULL || strcmp(path, "~") == 0)
    {
        // Case 1: No argument or "~"
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        path = pw->pw_dir;
    }
    else if (strcmp(path, "-") == 0)
    {
        // Case 2: "-"
        if (strlen(prev_dir2) == 0)
        {
            fprintf(stderr, "No previous directory\n");
            return;
        }
        path = prev_dir2;
    }
    else if (path[0] == '~')
    {
        // Case 3: "~/directory" or "~"
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        snprintf(home_dir, sizeof(home_dir), "%s", pw->pw_dir);
        snprintf(new_path, sizeof(new_path), "%s%s", home_dir, path + 1);
        path = new_path;
    }
    else if (path[0] == '.' && path[1] == '/')
    {
        // Case 4: "./directory"
        snprintf(new_path, sizeof(new_path), "%s%s", getcwd(cwd, sizeof(cwd)), path + 1);
        path = new_path;
    }
    else if (strcmp(path, "..") == 0)
    {
        // Case 5: ".."
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd");
            return;
        }
        snprintf(new_path, sizeof(new_path), "%s/..", cwd);
        if (realpath(new_path, resolved_path) == NULL)
        {
            perror("realpath");
            return;
        }
        path = resolved_path;
    }

    if (chdir(path) != 0)
    {
        perror("chdir");
        return;
    }
    strncpy(prev_dir2, prev_dir1, sizeof(prev_dir2));
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd");
        return;
    }
    printf("%s\n", cwd);

    if (*(args + 1) != NULL)
    {
        snprintf(new_path, sizeof(new_path), "%s/%s", cwd, *(args + 1));
        if (realpath(new_path, resolved_path) == NULL)
        {
            perror("realpath");
            return;
        }
        path = resolved_path;

        if (chdir(path) != 0)
        {
            perror("chdir");
            return;
        }

        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
            perror("getcwd");
            return;
        }
        printf("%s\n", cwd);
    }
}


// void hop(char **args)
// {
//     static char prev_dir1[PATH_MAX] = "";
//     static char prev_dir2[PATH_MAX] = "";

//     char *path = args[0];
//     char cwd[PATH_MAX];
//     char new_path[PATH_MAX];

//     // Save the current directory as prev_dir1
//     if (getcwd(prev_dir1, sizeof(prev_dir1)) == NULL)
//     {
//         perror("getcwd");
//         return;
//     }

//     if (path == NULL)
//     {
//         uid_t uid = getuid();
//         struct passwd *pw = getpwuid(uid);
//         path = pw->pw_dir;
//     }
//     else if (strcmp(path, "~") == 0)
//     {
//         uid_t uid = getuid();
//         struct passwd *pw = getpwuid(uid);
//         path = pw->pw_dir;
//     }
//     else if (strcmp(path, "-") == 0)
//     {
//         if (strlen(prev_dir2) == 0)
//         {
//             fprintf(stderr, "No previous directory\n");
//             return;
//         }
//         path = prev_dir2;
//     }
//     else if (path[0] == '~')
//     {
//         if (path[1] == '\0' || path[1] == '/')
//         {
//             uid_t uid = getuid();
//             struct passwd *pw = getpwuid(uid);
//             snprintf(new_path, sizeof(new_path), "%s%s", pw->pw_dir, path + 1);
//             path = new_path;
//         }
//     }
//     else if (path[0] == '.' && path[1] == '/')
//     {
//         snprintf(new_path, sizeof(new_path), "%s%s", getcwd(cwd, sizeof(cwd)), path + 1);
//         path = new_path;
//     }
//     else if (strcmp(path, "..") == 0)
//     {
//         if (getcwd(cwd, sizeof(cwd)) == NULL)
//         {
//             perror("getcwd");
//             return;
//         }
//         snprintf(new_path, sizeof(new_path), "%s/..", cwd);
//         path = realpath(new_path, NULL); // Resolve `..` to the actual path
//         if (path == NULL)
//         {
//             perror("realpath");
//             return;
//         }
//     }

//     // Change to the specified directory
//     if (chdir(path) != 0)
//     {
//         perror("chdir");
//         return;
//     }

//     // Save the new directory as prev_dir2
//     strcpy(prev_dir2, prev_dir1);

//     // Print the new working directory
//     if (getcwd(cwd, sizeof(cwd)) == NULL)
//     {
//         perror("getcwd");
//         return;
//     }
//     printf("%s\n", cwd);

//     // Update home directory if needed
//     if (strcmp(args[0], "~") == 0 || strcmp(args[0], "-") == 0)
//     {
//         set_home_directory();
//     }

//     // Move to the next argument if available and append to the current directory
//     if (*(args + 1) != NULL)
//     {
//         // Construct the new path with appended arguments
//         snprintf(new_path, sizeof(new_path), "%s/%s", cwd, *(args + 1));
//         if (chdir(new_path) != 0)
//         {
//             perror("chdir");
//             return;
//         }
//         // Print the new working directory after moving to the new path
//         if (getcwd(cwd, sizeof(cwd)) == NULL)
//         {
//             perror("getcwd");
//             return;
//         }
//         printf("%s\n", cwd);
//     }
// }
