#ifndef LEX_H
#define LEX_H

#include <stdio.h>

#include "token.h"

/*
 * Faz a inicialização do módulo léxico, preparando-o para ler do arquivo-fonte fornecido (o arquivo já deve estar aberto).
 * Retorna 1 em caso de sucesso, 0 em caso de falha.
 */
int lex_init(FILE *source);

/*
 * Faz a finalização do módulo léxico, liberando os recursos alocados.
 */
void lex_close(void);

/*
 * Faz a devolução do próximo token do arquivo-fonte, e avança a leitura sequencialmente a cada chamada.
 */
Token lex_next(void);

/*
 * Retorna a linha atual de leitura do arquivo-fonte.
 */
int lex_line(void);

#endif /* LEX_H */