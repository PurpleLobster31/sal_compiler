#ifndef LOG_H
#define LOG_H

#include "token.h"
#include "symtab.h"

#include <stdio.h>

/*
 * Inicializa o modulo de log a partir do caminho do arquivo fonte.
 * Deriva os nomes dos arquivos de saida trocando a extensao.
 * Retorna 1 em caso de sucesso, 0 em caso de falha.
 */
int log_init(const char *source_path);

/*
 * Finaliza o modulo de log e fecha todos os arquivos abertos.
 */
void log_close(void);

/*
 * Registra um token na lista de tokens (arquivo .tk).
 * Deve ser chamado pelo parser a cada token consumido,
 * ou pelo lexico a cada token retornado, quando habilitado.
 */
void log_write_token(const Token *token);

/*
 * Escreve a tabela de simbolos consolidada no arquivo .ts.
 * Deve ser chamado apos a analise estar completa.
 */
void log_write_symtab(void);

/*
 * Retorna o FILE* do arquivo de rastreamento (.trc).
 * Retorna NULL se o trace nao estiver habilitado.
 * Usado pelo diag para escrever mensagens de rastreamento.
 */
FILE *log_trace_file(void);

#endif /* LOG_H */