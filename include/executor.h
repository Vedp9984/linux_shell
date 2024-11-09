#ifndef EXECUTOR_H
#define EXECUTOR_H
#include"activity.h"



int execute_command(char *command, int background,int *job_number);
void handle_background_processes();

#endif
