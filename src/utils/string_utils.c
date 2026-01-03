#include "string_utils.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char EMPTY_ARRAY[] = "[]";
const char ARRAY_OPEN_BRACKET = '[';
const char ARRAY_CLOSE_BRACKET = ']';
const char ARRAY_SEPARATOR[] = ", ";
const int NULL_TERMINATOR_SIZE = 1;
const char NULL_TERMINATOR = '\0';
const char EOF_C = '\n';
const char LF_C = '\r';
const int EOF_SIZE = 1;

/*
 * Calculates the expected size for the buffer which will contain the string formating of the given int buffer
 *
 * out_buffer_size: pointer for calculated size
 * arr: the const int* array to be formatted
 * arr_len: arr size
 *
 * returns: Status of the function
 */
static Status calculate_int_arr_to_string_buffer_size(int *out_buffer_size, const int *arr, int arr_len) {
    int buffer_size;
    int chunk_size;
    int i;
    char tmp[32]; /* enough for any 32-bit int */

    buffer_size = (int) strlen(EMPTY_ARRAY);

    for (i = 0; i < arr_len; i++) {
        chunk_size = sprintf(tmp, "%d", arr[i]);
        if (chunk_size < 0) {
            fprintf(stderr, "Failed calculating current chunk size\n");
            return STATUS_SPRINTF_FAILED;
        }

        buffer_size += chunk_size;

        if (i < arr_len - 1) {
            buffer_size += (int) strlen(ARRAY_SEPARATOR);
        }
    }

    buffer_size += NULL_TERMINATOR_SIZE;

    *out_buffer_size = buffer_size;
    return STATUS_SUCCESS;
}

/*
 * Creates and returns and string representation for an empty array
 *
 * out_buffer: char** out param to the created buffer.
 *             Caller must free the returned string.
 *
 * returns: Status of the function
 */
static Status create_empty_array(char **out_buffer) {
    int buffer_size;

    buffer_size = strlen(EMPTY_ARRAY) + NULL_TERMINATOR_SIZE;
    *out_buffer = malloc(buffer_size);
    if (*out_buffer == NULL) {
        fprintf(stderr, "Failed allocating memory for empty buffer");
        return STATUS_ERROR_ALLOCATING_BUFFER;
    }
    strcpy(*out_buffer, EMPTY_ARRAY);
    return STATUS_SUCCESS;
}

/*
 * Safe version which have the same interface as sprintf but returns Status and checks for errors
 *
 */
static Status safe_sprintf(char **ptr, const char *format, ...) {
    va_list args;
    int written_chars;

    va_start(args, format);

    written_chars = vsprintf(*ptr, format, args);

    va_end(args);

    if (written_chars < 0) {
        fprintf(stderr, "Error detected while formatting string (invalid format?).\n");
        return STATUS_SPRINTF_FAILED;
    }

    *ptr += written_chars;

    return STATUS_SUCCESS;
}

Status int_arr_to_string(char **out_buffer, const int *arr, int arr_len) {
    char *result;
    char *ptr;
    int buffer_size;
    int i;
    Status status;

    if (arr_len <= 0) {
        return create_empty_array(out_buffer);
    }

    status = calculate_int_arr_to_string_buffer_size(&buffer_size, arr, arr_len);
    if (status != STATUS_SUCCESS) {
        return status;
    }

    result = malloc(buffer_size);
    if (result == NULL) {
        fprintf(stderr, "Failed allocating buffer for int array");
        return STATUS_ERROR_ALLOCATING_BUFFER;
    }

    ptr = result;
    status = safe_sprintf(&ptr, "%c", ARRAY_OPEN_BRACKET);
    if (status != STATUS_SUCCESS) {
        goto free_buffer_and_return_status;
    }

    for (i = 0; i < arr_len; i++) {
        if (i > 0) {
            status = safe_sprintf(&ptr, "%s", ARRAY_SEPARATOR);
            if (status != STATUS_SUCCESS) {
                goto free_buffer_and_return_status;
            }
        }
        status = safe_sprintf(&ptr, "%d", arr[i]);
        if (status != STATUS_SUCCESS) {
            goto free_buffer_and_return_status;
        }
    }

    status = safe_sprintf(&ptr, "%c", ARRAY_CLOSE_BRACKET);
    if (status != STATUS_SUCCESS) {
        goto free_buffer_and_return_status;
    }

    *out_buffer = result;
    return STATUS_SUCCESS;

free_buffer_and_return_status:
    free(result);
    return status;
}


char *str_dup(const char *s) {
    size_t n = strlen(s);
    char *p = (char *) malloc(n + 1);
    if (!p) {
        return NULL;
    }
    memcpy(p, s, n + 1);
    return p;
}


char *ltrim(char *s) {
    while (*s && isspace((unsigned char) *s)) s++;
    return s;
}

void rstrip_newline(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == EOF_C || s[n - 1] == LF_C)) {
        s[n - 1] = NULL_TERMINATOR;
        n--;
    }
}

/*parse two tokens seperated by whitespace*/
Boolean parse_two_tokens(char *line, char **tok1, char **tok2) {
    char *p = ltrim(line);
    if (*p == NULL_TERMINATOR) {
        *tok1 = NULL;
        *tok2 = NULL;
        return FALSE;
    }

    *tok1 = p;
    while (*p && !isspace((unsigned char) *p)) p++;
    if (*p) *p++ = NULL_TERMINATOR;

    p = ltrim(p);
    if (*p == NULL_TERMINATOR) {
        *tok2 = NULL;
        return TRUE;
    }

    *tok2 = p;
    while (*p && !isspace((unsigned char) *p)) p++;
    if (*p) *p = NULL_TERMINATOR;

    return TRUE;
}
