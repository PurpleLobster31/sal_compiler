#include "diag.h"
#include "lex.h"
#include "log.h"
#include "opt.h"
#include "parser.h"
#include "symtab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    const Options *opts;
    FILE *source;

    /* 1. Processa os argumentos de linha de comando */
    if (!opts_parse(argc, argv)) {
        opts_print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    opts = opts_get();

    /* 2. Abre o arquivo fonte */
    source = fopen(opts->source_path, "r");
    if (source == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo '%s'\n",
                opts->source_path);
        return EXIT_FAILURE;
    }

    /* 3. Inicializa os modulos */
    diag_init(opts->emit_trace);
    ts_init();

    if (!lex_init(source)) {
        fprintf(stderr, "Erro: falha ao inicializar o analisador lexico\n");
        fclose(source);
        ts_close();
        diag_close();
        return EXIT_FAILURE;
    }

    /* log_init depois do lex_init, pois deriva o nome do arquivo fonte */
    if (!log_init(opts->source_path)) {
        fclose(source);
        lex_close();
        ts_close();
        diag_close();
        return EXIT_FAILURE;
    }

    /* 4. Executa a analise sintatica (que aciona o lexico e a tabela de simbolos) */
    parse_program();

    /* 5. Gera os logs opcionais */
    if (opts->emit_symtab) {
        log_write_symtab();
    }
    /* tokens sao escritos em tempo real por parser_advance — nada a fazer aqui */

    /* 6. Encerra os modulos de forma ordenada */
    log_close();
    lex_close();
    ts_close();
    diag_close();
    fclose(source);

    if (!diag_has_error()) {
        printf("Analise concluida com sucesso.\n");
    }

    return diag_has_error() ? EXIT_FAILURE : EXIT_SUCCESS;
}