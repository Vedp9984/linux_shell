#  Custom Shell - Mini Project 1

> GitHub Repository: [OSN-Monsoon-2024/mini-project-1-Vedp9984](https://github.com/OSN-Monsoon-2024/mini-project-1-Vedp9984)

##  Overview

This project involves building a custom Unix-like shell in **C**, implementing essential shell features, user-defined commands, signal handling, job control, and more. The shell mimics standard shell behavior while extending functionality through custom commands like `hop`, `reveal`, `seek`, `proclore`, and others.

---

##  Key Features

### 🔹 Dynamic Shell Prompt
- Displays the **username**, **hostname**, and **current working directory**.
- Supports **relative paths** using `~`.

### 🔹 Command Parsing
- Executes **multiple commands** separated by `;` and `&`.
- Ignores redundant **spaces and tabs** in user input.
- Supports **quoted strings** in arguments.

### 🔹 Custom Commands

| Command     | Description |
|-------------|-------------|
| `hop`       | Changes directory with support for `~`, `.`, `..`, and sequential navigation. |
| `reveal`    | Custom version of `ls`. Supports `-a`, `-l`, and default behaviors. |
| `log`       | Tracks last 15 commands; supports purging and re-execution. |
| `proclore`  | Displays process information for a given PID. |
| `seek`      | Finds files and directories with extension/type and permission filtering. |
| `activities`| Lists current shell-spawned processes in lexicographic order. |
| `ping`      | Sends a signal to a given process ID. |
| `fg`        | Moves a background process to the foreground. |
| `bg`        | Resumes a stopped background process. |
| `neonate`   | Monitors newly spawned processes at a given interval. |
| `iMan`      | Fetches online man pages using sockets from `http://man.he.net`. |

###  System Command Execution
- Executes all system-level commands (`vim`, `gedit`, etc.).
- Supports **foreground** and **background** execution using `&`.

###  Job Control
- Tracks and manages **background jobs** with job numbers.
- Supports `fg` and `bg` commands for control.

###  File and Process Utilities
- `reveal`: Mimics `ls` with `-a` and `-l` flags:
  - If only `a` appears (regardless of count), `reveal -a` is assumed.
  - If any `l` appears, defaults to `reveal -l`.
  - Invalid combinations like `-laa`, `-alll` default to valid behavior.
- `seek`: Can search files **with or without extensions**.

###  I/O Redirection
- Supports:
  - `>`  → output redirection
  - `>>` → append output
  - `<`  → input redirection
- Works **in combination** with pipes.

### Pipes Support
- Fully supports **command chaining** via pipes (`|`).
- Works with **any number** of pipes and redirections.

###  Signal Handling
- Handles key signals:
  - `SIGINT` (`Ctrl+C`): Kills the foreground process.
  - `SIGTSTP` (`Ctrl+Z`): Stops the foreground process and moves it to the background.
  - `SIGQUIT` (`Ctrl+D`): Terminates all jobs and exits shell.
- Custom `ping` command to send any signal (0-31) to a process.

###  Activities Tracker
- `activities`: Lists shell-spawned processes with PID and status (Running/Stopped).
- Handles invalid processes gracefully (e.g., `sleeep &`).

---

##  Configuration File: `.myshrc`

##  Configuration File: `.myshrc`

A startup configuration file for defining **aliases** and **functions**. Loaded at shell startup to allow user-specific customizations.

Example entries:
```bash
alias ll='reveal -l'
function mk_hop() {
   hop $1 && mkdir -p $1;
}
```

##  Additional Features

### 🔹 Input Handling
- Handles multiple commands separated by `;`
- Supports quoted strings in commands
- Trims whitespace from commands and arguments

### 🔹 Command Behavior Details

#### `reveal` Command
- Default behavior: Acts like `reveal -a` when no flags are provided
- Flag handling:
  - If any `l` appears in flags: Defaults to `reveal -l` (e.g., `-laa`, `-alll`)
  - If only `a` appears: Executes as `reveal -a` (e.g., `-aaa`)
  - Shows hidden files with `-a` and detailed listing with `-l`

#### `seek` Command
- Can search files with or without extensions
- Supports flags for filtering by type and permissions

### 🔹 I/O Redirection and Pipes
- Supports redirection with `>`, `>>`, and `<` operators
- Handles any number of pipes for command chaining
- Combines redirection with pipes for complex command sequences

### 🔹 Process Management Commands

#### `activities`
- Lists all shell-spawned processes in lexicographic order
- Shows command name, PID, and state (Running/Stopped)
- Handles invalid processes gracefully (e.g., `sleeep &`)

#### `neonate`
- Usage: `neonate -n <time_arg>`
- Monitors newly created processes by examining `/proc` directory
- Uses terminal attribute manipulation for real-time monitoring

#### `ping`
- Usage: `ping <pid> <signal_number>`
- Sends specified signal (0-31) to a given process

#### `iMan`
- Usage: `iMan <command_name>`
- Fetches man pages from http://man.he.net/ using sockets
- Outputs documentation directly to terminal

##  Command Usage Guide

1. `hop <path>` - Navigate directories
2. `reveal <flags> <path>` - List files and directories
3. `proclore <pid>` - Show process information
4. `seek <flags> <search> <target_directory>` - Find files/directories
5. `activities` - List running processes
6. `ping <pid> <signal_number>` - Send signal to process
7. `fg <pid>` - Move process to foreground
8. `bg <pid>` - Resume background process
9. `neonate -n <time_arg>` - Monitor new processes
10. `iMan <command_name>` - Get online manual pages
