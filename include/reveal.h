#ifndef REVEAL_H
#define REVEAL_H
#include <sys/stat.h>

void print_details(struct stat *file_stat, char *name);
void print_permissions(mode_t mode);
void reveal(char *flags, char *path);

#endif
