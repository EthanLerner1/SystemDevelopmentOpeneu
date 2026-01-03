#include "buffer_utils.h"

#include <stdlib.h>
#include <string.h>

char *buf_dup(const char *src, size_t src_size) {
    char *dst;
    dst = malloc(src_size);
    if (dst == NULL) {
        return NULL;
    }

    memcpy(dst, src, src_size);
    return dst;
}
