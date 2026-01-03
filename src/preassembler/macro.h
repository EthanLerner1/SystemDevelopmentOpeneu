#pragma once
#include "../status.h"

typedef struct Macro {
    char *name;
    char *body;
    size_t body_len;
    size_t body_cap;
} Macro;

void macro_free(Macro *m);

Status macro_init_name(Macro *m, const char *name);

Status macro_init_body(Macro *m, size_t initial_cap);

Status macro_append_line(Macro *m, const char *line_no_nl);
