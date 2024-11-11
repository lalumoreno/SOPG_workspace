#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT     5000
#define SUCCESS         0

int main(void) {
    // Init
    int socket_fd;
    struct sockaddr_in server_addr, client;

    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
     if (socket_fd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } else {
        printf("Socket successfully created..\n"); 
    }
    
    // Set IP and port for socket
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //local IP

    // Bind socket 
    if ((bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))) != SUCCESS) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } else {
        printf("Socket successfully binded..\n"); 
    }

    // Start listening
    if ((listen(socket_fd, 5)) != SUCCESS) {
        printf("Listen failed...\n"); 
        exit(0); 
    } else {
        printf("Server listening...\n"); 
    }

    // Accept incoming client connection
    int len = sizeof(client); 
    int connection_fd = accept(socket_fd, (struct sockaddr*)&client, &len); 
    if (connection_fd < 0) { 
        printf("server accept failed...\n"); 
        exit(0); 
    } else {
        printf("server accept the client...\n");
    }
  
    // Function for chatting between client and server 
    //func(connfd); 
   sleep (3);
    // After chatting close the socket 
    close(socket_fd); 
}