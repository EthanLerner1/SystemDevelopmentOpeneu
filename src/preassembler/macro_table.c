#include "macro_table.h"

#include <stdlib.h>
#include <string.h>

#include "../utils/string_utils.h"


Status macro_table_init(MacroTable *t) {
    if (!t) {
        return STATUS_INVALID_ARGUMENTS;
    }
    t->arr = NULL;
    t->len = 0;
    t->cap = 0;
    return STATUS_SUCCESS;
}


Status macro_table_free(MacroTable *t) {
    size_t i;

    if (!t) {
        return STATUS_INVALID_ARGUMENTS;
    }
    for (i = 0; i < t->len; i++) {
        macro_free(&t->arr[i]);
    }
    free(t->arr);
    t->arr = NULL;
    t->len = 0;
    t->cap = 0;
    return STATUS_SUCCESS;
}

Status macro_table_find(MacroTable *t, Macro **macro, const char *name) {
    size_t i;

    if (!t || !name) {
        return STATUS_INVALID_ARGUMENTS;
    }
    for (i = 0; i < t->len; i++) {
        if (strcmp(t->arr[i].name, name) == 0) {
            *macro = &t->arr[i];
            return STATUS_SUCCESS;
        }
    }
    *macro = NULL;
    return STATUS_EOF;
}

static Status macro_table_check_can_add(MacroTable *t, const char *name) {
    Macro *existing = NULL;
    Status st;

    if (!t || !name) {
        return STATUS_INVALID_ARGUMENTS;
    }

    st = macro_table_find(t, &existing, name);
    if (st == STATUS_SUCCESS) {
        print_error("cannot create more than 1 macro with the same name: %s\n", name);
        return STATUS_ERROR_MACRO_WITH_SAME_NAME_ALREADY_EXISTS;
    }
    if (st != STATUS_EOF) {
        return st; /* real error from find */
    }

    return STATUS_SUCCESS;
}


static Status macro_table_ensure_capacity(MacroTable *t) {
    size_t new_cap;
    Macro *p;

    if (t->len < t->cap) {
        return STATUS_SUCCESS;
    }

    new_cap = (t->cap == 0) ? 8 : t->cap * 2;
    p = realloc(t->arr, new_cap * sizeof(*p));
    if (!p) {
        return STATUS_ERROR_REALLOC_FAILED;
    }

    t->arr = p;
    t->cap = new_cap;
    return STATUS_SUCCESS;
}


Status macro_table_add(MacroTable *t, const char *name, Macro **out_macro) {
    Status st;
    Macro *m;

    st = macro_table_check_can_add(t, name);
    if (st != STATUS_SUCCESS) {
        return st;
    }

    st = macro_table_ensure_capacity(t);
    if (st != STATUS_SUCCESS) {
        return st;
    }

    m = &t->arr[t->len];
    memset(m, 0, sizeof(*m));

    st = macro_init_name(m, name);
    if (st != STATUS_SUCCESS) {
        return st;
    }

    st = macro_init_body(m, 256);
    if (st != STATUS_SUCCESS) {
        macro_free(m);
        return st;
    }

    t->len++;

    if (out_macro) {
        *out_macro = m;
    }

    return STATUS_SUCCESS;
}
