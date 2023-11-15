#include "include/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //ssize_t

char *tac_read_file(const char *filename)
{
    FILE *fp;

    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Could not read file '%s'\n", filename);
        exit(1);
    }

    size_t strlen_buffer = 0;
    char *buffer = (char *)calloc(1, sizeof(char));
    // buffer[0] = '\0';

    size_t strlen_line = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        // printf("line: %s, len: %ld, strlen(line): %ld\n", line, len, strlen(line));
        // printf("buffer: %s, strlen(buffer): %ld\n", buffer, strlen(buffer));
        strlen_line = strlen(line);
        buffer = (char *)realloc(buffer, (strlen_buffer + strlen_line + 1) * sizeof(char));
        strcat(buffer, line);
        strlen_buffer += strlen_line;
    }
    // printf("buffer: %s, strlen(buffer): %ld\n", buffer, strlen(buffer));

    fclose(fp);
    if (line)
        free(line);

    return buffer;
}