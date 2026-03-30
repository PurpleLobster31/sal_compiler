#ifndef PARSER_H
#define PARSER_H

/*
 * Faz a análise sintática completa do programa-fonte, consumindo os tokens fornecidos pelo analisador léxico via lex_next(). 
 * Implementa o ASDRcorrespondente à gramática oficial da linguagem SAL.
 * Caso tenha erro léxico ou sintático, encerra o processo via diag.
 */
void parse_program(void);

#endif /* PARSER_H */