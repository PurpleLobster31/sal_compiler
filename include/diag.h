#ifndef DIAG_H
#define DIAG_H

#include "token.h"

/*
 * Faz a inicialização do módulo de diagnosticos.
 * Se trace_enabled != 0, mensagens informativas serão emitidas através de diag_info().
 */
void diag_init(int trace_enabled);

/*
 * Faz a finalização do módulo de diagnosticos e libera os recursos alocados.
 */
void diag_close(void);

/*
 * Verifica se há erros registrados no módulo de diagnosticos.
 * Retorna 1 se houver erros, ou 0 caso contrário.
 */
int diag_has_error(void);

/*
 * Parte do módulo de diagnósticos que emite as mensagens.
 * Pode ser usada pelo parser para rastreamento.
 */
void diag_info(const char *fmt, ...);

/*
 * Emite um erro léxico padronizado, informando a linha e a mensagem. ALém disso, interrompe o processo via exit.
 */
void diag_lex_error(int line, const char *message);

/*
 * Emite um erro sintático padronizado, informando o token esperado, o token encontrado e a linha. Interrompe o processo via exit.
 */
void diag_syntax_error(TokenType expected, const Token *found);

/*
 * Emite um erro sintático padronizado, informando a descrição do que era esperado (ex: "comando", "tipo", "identificador ou vetor"), o token encontrado e a linha. 
 * Interrompe o processo via exit.
 */
void diag_syntax_error_expected(const char *expected_desc, const Token *found);

/*
 * Emite um erro de identificador não declarado, informando o nome do identificador e a linha onde foi referenciado. Interrompe o processo via exit.
 * Além disso, exibe o nome do identificador e a linha onde foi referenciado.
 */
void diag_undeclared_error(const char *name, int line);

#endif /* DIAG_H */