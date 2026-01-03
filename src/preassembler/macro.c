#include "macro.h"

#include <stdlib.h>
#include <string.h>

#include "../utils/string_utils.h"

void macro_free(Macro *m) {
    if (!m) {
        return;
    }
    free(m->name);
    free(m->body);
    m->name = NULL;
    m->body = NULL;
    m->body_len = 0;
    m->body_cap = 0;
}

Status macro_init_name(Macro* m, const char *name) {
    m->name = str_dup(name);
    if (!m->name) {
        return STATUS_ERROR_FAILED_COPYING_NAME;
    }
    return STATUS_SUCCESS;
}

Status macro_init_body(Macro *m, size_t initial_cap) {
    m->body = malloc(initial_cap);
    if (!m->body) {
        return STATUS_ERROR_ALLOCATING_BUFFER;
    }
    m->body_cap = initial_cap;
    m->body[0] = '\0';
    return STATUS_SUCCESS;
}


Status macro_append_line(Macro *m, const char *line_no_nl) {
    size_t line_len, add, needed;
    char *p;

    if (!m || !m->body || !line_no_nl) {
        return STATUS_INVALID_ARGUMENTS;
    }

    line_len = strlen(line_no_nl);
    add = line_len + EOF_SIZE;
    needed = m->body_len + add + NULL_TERMINATOR_SIZE;

    if (needed > m->body_cap) {
        size_t new_cap = m->body_cap;
        while (needed > new_cap) {
            new_cap *= 2;
        }

        p = realloc(m->body, new_cap);
        if (!p) {
            return STATUS_ERROR_REALLOC_FAILED;
        }
        m->body = p;
        m->body_cap = new_cap;
    }

    memcpy(m->body + m->body_len, line_no_nl, line_len);
    m->body_len += line_len;
    m->body[m->body_len++] = EOF_C;
    m->body[m->body_len] = NULL_TERMINATOR;

    return STATUS_SUCCESS;
}
