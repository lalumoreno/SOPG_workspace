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
    memset(text, 0, MAX); // Usamos memset en lugar de bzero
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        printf("File [%s] does not exist\n", filename);
        return false;
    }

    char buffer[MAX];
    memset(buffer, 0, MAX); // Inicializa el buffer

    ssize_t totalBytesRead = 0;
    ssize_t bytesRead = 0;

    // Lee el archivo en bloques
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (totalBytesRead + bytesRead >= MAX)
        {
            perror("Error: file size exceeds buffer capacity");
            close(fd); // Cerrar el archivo antes de retornar
            return false;
        }

        // Acumula los datos leídos en `text`
        memcpy(text + totalBytesRead, buffer, bytesRead);
        totalBytesRead += bytesRead;
    }

    if (bytesRead == -1)
    {
        perror("Error reading file");
        close(fd); // Cerrar el archivo antes de retornar
        return false;
    }

    // Asegúrate de que el texto esté correctamente terminado en null
    text[totalBytesRead] = '\0';

    printf("File [%s] read sucessfully\n", filename);

    if (close(fd) == -1)
    {
        perror("Error closing file");
        return false;
    }

    return true;
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
        printf("File [%s] does not exist\n", filename);
        return false;
    }

    return true;
}
