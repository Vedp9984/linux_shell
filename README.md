# Custom Shell Project

## Overview

This project involves building a custom shell in C. The shell supports various functionalities, such as custom commands (`hop`, `reveal`, `log`, `proclore`, `seek`), handling background and foreground processes, executing system commands, and more. 

## Features Implemented

1. **Basic Command Execution**

   - The shell can execute standard system commands.
   - Supports background execution of commands using `&`.

2. **Custom Commands**

   - **`log`**: Handles logging of commands.
   - **`hop`**: Custom command functionality.
   - **`reveal`**: Reveals the content of files or directories.
   - **`proclore`**: Provides details about processes.
   - **`seek`**: Searches for files or directories based on specified flags.

3. **Input Handling**

   - Handles multiple commands separated by `;`.
   - Supports quoted strings in commands.
   - Trims whitespace from commands and arguments.

4. **Job Control**

   - Handles background and foreground processes.
   - Tracks job numbers for background processes.

# . in reveal if we command only reveal defalt extension is set to reveal -a.

-a for displaying hidden files and -l for displaying all details of files. It works exactly like ls command in linux terminal.
if there is any l in flag it will set to default as reveal -l
reveal -laa=reveal -l
reveal -alll =reveal -l
reveal -aaa= reveal -a
for reveal -a there should be only a in flag does not matter count

# . For 'seek' command, files can be searched with or without extensions.

## Part B

# #myshrc File

The myshrc file is a customized shell configuration file that includes useful aliases and functions to streamline the workflow and improve efficiency. It supports aliases and functions, including mk_hop and hop_seek.

## I/O Redirection

The shell supports I/O redirection using >, >>, and < operators. It can redirect output to a file, append to a file, and read input from a file.

# #Pipes

The shell supports pipes, which allow passing information between commands. It can handle any number of pipes and runs commands sequentially from left to right.


# Redirection with Pipes

The shell supports I/O redirection along with pipes, allowing for complex command sequences.

# Activities

The shell provides an activities command that lists all the processes currently running that were spawned by the shell in lexicographic order. The list includes the command name, pid, and state (running or stopped) of each process.
1. if an unknown activities like sleeep & then it will print that not valid one along with it store it in activities..
whose statues we  it can give running 

## nenoate

1. The system has a **'/proc'** directory that contains information about running processes.
2. The readdir function can be used to iterate through the /proc directory.
3. The opendir and closedir functions can be used to open and close the /proc directory.
4. The qsort function can be used to sort an array of integers.
5. The fork function can be used to create a new process.
6. The select function can be used to wait for input on the standard input file descriptor.
7. The tcgetattr and tcsetattr functions can be used to manipulate the terminal attributes.
   Usage

\*\* To use Neonate, simply compile the code and run the resulting executable with the following command:

neonate -n <time_arg>

## signals

# Signal Handler

1. The signal handler catches the following signals and performs the following actions:

   1. SIGINT (Ctrl-C): Kills the foreground process and resets the foreground process ID to -1.
   2. SIGTSTP (Ctrl-Z): Stops the foreground process, adds it to a list of background processes, and resets the
   3. foreground process ID to -1.
   4. SIGD (Ctrl-D): Logs out of the shell after killing all processes.

# Ping Command

-> . The ping command sends a signal to a specified process. The command takes two arguments: the process ID and the signal number.

# Usage

To use the ping command, simply type:

ping <pid> <signal_number>

Replace <pid> with the process ID and <signal_number> with the signal number (0-31).

## iman

 ## Description

The iMan command fetches man pages from the internet using sockets and outputs them to the terminal (stdout). It uses the website http://man.he.net/ to retrieve the man pages.
Usage

Replace <command_name> with the name of the man page you want to fetch.

# Directions for use

1. hop
   hop <path>
2. reveal
   reveal <flags> <path>
3. proclore
   proclore <pid>
4. seek
   seek <flags> <search> <target_directory>
5. activities
   activities
6. ping
   ping <pid> <signal_number>
7. fg
   fg <pid>
8. bg
   bg <pid>
9. neonate
   neonate -n [time_arg]
10. iMan
    iMan <command_name>
