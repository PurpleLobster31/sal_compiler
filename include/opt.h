#ifndef OPT_H
#define OPT_H

/*
 * Essa struct representa as opções de execução do compilador, preenchidas a partir da linha de comando.
 * Os atributos são os seguintes:
 * - source_path : caminho do arquivo-fonte (.sal) informado na CLI.
 * - emit_tokens : indica se --tokens foi ativada.
 * - emit_symtab : indica se --symtab foi ativada.
 * - emit_trace  : indica se --trace foi ativada.
 */
typedef struct Options {
    const char *source_path;
    int emit_tokens;
    int emit_symtab;
    int emit_trace;
} Options;

/*
 * Faz o processamento da linha de comando, preenchendo a estrutura Options interna.
 * Retorna 1 em caso de sucesso, 0 em caso de argumento inválido ou ausente.
 */
int opts_parse(int argc, char **argv);

/*
 * Retorna um ponteiro para a estrutura Options preenchida por opts_parse().
 * Não deve ser chamada antes de opts_parse().
 */
const Options *opts_get(void);

/*
 * Imprime a mensagem de uso do compilador no stderr.
 */
void opts_print_usage(const char *progname);

#endif /* OPT_H */