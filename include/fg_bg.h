#ifndef FG_BG_H
#define FG_BG_H
#include"signals.h"


void fg(pid_t pid);
void bg(pid_t pid);
void handle_fg_bg(char *command);

#endif // FG_BG_H
