#pragma once
#include "../status.h"
#include <stdint.h>
#include <stdio.h>

typedef enum FileAccess {
    FILE_ACCESS_READ,
    FILE_ACCESS_WRITE
} file_access_t;

typedef struct {
    int fd;
    const char *path;
    file_access_t access;
    uint32_t line_number;
} File;

Status file_open(File* file, const char *path, const file_access_t file_access);

Status file_close(File *file);

Status file_read_line(File *file, char *buf, uint32_t buf_size);

Status file_write_line(File *file, char *buf, uint32_t buf_size);

Status file_write_char(File *file, char c);
