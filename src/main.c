#include "preassembler/preassembler.h"
#include "status.h"

#include <stdio.h>
#include <string.h>

static void make_output_path(const char *input, char *out, size_t out_size) {
    /* יוצר output עם סיומת .am במקום .as (או מוסיף .am אם אין .as) */
    size_t n = strlen(input);
    if (n >= 3 && strcmp(input + n - 3, ".as") == 0) {
        /* replace .as -> .am */
        if (n + 1 > out_size) {
            /* same length */
            out[0] = '\0';
            return;
        }
        strcpy(out, input);
        out[n - 1] = 'm'; /* ".as" -> ".am" (האות האחרונה) */
    } else {
        /* append .am */
        if (n + 4 + 1 > out_size) {
            /* ".am" + '\0' */
            out[0] = '\0';
            return;
        }
        strcpy(out, input);
        strcat(out, ".am");
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.as>\n", argv[0]);
        return 1;
    }

    const char *input_path = argv[1];

    char output_path[1024];
    make_output_path(input_path, output_path, sizeof(output_path));
    if (output_path[0] == '\0') {
        fprintf(stderr, "Error: output path too long\n");
        return 1;
    }

    Status st = preassemble_file(input_path, output_path);
    if (st != STATUS_SUCCESS) {
        fprintf(stderr, "Preassembler failed (status=%d)\n", (int) st);
        return 1;
    }

    printf("OK: wrote %s\n", output_path);
    return 0;
}
