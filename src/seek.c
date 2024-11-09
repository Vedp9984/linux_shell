#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include<linux/limits.h>
#include <errno.h>
#include "seek.h"
#include "utils.h"

#define BLUE_COLOR "\033[34m"
#define GREEN_COLOR "\033[32m"
#define RESET_COLOR "\033[0m"

int flag_e = 0;
char f_d[265];

char home[1000];
char previous_directory[1000];

void initialize_globals()
{
    if (getcwd(home, sizeof(home)) == NULL)
    {
        perror("Error getting current working directory");
        exit(EXIT_FAILURE);
    }
    strcpy(previous_directory, home);
}

int search_dir_file(char *original_target_path, char *target_path, char *f_d_name, int flag, int found)
{
    if (strcmp(original_target_path, "~") == 0)
    {
        strcpy(original_target_path, home);
        strcpy(target_path, home);
    }
    else if (original_target_path[0] == '~')
    {
        snprintf(target_path, sizeof(target_path), "%s%s", home, original_target_path + 1);
        strcpy(original_target_path, target_path);
    }

    DIR *dir = opendir(target_path);
    if (dir == NULL)
    {
        perror("Error opening directory");
        return found;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        char fullPath[1000];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", target_path, entry->d_name);

        struct stat f_d_stat;
        if (lstat(fullPath, &f_d_stat) == -1)
        {
            perror("Error stat-ing file");
            continue;
        }

        char temp_path[1000];
        strncpy(temp_path, entry->d_name, sizeof(temp_path) - 1);
        temp_path[sizeof(temp_path) - 1] = '\0'; // Ensure null-termination
        char *optional_path = strtok(temp_path, ".");
        if (S_ISDIR(f_d_stat.st_mode))
        {
            if (strcmp(entry->d_name, f_d_name) == 0 && flag != 2)
            {
                found = 1;
                if (flag_e == 0)
                {
                    int dir_len = strlen(original_target_path);
                    printf(BLUE_COLOR ".%s/%s\n" RESET_COLOR, target_path + dir_len, f_d_name);
                }
                else if (flag_e == 1)
                {
                    strncpy(f_d, fullPath, sizeof(f_d) - 1);
                    f_d[sizeof(f_d) - 1] = '\0';
                    flag_e = 2;
                }
                else if (flag_e == 2)
                {
                    flag_e = 3;
                    closedir(dir);
                    return found;
                }
                else if (flag_e == 3)
                {
                    closedir(dir);
                    return found;
                }
            }
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                found = search_dir_file(original_target_path, fullPath, f_d_name, flag, found);
            }
        }
        else if (S_ISREG(f_d_stat.st_mode))
        {
            if ((strcmp(entry->d_name, f_d_name) == 0 || strcmp(optional_path, f_d_name) == 0) && flag != 1)
            {
                found = 1;
                if (flag_e == 0)
                {
                    int dir_len = strlen(original_target_path);
                    printf(GREEN_COLOR ".%s/%s\n" RESET_COLOR, target_path + dir_len, entry->d_name);
                }
                else if (flag_e == 1)
                {
                    strncpy(f_d, fullPath, sizeof(f_d) - 1);
                    f_d[sizeof(f_d) - 1] = '\0';
                    flag_e = 2;
                }
                else if (flag_e == 2)
                {
                    flag_e = 3;
                    closedir(dir);
                    return found;
                }
                else if (flag_e == 3)
                {
                    closedir(dir);
                    return found;
                }
            }
        }
    }

    closedir(dir);
    return found;
}

void seek(char *command)
{   
    char *d = strstr(command, "-d");
    char *f = strstr(command, "-f");
    char *e = strstr(command, "-e");

    char target_dir[1000];
    char f_d_name[1000];
    int found = 0;
    // printf("1\n");
    if (d == NULL && f == NULL && e == NULL)
    {
        strcpy(f_d_name, strtok(command, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        char *temp = strtok(NULL, " ");
        strcpy(target_dir, temp == NULL ? "." : temp);
        // printf("1\n");
        found = search_dir_file(target_dir, target_dir, f_d_name, 0, found);
        // printf("2\n");
    }
    else if (d != NULL && f != NULL)
    {
        printf("Invalid flags!\n");
    }
    else if (d != NULL && e == NULL)
    {
        strcpy(f_d_name, strtok(command, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        char *temp = strtok(NULL, " ");
        strcpy(target_dir, temp == NULL ? "." : temp);
        // printf("1e\n");
        found = search_dir_file(target_dir, target_dir, f_d_name, 1, found);
        // printf("2e\n");
    }
    else if (f != NULL && e == NULL)
    {
        strcpy(f_d_name, strtok(command, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        char *temp = strtok(NULL, " ");
        strcpy(target_dir, temp == NULL ? "." : temp);
        found = search_dir_file(target_dir, target_dir, f_d_name, 2, found);
    }
    else if (e != NULL && d == NULL && f == NULL)
    {
        flag_e = 1;
        strcpy(f_d_name, strtok(command, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        char *temp = strtok(NULL, " ");
        strcpy(target_dir, temp == NULL ? "." : temp);
        found = search_dir_file(target_dir, target_dir, f_d_name, 0, found);
    }
    else if (e != NULL && d != NULL)
    {
        flag_e = 1;
        strcpy(f_d_name, strtok(command, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        char *temp = strtok(NULL, " ");
        strcpy(target_dir, temp == NULL ? "." : temp);
        found = search_dir_file(target_dir, target_dir, f_d_name, 1, found);
    }
    else if (e != NULL && f != NULL)
    {
        flag_e = 1;
        strcpy(f_d_name, strtok(command, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        strcpy(f_d_name, strtok(NULL, " "));
        char *temp = strtok(NULL, " ");
        strcpy(target_dir, temp == NULL ? "." : temp);
        found = search_dir_file(target_dir, target_dir, f_d_name, 2, found);
    }

    if (found == 0)
        printf("Not found!\n");

    if (flag_e == 2)
    {
        struct stat f_d_stat;
        if (lstat(f_d, &f_d_stat) == -1)
        {
            perror("Error stat-ing file");
        }

        if (S_ISDIR(f_d_stat.st_mode))
        {
            if (access(f_d, X_OK) == 0)
            {
                char input[1000];
                snprintf(input, sizeof(input), "warp %s", f_d);
                warp(home, previous_directory, input); 
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
        else if (S_ISREG(f_d_stat.st_mode))
        {
            if (access(f_d, R_OK) == 0)
            {
                int dir_len = strlen(target_dir);
                printf(GREEN_COLOR ".%s/%s\n" RESET_COLOR, target_dir + dir_len, f_d);
            }
            else
            {
                printf("Missing permissions for task!\n");
            }
        }
    }
    flag_e = 0;
}
