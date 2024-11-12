#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file.h"

bool WriteFile(const char *filename, const char *text)
{
    // File descriptor
    int file;

    // Open the file with write-only and create flags, and set permissions to 0644
    file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // Check if the file was created successfully
    if (file == -1)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    // Write to the file
    ssize_t bytes_written = write(file, text, strlen(text));

    // Check if the write was successful
    if (bytes_written == -1)
    {
        perror("Error writing to file");
        close(file); // Close the file descriptor on error
        return EXIT_FAILURE;
    }

    // Close the file
    close(file);

    printf("File created and text written successfully.\n");
}

bool ReadFile(const char *filename)
{
    int fd = open(filename, O_RDONLY);

    if (fd == -1)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[4096];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (write(STDOUT_FILENO, buffer, bytesRead) != bytesRead)
        {
            perror("Error writing to stdout");
            exit(EXIT_FAILURE);
        }
    }

    if (bytesRead == -1)
    {
        perror("Error reading file");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1)
    {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
}
