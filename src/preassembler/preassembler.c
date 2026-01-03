#include "preassembler.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"
#include "macro_table.h"

#include "../instructions.h"
#include "../utils/file.h"
#include "../config.h"
#include "../utils/string_utils.h"


static Boolean is_macro_invocation_line(const char *line_no_nl, const char *tok1) {
    const char *p;

    p = line_no_nl;

    if (strcmp(tok1, STOP_INSTRUCTION) == 0) {
        return FALSE;
    }

    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    p += strlen(tok1);

    while (*p && isspace((unsigned char)*p)) {
        p++;
    }

    if (*p == NULL_TERMINATOR) {
        return TRUE;
    }

    return FALSE;
}

/*
 * Tokenize line into at most 2 tokens.
 * - Writes a modified copy of 'line' into parse_buf
 * - tok1/tok2 point INTO parse_buf (do not free)
 */
static Boolean tokenize_line(const char *line,
                             char *parse_buf,
                             uint32_t parse_buf_size,
                             char **tok1,
                             char **tok2) {
    strncpy(parse_buf, line, (size_t) parse_buf_size - 1);
    parse_buf[parse_buf_size - 1] = NULL_TERMINATOR;

    return parse_two_tokens(parse_buf, tok1, tok2);
}

static Status write_empty_line(File *out) {
    return file_write_char(out, EOF_C);
}

static Boolean is_mcro_keyword(const char *tok1) {
    if (tok1 == NULL) {
        return FALSE;
    }

    if (strcmp(tok1, MACRO_INSTRUCTION) == 0) {
        return TRUE;
    }

    return FALSE;
}

static int is_mcroend_keyword_alone(const char *tok1, const char *tok2) {
    if (tok1 == NULL) {
        return 0;
    }

    if (strcmp(tok1, MACRO_END_INSTRUCTION) != 0) {
        return 0;
    }

    if (tok2 != NULL) {
        return 0;
    }

    return 1;
}

static Status read_macro_body(File *in, Macro *cur, char *buf, uint32_t buf_cap) {
    Status st;
    int found_end;

    found_end = 0;

    while ((st = file_read_line(in, buf, buf_cap)) == STATUS_SUCCESS) {
        char parse_buf[DEFAULT_MAX_LINE_LEN * 2];
        char *t1;
        char *t2;
        Boolean ht;

        t1 = NULL;
        t2 = NULL;

        rstrip_newline(buf);

        ht = tokenize_line(buf,
                           parse_buf,
                           (uint32_t) sizeof(parse_buf),
                           &t1,
                           &t2);

        if (ht) {
            if (is_mcroend_keyword_alone(t1, t2)) {
                found_end = 1;
                break;
            }
        }

        st = macro_append_line(cur, buf);
        if (st != STATUS_SUCCESS) {
            return st;
        }
    }

    if (st != STATUS_SUCCESS) {
        if (st != STATUS_EOF) {
            return st;
        }
    }

    if (!found_end) {
        return STATUS_ERROR_FILE_ENDED_BEFORE_CLOSING_MACRO;
    }

    return STATUS_SUCCESS;
}

static Status handle_macro_definition(File *in,
                                      MacroTable *macros,
                                      char *buf,
                                      uint32_t buf_cap,
                                      const char *macro_name) {
    Status st;
    Macro *cur;

    cur = NULL;

    if (macro_name == NULL) {
        return STATUS_ERROR_MACRO_DEFINED_WITHOUT_A_NAME;
    }

    st = macro_table_add(macros, macro_name, &cur);
    if (st != STATUS_SUCCESS) {
        return st;
    }

    st = read_macro_body(in, cur, buf, buf_cap);
    return st;
}

static Status handle_macro_invocation(File *out,
                                      MacroTable *macros,
                                      const char *invoked_name) {
    Status st;
    Macro *m;

    m = NULL;

    st = macro_table_find(macros, &m, invoked_name);

    if (st == STATUS_EOF) {
        return STATUS_ERROR_USAGE_OF_MACRO_BEFORE_DEFINITION;
    }
    if (m == NULL) {
        return STATUS_ERROR_USAGE_OF_MACRO_BEFORE_DEFINITION;
    }
    if (st != STATUS_SUCCESS) {
        return st;
    }

    return file_write_line(out, m->body, (uint32_t) m->body_cap);
}

/* Processes a single already-read line (buf is mutable and has no trailing newline). */
static Status process_line(File *in,
                           File *out,
                           MacroTable *macros,
                           char *buf,
                           uint32_t buf_size) {
    char parse_buf[DEFAULT_MAX_LINE_LEN * 2];
    char *tok1;
    char *tok2;
    Boolean has_tok;

    tok1 = NULL;
    tok2 = NULL;

    has_tok = tokenize_line(buf,
                            parse_buf,
                            (uint32_t) sizeof(parse_buf),
                            &tok1,
                            &tok2);

    if (!has_tok) {
        return write_empty_line(out);
    }

    if (strcmp(tok1, MACRO_END_INSTRUCTION) == 0) {
        return STATUS_ERROR_MACROEND_OUTSIDE_DEFINITION;
    }

    if (is_mcro_keyword(tok1)) {
        return handle_macro_definition(in, macros, buf, buf_size, tok2);
    }

    if (is_macro_invocation_line(buf, tok1)) {
        return handle_macro_invocation(out, macros, tok1);
    }

    return file_write_line(out, buf, buf_size);
}


Status preassemble_file(const char *input_path, const char *output_path) {
    Status st;
    File in;
    File out;
    MacroTable macros;
    char *buf;

    buf = NULL;
    memset(&in, 0, sizeof(in));
    memset(&out, 0, sizeof(out));

    macro_table_init(&macros);

    st = file_open(&in, input_path, FILE_ACCESS_READ);
    if (st != STATUS_SUCCESS) {
        macro_table_free(&macros);
        return st;
    }

    st = file_open(&out, output_path, FILE_ACCESS_WRITE);
    if (st != STATUS_SUCCESS) {
        file_close(&in);
        macro_table_free(&macros);
        return st;
    }

    buf = malloc(DEFAULT_MAX_LINE_LEN);
    if (buf == NULL) {
        file_close(&out);
        file_close(&in);
        macro_table_free(&macros);
        return STATUS_ERROR_ALLOCATING_BUFFER;
    }

    while ((st = file_read_line(&in, buf, DEFAULT_MAX_LINE_LEN)) == STATUS_SUCCESS) {
        rstrip_newline(buf);

        st = process_line(&in, &out, &macros, buf, DEFAULT_MAX_LINE_LEN);
        if (st != STATUS_SUCCESS) {
            break;
        }
    }

    if (st == STATUS_EOF) {
        st = STATUS_SUCCESS;
    }

    file_close(&out);
    file_close(&in);
    macro_table_free(&macros);
    free(buf);

    return st;
}
