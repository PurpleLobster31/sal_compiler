#include "log.h"

#include "opt.h"
#include "symtab.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Arquivos de saida opcionais */
static FILE *g_token_file = NULL;
static FILE *g_symtab_file = NULL;
static FILE *g_trace_file = NULL;

/*
 * Deriva o caminho de saida trocando (ou adicionando) a extensao.
 * Ex: "prog.sal" + ".tk" => "prog.tk"
 */
static void log_make_path(const char *source_path,
                          const char *new_ext,
                          char *out,
                          size_t out_size) {
    const char *dot;
    size_t base_len;

    dot = strrchr(source_path, '.');
    if (dot != NULL) {
        base_len = (size_t)(dot - source_path);
    } else {
        base_len = strlen(source_path);
    }

    if (base_len >= out_size) {
        base_len = out_size - 1;
    }

    strncpy(out, source_path, base_len);
    out[base_len] = '\0';
    strncat(out, new_ext, out_size - base_len - 1);
}

int log_init(const char *source_path) {
    const Options *opts = opts_get();
    char path[512];

    /* Abre o arquivo de tokens se solicitado */
    if (opts->emit_tokens) {
        log_make_path(source_path, ".tk", path, sizeof(path));
        g_token_file = fopen(path, "w");
        if (g_token_file == NULL) {
            fprintf(stderr, "Erro: nao foi possivel criar '%s'\n", path);
            return 0;
        }
    }

    /* Abre o arquivo de tabela de simbolos se solicitado */
    if (opts->emit_symtab) {
        log_make_path(source_path, ".ts", path, sizeof(path));
        g_symtab_file = fopen(path, "w");
        if (g_symtab_file == NULL) {
            fprintf(stderr, "Erro: nao foi possivel criar '%s'\n", path);
            return 0;
        }
    }

    /* Abre o arquivo de rastreamento se solicitado */
    if (opts->emit_trace) {
        log_make_path(source_path, ".trc", path, sizeof(path));
        g_trace_file = fopen(path, "w");
        if (g_trace_file == NULL) {
            fprintf(stderr, "Erro: nao foi possivel criar '%s'\n", path);
            return 0;
        }
    }

    return 1;
}

void log_close(void) {
    if (g_token_file != NULL) {
        fclose(g_token_file);
        g_token_file = NULL;
    }
    if (g_symtab_file != NULL) {
        fclose(g_symtab_file);
        g_symtab_file = NULL;
    }
    if (g_trace_file != NULL) {
        fclose(g_trace_file);
        g_trace_file = NULL;
    }
}

void log_write_token(const Token *token) {
    if (g_token_file == NULL || token == NULL) {
        return;
    }

    /*
     * Formato: <linha>  <CATEGORIA>  "<lexema>"
     * Tokens internos de controle (EOF, ERRO) nao sao registrados.
     */
    if (token->type == sEOF || token->type == sERRO) {
        return;
    }

    fprintf(g_token_file, "%d  %s  \"%s\"\n",
            token->line,
            token_type_name(token->type),
            token->lexeme);
}

void log_write_symtab(void) {
    const Symbol *sym;

    if (g_symtab_file == NULL) {
        return;
    }

    /* formato da impressão: SCOPE=<descr>  id="<lex>"  cat=<cat>  tipo=<tipo>  extra=<val> */
    sym = ts_first_symbol();
    while (sym != NULL) {
        fprintf(g_symtab_file,
                "SCOPE=%s  id=\"%s\"  cat=%s  tipo=%s  extra=%d\n",
                sym->scope_name,
                sym->lexeme,
                ts_symbol_category_name(sym->category),
                ts_data_type_name(sym->type),
                sym->extra);
        sym = sym->next;
    }
}

FILE *log_trace_file(void) {
    return g_trace_file;
}