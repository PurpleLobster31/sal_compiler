#ifndef LEX_H
#define LEX_H

#include <stdio.h>

#include "token.h"

/*
 * Inicializa o analisador léxico com o arquivo-fonte já aberto.
 * Retorna 1 em caso de sucesso, 0 em caso de falha.
 */
int lex_init(FILE *source);

/*
 * Finaliza o módulo léxico e limpa seu estado interno.
 */
void lex_close(void);

/*
 * Devolve o próximo token do arquivo-fonte.
 * Cada chamada avança a leitura sequencialmente.
 */
Token lex_next(void);

/*
 * Retorna a linha atual de leitura do fonte.
 * Útil para diagnósticos e rastreamento.
 */
int lex_line(void);

#endif /* LEX_H */