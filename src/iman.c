#include "header.h"

// Function to resolve hostname to IP address
int resolve_hostname(const char *hostname, char *ip_address)
{
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, NULL, &hints, &res) != 0)
    {
        fprintf(stderr, "Error: Incorrect hostname\n");
        return -1;
    }

    inet_ntop(res->ai_family, &((struct sockaddr_in *)res->ai_addr)->sin_addr, ip_address, INET_ADDRSTRLEN);
    freeaddrinfo(res);
    return 0;
}

// Function to establish a connection to the server
int connect_to_server(const char *ip_address, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Error creating socket");
        return -1;
    }

    struct sockaddr_in serv_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)};
    inet_pton(AF_INET, ip_address, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("Error connecting to server");
        close(sock);
        return -1;
    }

    return sock;
}

// Function to send an HTTP GET request
int send_http_request(int sock, const char *command)
{
    char request[1024];
    snprintf(request, sizeof(request), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", command);
    return send(sock, request, strlen(request), 0) == -1 ? perror("send"), -1 : 0;
}

// Function to receive the HTTP response
int receive_http_response(int sock, char *contents, size_t max_size)
{
    char response_buffer[4096];
    int bytes_received, total_received = 0;

    while ((bytes_received = recv(sock, response_buffer, sizeof(response_buffer) - 1, 0)) > 0)
    {
        response_buffer[bytes_received] = '\0';
        if (total_received + bytes_received < (int)max_size)
        {
            strncat(contents, response_buffer, bytes_received);
            total_received += bytes_received;
        }
        else
            break;
    }

    return bytes_received < 0 ? perror("recv"), -1 : total_received;
}

// Function to extract and print relevant content
void print_relevant_content(const char *content)
{
    const char *start_pos = strstr(content, "NAME"), *end_pos = strstr(content, "Man Pages Copyright");
    if (!start_pos || !end_pos)
    {
        printf("No such command\n");
        return;
    }

    int inside_tag = 0;
    for (const char *ptr = start_pos; ptr < end_pos; ptr++)
    {
        if (*ptr == '<')
            inside_tag = 1;
        else if (*ptr == '>')
            inside_tag = 0;
        else if (!inside_tag)
        {
            if (*ptr == '&')
            {
                if (strncmp(ptr, "&amp;", 5) == 0)
                    putchar('&'), ptr += 4;
                else if (strncmp(ptr, "&quot;", 6) == 0)
                    putchar('"'), ptr += 5;
                else if (strncmp(ptr, "&lt;", 4) == 0)
                    putchar('<'), ptr += 3;
                else if (strncmp(ptr, "&apos;", 6) == 0)
                    putchar('\''), ptr += 5;
                else if (strncmp(ptr, "&gt;", 4) == 0)
                    putchar('>'), ptr += 3;
                else
                    putchar(*ptr);
            }
            else
                putchar(*ptr);
        }
    }
    putchar('\n');
}

// Main function to handle the command
int iman_final(char *command)
{
    char ip_address[INET_ADDRSTRLEN];
    if (resolve_hostname("man.he.net", ip_address) == -1)
        return 1;

    int sock = connect_to_server(ip_address, 80);
    if (sock == -1)
        return 1;

    if (send_http_request(sock, command) == -1)
        return close(sock), 1;

    char contents[50000] = {0};
    if (receive_http_response(sock, contents, sizeof(contents)) == -1)
        return close(sock), 1;
    close(sock);

    print_relevant_content(contents);

    return 0;
}

int handle_iMan(char *command)
{
    char *cmd = strtok(command, " ");
    if (!cmd)
        return fprintf(stderr, "Error: Invalid command format\n"), 1;
    command = strtok(NULL, " ");
    return command ? iman_final(command) : fprintf(stderr, "Kindly provide one more argument.\n"), 1;
}
