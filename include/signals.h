#ifndef SIGNAL_H
#define SIGNAL_H
#include"activity.h"

extern Process *head;


void sig_c(int signum);
void handle_sigstop(int sig);
void child_handler(int sig);
void handle_ping_command(char *cmd);
void ping_command(int pid, int signal);

#endif 
