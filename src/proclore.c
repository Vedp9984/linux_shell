#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "proclore.h"
#include <linux/limits.h>
#include <fcntl.h>
#include <errno.h>
#include "proclore.h"

void proclore(int pid)
{
    char path[PATH_MAX];
    char buffer[256];
    char status;
    char vm_size[64];
    char exe_path[PATH_MAX];

    if (pid == 0)
    {
        pid = getpid();
    }


    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *status_file = fopen(path, "r");
    if (!status_file)
    {
        perror("fopen");
        return;
    }

   
    while (fgets(buffer, sizeof(buffer), status_file))
    {
        if (strncmp(buffer, "State:", 6) == 0)
        {
            sscanf(buffer, "State:\t%c", &status);
        }
        else if (strncmp(buffer, "VmSize:", 7) == 0)
        {
            sscanf(buffer, "VmSize:\t%63s", vm_size);
        }
    }
    fclose(status_file);

    pid_t pgid = getpgid(pid);
    if (pgid == -1)
    {
        perror("getpgid");
        return;
    }
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, exe_path, sizeof(exe_path) - 1);
    if (len == -1)
    {
        perror("readlink");
        return;
    }
    exe_path[len] = '\0';

    printf("pid : %d\n", pid);
    printf("process status : %c", status);
    if(pid==getpid())
    {
        printf("+");
    }
    printf("\nProcess Group : %d\n", pgid);
    printf("Virtual memory : %s KB\n", vm_size);
    printf("executable path : %s\n", exe_path);
}
