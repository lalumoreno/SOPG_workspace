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

/**
 * @brief Check if user input is a valid command for the server
 * @param user input
 * @retval Command type
 */
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

/**
 * @brief Parse command to get key and value
 * @param user input and pointer to Command_str to save key and value
 * @retval True if command has valid tokens, false otherwise
 */
bool parseCommand(char *input, Command_str *cmd)
{
    size_t len = strlen(input);
    input[len - 1] = '\0'; // Replace '\n' with '\0'

    char *token = strtok(input, " "); // Split input by " "
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

    // Check for third token
    token = strtok(NULL, " ");

    switch (cmd->command_enum)
    {
    case SET:
        if (token == NULL)
        {
            printf("Invalid usage. Less than 3 tokens...\n");
            return false; // Less than 3 tokens
        }

        strncpy(cmd->value, token, sizeof(cmd->value) - 1);
        cmd->value[sizeof(cmd->value) - 1] = '\0';

        // Check for extra token
        token = strtok(NULL, " ");
        if (token != NULL)
        {
            printf("Invalid usage. More than 3 tokens...\n");
            return false; // More than 3 tokens.
        }
        break;

    default:
        if (token != NULL)
        {
            perror("Invalid usage. More than 2 tokens...");
            return false; // More than 3 tokens.
        }
        break;
    }

    return true;
}

/**
 * @brief Handle each command
 * @param pointer to Command_str
 * @retval True if command execution was successful, false otherwise
 */
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
        perror("Invalid command");
        return false;
        break;
    }

    return true;
}

/**
 * @brief Start chat between server and client connected
 * @param file descriptor to the client connection
 */
void chat(int fd)
{
    char buff[MAX];
    ssize_t bytesRead;

    bzero(buff, MAX);

    // read the message from client
    bytesRead = read(fd, buff, sizeof(buff));

    if (bytesRead == 0)
    {
        perror("Client disconnected...");
    }
    else if (bytesRead == -1)
    {
        perror("Error reading from client");
    }

    Command_str cmd;
    if (!parseCommand(buff, &cmd))
    {
        // send buffer to client
        bzero(buff, MAX);
        strcpy(buff, "Invalid command\n");
        write(fd, buff, strlen(buff));
    }
    else if (processCommand(&cmd))
    {
        // send buffer to client
        bzero(buff, MAX);
        strcpy(buff, "OK\n");
        write(fd, buff, strlen(buff));

        if (cmd.command_enum == GET)
        {
            // send buffer to client
            bzero(buff, MAX);
            strcpy(buff, cmd.value);
            buff[strlen(buff)] = '\n';
            write(fd, buff, strlen(buff));
        }
     }
     else
     {
        // send buffer to client
        bzero(buff, MAX);
        strcpy(buff, "NOTFOUND\n");
        write(fd, buff, strlen(buff));
    }
}

/**
 * @brief Create TCP socket for server
 * @retval SUCCESS if socket creation was successful, ERROR otherwise
 */
int createSocket()
{
    int fd;
    struct sockaddr_in server_addr;

    // Create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket creation failed...");
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
        perror("Socket bind failed...");
        return ERROR;
    }

    // Start listening
    if ((listen(fd, 5)) != SUCCESS)
    {
        perror("Listen failed...");
        return ERROR;
    }

    printf("Server listening...\n\n");
    return fd;
}

/**
 * @brief Main process: launch server and listen for TCP connections
 */
int main(void)
{
    int socket_fd;
    struct sockaddr_in client;

    socket_fd = createSocket();

    if (socket_fd == ERROR)
    {
        perror("createSocket failed...");
        exit(0);
    }

    while (true)
    {

        // Accept incoming client connection
        int len = sizeof(client);
        int connection_fd = accept(socket_fd, (struct sockaddr *)&client, (socklen_t *)&len);
        if (connection_fd < 0)
        {
            perror("Server accept failed...");
            break;
        }

        printf("Client connected...\n");
        chat(connection_fd); // Chat between client and server
        
        printf("Client disconnected...\n");
        close(connection_fd);
    }

    // Close socket
    close(socket_fd);
}