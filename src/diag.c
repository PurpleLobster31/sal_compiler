#include "diag.h"
#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static int g_trace_enabled = 0;
static int g_has_error = 0;

void diag_init(int trace_enabled) {
    g_trace_enabled = trace_enabled ? 1 : 0;
    g_has_error = 0;
}

void diag_close(void) {
    g_trace_enabled = 0;
}

int diag_has_error(void) {
    return g_has_error;
}

void diag_info(const char *fmt, ...) {
    va_list args;
    FILE *dest;

    if (!g_trace_enabled) {
        return;
    }

    /*
     * Se o log de rastreamento estiver aberto, escreve nele.
     * Caso contrario, escreve em stdout (modo sem arquivo).
     */
    dest = log_trace_file();
    if (dest == NULL) {
        dest = stdout;
    }

    va_start(args, fmt);
    vfprintf(dest, fmt, args);
    fprintf(dest, "\n");
    va_end(args);
}

void diag_lex_error(int line, const char *message) {
    g_has_error = 1;
    fprintf(stderr, "Erro lexico na linha %d: %s\n", line, message);
    exit(EXIT_FAILURE);
}

void diag_syntax_error(TokenType expected, const Token *found) {
    g_has_error = 1;

    if (found == NULL) {
        fprintf(stderr,
                "Erro sintatico: esperado %s, encontrado <nulo>\n",
                token_display_name(expected));
    } else {
        fprintf(stderr,
                "Erro sintatico na linha %d: esperado %s, encontrado %s (\"%s\")\n",
                found->line,
                token_display_name(expected),
                token_display_name(found->type),
                found->lexeme);
    }

    exit(EXIT_FAILURE);
}

void diag_syntax_error_expected(const char *expected_desc, const Token *found) {
    g_has_error = 1;

    if (found == NULL) {
        fprintf(stderr,
                "Erro sintatico: esperado %s, encontrado <nulo>\n",
                expected_desc);
    } else {
        fprintf(stderr,
                "Erro sintatico na linha %d: esperado %s, encontrado %s (\"%s\")\n",
                found->line,
                expected_desc,
                token_display_name(found->type),
                found->lexeme);
    }

    exit(EXIT_FAILURE);
}

void diag_undeclared_error(const char *name, int line) {
    g_has_error = 1;
    fprintf(stderr,
            "Erro semantico na linha %d: identificador '%s' nao foi declarado\n",
            line, name);
    exit(EXIT_FAILURE);
}