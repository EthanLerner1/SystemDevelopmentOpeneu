#include <stdio.h>
#include <stdlib.h>

#include "file.h"

int main() {
    printf("Hello world!\n");
    File f;

    Status status = file_open(&f, "/tmp/file.txt", FILE_ACCESS_READ);
    if (status != STATUS_SUCCESS) {
        printf("Error opening file! %d\n", status);
        return EXIT_FAILURE;
    }
    char buffer[80];
    status = file_read_line(&f, buffer, 80);
    if (status != STATUS_SUCCESS) {
        printf("Error reading file! %d\n", status);
        file_close(&f);
        return EXIT_FAILURE;
    }
    printf("line: %s", buffer);
    return EXIT_SUCCESS;
}
