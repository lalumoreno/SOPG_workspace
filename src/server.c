#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "file.h"

#define SERVER_PORT 5000
#define SUCCESS 0
#define ERROR -1
#define MAX 100

typedef enum
{
    SET,
    GET,
    DEL,
    NUM_COMMANDS,
    INVALID_COMMAND
} Command;

const char *command_names[] = {"SET", "GET", "DEL"};

typedef struct
{
    char command_name[MAX];
    Command command_enum;
    char key[MAX];
    char value[MAX];
} Command_str;

void printHelp()
{
    printf("\nUsage: \n");
    printf("SET <key> <value>\n");
    printf("GET <key>\n");
    printf("DEL <key>\n\n");
}

Command isValidCommand(char *input)
{
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        if (strcmp(input, command_names[i]) == 0)
        {
            return i;
        }
    }

    return INVALID_COMMAND;
}

bool parseCommand(char *str, Command_str *cmd)
{
    char temp[100];
    size_t len = strlen(str);
    str[len - 1] = '\0'; // Replace '\n' with '\0'

    strncpy(temp, str, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0'; // Ensure null termination

    char *token = strtok(temp, " ");
    if (token == NULL)
        return false; // No tokens found

    strncpy(cmd->command_name, token, sizeof(cmd->command_name) - 1);
    cmd->command_name[sizeof(cmd->command_name) - 1] = '\0';

    cmd->command_enum = isValidCommand(cmd->command_name);
    if (cmd->command_enum == INVALID_COMMAND)
        return false;

    token = strtok(NULL, " ");
    if (token == NULL)
        return false; // Less than 2 tokens

    strncpy(cmd->key, token, sizeof(cmd->key) - 1);
    cmd->key[sizeof(cmd->key) - 1] = '\0';

    // Check for extra tokens
    token = strtok(NULL, " ");

    switch (cmd->command_enum)
    {
    case SET:
        if (token == NULL)
        {
            printf("Less than 3 tokens...\n");
            return false; // Less than 3 tokens
        }

        strncpy(cmd->value, token, sizeof(cmd->value) - 1);
        cmd->value[sizeof(cmd->value) - 1] = '\0';
        break;

    default:
        if (token != NULL)
        {
            printf("More than 3 tokens...\n");
            return false; // More than 3 tokens, invalid input
        }

        break;
    }

    return true;
}

bool processCommand(Command_str *cmd)
{

    switch (cmd->command_enum)
    {
    case SET:
        return WriteFile(cmd->key, cmd->value);
        break;
    case GET:
        return ReadFile(cmd->key, cmd->value);
        break;
    case DEL:
        return DeleteFile(cmd->key);
        break;
    default:
        printf("Command_enum error. Invalid command\n");
        printf("Server exit...\n");
        return false;
        break;
    }

    return true;
}

/*
 * @param
 */
void chat(int fd)
{
    char buff[MAX];
    int n;
    bool running = true;

    // infinite loop for chat
    while (running)
    {
        bzero(buff, MAX);

        // read the message from client
        read(fd, buff, sizeof(buff));

        Command_str cmd;
        if (!parseCommand(buff, &cmd))
        {
            printf("Invalid command\n");
            printHelp();
            continue;
        }

        if (processCommand(&cmd))
        {
            // send buffer to client
            bzero(buff, MAX);
            strcpy(buff, "OK\n");
            write(fd, buff, sizeof(buff));

            if(cmd.command_enum == GET) {
                bzero(buff, MAX);
                strcpy(buff, cmd.value);
                write(fd, buff, sizeof(buff));
            }
        }
        else
        {
            // send buffer to client
            bzero(buff, MAX);
            strcpy(buff, "NOTFOUND\n");
            write(fd, buff, sizeof(buff));
        }
    }
}

int createSocket()
{
    int fd;
    struct sockaddr_in server_addr;

    // Create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        printf("socket creation failed...\n");
        return ERROR;
    }

    // Set IP and port for socket
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // local IP

    // Bind socket
    if ((bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) != SUCCESS)
    {
        printf("Socket bind failed...\n");
        return ERROR;
    }

    // Start listening
    if ((listen(fd, 5)) != SUCCESS)
    {
        printf("Listen failed...\n");
        return ERROR;
    }

    printf("Server listening...\n\n");
    return fd;
}

int main(void)
{
    int socket_fd;
    struct sockaddr_in server_addr, client;

    socket_fd = createSocket();

    if (socket_fd == ERROR)
    {
        printf("createSocket failed...\n");
        exit(0);
    }

    // Accept incoming client connection
    int len = sizeof(client);
    int connection_fd = accept(socket_fd, (struct sockaddr *)&client, &len);
    if (connection_fd < 0)
    {
        printf("Server accept failed...\n");
        exit(0);
    }
    else
    {
        printf("Server accept the client...\n\n");
    }

    // Chat between client and server
    chat(connection_fd);

    // Close socket
    close(socket_fd);
}