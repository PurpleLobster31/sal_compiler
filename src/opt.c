#include "opt.h"

#include <stdio.h>
#include <string.h>

static Options g_opts;

static void opts_reset(void) {
    g_opts.source_path = NULL;
    g_opts.emit_tokens = 0;
    g_opts.emit_symtab = 0;
    g_opts.emit_trace = 0;
}

int opts_parse(int argc, char **argv) {
    int i;

    opts_reset();

    if (argc < 2) {
        return 0;
    }

    g_opts.source_path = argv[1];

    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--tokens") == 0) {
            g_opts.emit_tokens = 1;
        } else if (strcmp(argv[i], "--symtab") == 0) {
            g_opts.emit_symtab = 1;
        } else if (strcmp(argv[i], "--trace") == 0) {
            g_opts.emit_trace = 1;
        } else {
            return 0;
        }
    }

    return 1;
}

const Options *opts_get(void) {
    return &g_opts;
}

void opts_print_usage(const char *progname) {
    fprintf(stderr, "Uso: %s <arquivo.sal> [--tokens] [--symtab] [--trace]\n",
            progname);
}