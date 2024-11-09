#ifndef LOG_H
#define LOG_H

void log_command(const char *cmd);
void purge_log();
void handle_log_command(const char *command);
void display_log();
void execute_log(int index);
int is_valid_command(const char *cmd);
void init_log();
#endif
