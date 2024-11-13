#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "file.h"

#define MAX 100

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
        return false;
    }
    // Write to the file
    ssize_t bytes_written = write(file, text, strlen(text));

    // Check if the write was successful
    if (bytes_written == -1)
    {
        perror("Error writing to file");
        close(file); // Close the file descriptor on error
        return false;
    }

    // Close the file
    close(file);

    printf("File [%s] created and text written successfully.\n", filename);
}

bool ReadFile(const char *filename, char *text)
{
    int fd = open(filename, O_RDONLY);

    if (fd == -1)
    {
        perror("Error opening file");
        return false;
    }

    char buffer[MAX];
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (bytesRead > MAX) {
            perror("Error: file size exceeds buffer capacity");
            return false;
        }
    }

    if (bytesRead == -1)
    {
        perror("Error reading file");
        return false;
    } else {
        printf("REad %s",text);
        memcpy(text, buffer, bytesRead);  // Append to outputBuffer
    }

    if (close(fd) == -1)
    {
        perror("Error closing file ");
        return false;
    }
}

bool DeleteFile(const char *filename)
{
    if (remove(filename) == 0)
    {
        printf("File [%s] deleted successfully.\n", filename);
    }
    else
    {
        perror("Error deleting file");
        return false;
    }

    return true;
}
