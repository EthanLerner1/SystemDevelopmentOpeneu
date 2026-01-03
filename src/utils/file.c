#include "file.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "string_utils.h"

Status file_open(File *file, const char *path, const file_access_t file_access) {
    int flags;
    mode_t perms = 0;

    if (!file || !path) {
        return STATUS_INVALID_ARGUMENTS;
    }

    file->path = path;
    file->line_number = 0;
    file->access = file_access;
    file->fd = 0;

    if (file_access == FILE_ACCESS_READ) {
        static mode_t PERMISSIONS_IGNORE = 0;
        flags = O_RDONLY;
        perms = PERMISSIONS_IGNORE;
    } else {
        flags = O_WRONLY | O_CREAT | O_TRUNC;
        perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    }

    file->fd = open(path, flags, perms);
    if (file->fd == -1)
        return STATUS_ERROR_OPENING_FILE;

    return STATUS_SUCCESS;
}

Status file_close(File *file) {
    if (!file) {
        return STATUS_ERROR_CALLED_CLOSE_ON_CLOSED_FILE;
    }
    const int close_result = close(file->fd);
    if (close_result != 0) {
        return STATUS_ERROR_CLOSE_FAILED;
    }

    file->fd = 0;
    return STATUS_SUCCESS;
}

Status file_read_line(File *file, char *buf, const uint32_t buf_size) {
    size_t i = 0;
    char c;
    ssize_t n;

    if (!file || !buf || buf_size == 0 || file->access != FILE_ACCESS_READ) {
        return STATUS_INVALID_ARGUMENTS;
    }

    while (i + 1 < buf_size) {
        n = read(file->fd, &c, 1);
        if (n == 0) {
            /* EOF */
            if (i == 0)
                return STATUS_EOF;
            break;
        }
        if (n < 0) {
            return STATUS_ERROR_READ_FAILED;
        }

        buf[i++] = c;
        if (c == EOF_C) {
            buf[i] = NULL_TERMINATOR;
            file->line_number++;
            return STATUS_SUCCESS;
        }
    }

    return STATUS_ERROR_BUFFER_TOO_SMALL;
}

Status file_write_line(File *file, char *buf, uint32_t buf_size) {
    size_t len;
    size_t total_written = 0;

    if (!file || !buf || buf_size == 0 || file->access != FILE_ACCESS_WRITE) {
        return STATUS_INVALID_ARGUMENTS;
    }

    len = strlen(buf);

    /* Write the buffer (excluding NUL terminator) */
    while (total_written < len) {
        ssize_t n = write(
            file->fd,
            buf + total_written,
            len - total_written
        );
        if (n < 0) {
            return STATUS_ERROR_WRITE_FAILED; /* or STATUS_ERROR */
        }
        total_written += (size_t) n;
    }

    /* Append newline if missing */
    if (len == 0 || buf[len - 1] != EOF_C) {
        char nl = EOF_C;
        ssize_t n = write(file->fd, &nl, 1);
        if (n != 1) {
            return STATUS_ERROR_WRITE_FAILED; /* or STATUS_ERROR */
        }
    }

    file->line_number++;
    return STATUS_SUCCESS;
}

Status file_write_char(File *file, const char c) {
    static const int WRITE_SINGLE_CHAR = 1;
    if (write(file->fd, &c, WRITE_SINGLE_CHAR) != WRITE_SINGLE_CHAR) {
        return STATUS_ERROR_WRITE_FAILED;
    }
    return STATUS_SUCCESS;
}
