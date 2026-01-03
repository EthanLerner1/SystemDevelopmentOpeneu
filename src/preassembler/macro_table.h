#pragma once
#include "macro.h"

typedef struct MacroTable {
    Macro *arr;
    size_t len;
    size_t cap;
} MacroTable;

Status macro_table_init(MacroTable *t);

Status macro_table_free(MacroTable *t);

Status macro_table_find(MacroTable *t,Macro** macro, const char *name);

Status macro_table_add(MacroTable *t, const char *name, Macro **out_macro);
