#ifndef DIAG_H
#define DIAG_H

#include "token.h"

/*
 * Inicializa o modulo de diagnosticos.
 * Se trace_enabled != 0, mensagens informativas podem ser emitidas.
 */
void diag_init(int trace_enabled);

/*
 * Finaliza o modulo de diagnosticos.
 */
void diag_close(void);

/*
 * Retorna 1 se ja houve erro fatal; 0 caso contrario.
 */
int diag_has_error(void);

/*
 * Emite uma mensagem informativa.
 * Pode ser usada pelo parser para rastreamento.
 */
void diag_info(const char *fmt, ...);

/*
 * Erro lexico padronizado.
 * Interrompe o processo via exit.
 */
void diag_lex_error(int line, const char *message);

/*
 * Erro sintatico padronizado.
 * Informa token esperado, token encontrado e linha.
 * Interrompe o processo via exit.
 */
void diag_syntax_error(TokenType expected, const Token *found);

/*
 * Erro sintatico com texto customizado de esperado.
 * Util para casos como "comando", "tipo", "identificador ou vetor".
 */
void diag_syntax_error_expected(const char *expected_desc, const Token *found);

#endif /* DIAG_H */