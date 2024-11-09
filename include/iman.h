#ifndef IMAN_H
#define IMAN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Function prototypes

/**
 * Resolves the IP address of a given hostname.
 *
 * @param hostname The hostname to resolve.
 * @return A string containing the IPv4 address of the hostname. The caller is responsible for freeing the allocated memory.
 */


/**
 * Strips HTML tags from a given text.
 *
 * @param text The text to process. The input string is modified in place to remove HTML tags.
 */


/**
 * Handles the `iMan` command to fetch and display man pages from the internet.
 *
 * @param cmd The command string containing the man page request. Only the first argument is considered.
 */
int handle_iMan(char *cmd);

#endif // IMAN_H
