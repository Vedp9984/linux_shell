#ifndef ACTIVITY_H
#define ACTIVITY_H

typedef struct Process {
    int pid;
    char command[256];
    char state[16]; // "Running" or "Stopped"
    struct Process *next;
} Process;


Process *find_process_by_pid(int pid);
void add_process(int pid, const char *command, const char *state);
int update_process_info(Process *process);
void update_process_list();
void update_process_state(int pid, const char *state);
void print_activities();
void remove_process(int pid);

#endif
