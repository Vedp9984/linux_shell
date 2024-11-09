#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>
// #include <limits.h>
#include "reveal.h"
#include <fcntl.h>
void print_permissions(mode_t mode)
{
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

void print_details(struct stat *file_stat, char *name)
{
    print_permissions(file_stat->st_mode);
    printf(" %ld", file_stat->st_nlink);

    struct passwd *pw = getpwuid(file_stat->st_uid);
    struct group *gr = getgrgid(file_stat->st_gid);

    printf(" %s %s", pw->pw_name, gr->gr_name);
    printf(" %ld", file_stat->st_size);

    char time_buf[80];
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat->st_mtime));
    printf(" %s ", time_buf);

    if (S_ISDIR(file_stat->st_mode))
        printf("\033[1;34m%s\033[0m\n", name);
    else if (file_stat->st_mode & S_IXUSR)
        printf("\033[1;32m%s\033[0m\n", name);
    else
        printf("\033[0m%s\033[0m\n", name);
}

void reveal(char *flags, char *path)
{
    int show_all = 0;
    int show_long = 0;

    if (flags == NULL || strlen(flags) == 0 )
    {
        show_all = 1;
    }
    else
    {

        for (int i = 0; flags && flags[i]; i++)
        {
            if (flags[i] == 'a')
            {
                show_all = 1;
            }
            else if (flags[i] == 'l')
            {
                show_long = 1;
            }else{
                ;
            }
            
        }
    }

    if (path == NULL || strlen(path) == 0)
    {
        path = ".";
    }

    char resolved_path[PATH_MAX];

    // Resolve the path
    if (strcmp(path, "~") == 0)
    {
        struct passwd *pw = getpwuid(getuid());
        strcpy(resolved_path, pw->pw_dir);
    }
    else if (strcmp(path, "-") == 0)
    {
        static char prev_dir[PATH_MAX] = "";
        if (strlen(prev_dir) == 0)
        {
            fprintf(stderr, "No previous directory\n");
            return;
        }
        strcpy(resolved_path, prev_dir);
    }
    else if (strcmp(path, ".") == 0)
    {
        if (getcwd(resolved_path, sizeof(resolved_path)) == NULL)
        {
            perror("getcwd");
            return;
        }
    }
    else if (strcmp(path, "..") == 0)
    {
        if (getcwd(resolved_path, sizeof(resolved_path)) == NULL)
        {
            perror("getcwd");
            return;
        }
        strcat(resolved_path, "/..");
    }
    else if (path[0] == '~')
    {
        struct passwd *pw = getpwuid(getuid());
        snprintf(resolved_path, sizeof(resolved_path), "%s%s", pw->pw_dir, path + 1);
    }
    else
    {
        if (realpath(path, resolved_path) == NULL)
        {
            perror("realpath");
    
            strcpy(resolved_path, ".");
        }
    }

    DIR *dir = opendir(resolved_path);
    if (!dir)
    {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    struct stat file_stat;
    char full_path[PATH_MAX];

    while ((entry = readdir(dir)) != NULL)
    {
       
        if (!show_all && entry->d_name[0] == '.')
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", resolved_path, entry->d_name);
        if (stat(full_path, &file_stat) == -1)
        {
            perror("stat");
            continue;
        }

     
        if (show_long)
        {
            print_details(&file_stat, entry->d_name);
        }
        else
        {
            if (S_ISDIR(file_stat.st_mode))
                printf("\033[1;34m%s\033[0m\n", entry->d_name); // Blue for directories
            else if (file_stat.st_mode & S_IXUSR)
                printf("\033[1;32m%s\033[0m\n", entry->d_name); // Green for executables
            else
                printf("\033[0m%s\033[0m\n", entry->d_name); // White for files
        }
    }

    closedir(dir);


    if (strcmp(path, "-") != 0 && strcmp(path, "~") != 0)
    {
        if (getcwd(resolved_path, sizeof(resolved_path)) != NULL)
        {
            static char prev_dir[PATH_MAX];
            strcpy(prev_dir, resolved_path);
        }
    }
}
