#ifndef OPT_H
#define OPT_H

typedef struct Options {
    const char *source_path;
    int emit_tokens;
    int emit_symtab;
    int emit_trace;
} Options;

/*
 * Processa a linha de comando.
 * Retorna 1 em caso de sucesso, 0 em caso de erro.
 */
int opts_parse(int argc, char **argv);

/*
 * Retorna as opcoes atualmente carregadas.
 */
const Options *opts_get(void);

/*
 * Imprime mensagem de uso.
 */
void opts_print_usage(const char *progname);

#endif /* OPT_H */