#ifndef LOG_H
#define LOG_H

#include "token.h"
#include "symtab.h"

#include <stdio.h>

/*
 * Faz a inicialização do módulo de log a partir do caminho do arquivo-fonte, derivando os nomes dos arquivos de saída pela troca de extensão.
 * Retorna 1 em caso de sucesso, 0 em caso de falha.
 */
int log_init(const char *source_path);

/*
 * Faz a finalização do módulo de log e fecha todos os arquivos abertos.
 */
void log_close(void);

/*
 * Registra um token consumido no arquivo de tokens (.tk), se a opção --tokens estiver ativa.
 * Deve ser chamada pelo parser a cada chamada a parser_advance().
 */
void log_write_token(const Token *token);

/*
 * Escreve a tabela de símbolos consolidada no arquivo (.ts), se a opção --symtab estiver ativa.
 * Deve ser chamada após a conclusão da análise.
 */
void log_write_symtab(void);

/*
 * Retorna o FILE* do arquivo de rastreamento (.trc), se a opção --trace estiver ativa.
 * Retorna NULL se o rastreamento não estiver habilitado.
 */
FILE *log_trace_file(void);

#endif /* LOG_H */