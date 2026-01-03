#pragma once

#include "boolean.h"
#include "../status.h"
/* Constants for array string formatting */
extern const char EMPTY_ARRAY[];
extern const char ARRAY_OPEN_BRACKET;
extern const char ARRAY_CLOSE_BRACKET;
extern const char ARRAY_SEPARATOR[];
extern const int NULL_TERMINATOR_SIZE;
extern const char NULL_TERMINATOR;
extern const char EOF_C;
extern const char LF_C;
extern const int EOF_SIZE;


/*
 * Converts an integer array to a string in the format [x1, x2, ..., xn].
 *
 * out_buffer: char** out param to the created buffer.
 *             Caller must free the returned string.     
 * 
 * arr: pointer to integer array
 * arr_len: length of the array
 *
 * returns: Status stating weather an error accrued or not.
 *          if error accrued user doesn't need to free the allocated buffer
 */
Status int_arr_to_string(char **out_buffer, const int *arr, int arr_len);

char *str_dup(const char *s);

char *ltrim(char *s);

void rstrip_newline(char *s);

Boolean parse_two_tokens(char *line, char **tok1, char **tok2);
