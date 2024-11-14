#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "file.h"

#define MAX 100

/**
 * @brief Create and write text in file
 * @param file name and text to write
 * @retval True if success, false otherwise
 */
bool WriteFile(const char *filename, const char *text)
{
    int file;
    file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (file == -1)
    {
        perror("Error opening file");
        return false;
    }
    // Write to the file
    ssize_t bytes_written = write(file, text, strlen(text));
    if (bytes_written == -1)
    {
        perror("Error writing to file");
        close(file); // Close the file on error
        return false;
    }

    close(file);

    printf("File [%s] created and text written successfully.\n", filename);
}

/**
 * @brief Read file
 * @param file name and porinter to save text
 * @retval True if success, false otherwise
 */
bool ReadFile(const char *filename, char *text)
{
    char buffer[MAX];
    memset(buffer, 0, MAX);
    memset(text, 0, MAX);

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        printf("File [%s] does not exist\n", filename);
        return false;
    }

    ssize_t totalBytesRead = 0;
    ssize_t bytesRead = 0;

    // Lee el archivo en bloques
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (totalBytesRead + bytesRead >= MAX)
        {
            perror("Error: file size exceeds buffer capacity");
            close(fd);
            return false;
        }

        memcpy(text + totalBytesRead, buffer, bytesRead);
        totalBytesRead += bytesRead;
    }

    if (bytesRead == -1)
    {
        perror("Error reading file");
        close(fd);
        return false;
    }

    // Add '\0' termination
    text[totalBytesRead] = '\0';

    printf("File [%s] read sucessfully\n", filename);

    if (close(fd) == -1)
    {
        perror("Error closing file");
        return false;
    }

    return true;
}

/**
 * @brief Delete file
 * @param file name
 * @retval True if success, false otherwise
 */
bool DeleteFile(const char *filename)
{
    if (remove(filename) == 0)
    {
        printf("File [%s] deleted successfully.\n", filename);
    }
    else
    {
        perror("Error deleting file");
        printf("File [%s] does not exist\n", filename);
        return false;
    }

    return true;
}
