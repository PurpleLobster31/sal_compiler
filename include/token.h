#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

#define TOKEN_LEXEME_MAX 256

/*
 * Esse enum representa todas as categorias de token reconhecidas pelo analisador léxico.
 * Valores iniciados com "s" seguem a convenção da especificação da linguagem SAL.
 *
 * Os tipos possíveis são os seguintes:
 * - Categorias lexicais  : sIDENTIF, sCTEINT, sCTECHAR, sSTRING
 * - Palavras reservadas  : sMODULE, sGLOBALS, sLOCALS, sPROC, sFN, sMAIN,
 *                        sSTART, sEND, sINT, sBOOL, sCHAR, sIF, sELSE,
 *                        sMATCH, sWHEN, sOTHERWISE, sFOR, sTO, sSTEP, sDO,
 *                        sLOOP, sWHILE, sUNTIL, sPRINT, sSCAN, sRETURN,
 *                        sTRUE, sFALSE
 * - Operadores           : sATRIB, sSOMA, sSUBRAT, sMULT, sDIV, sIGUAL,
 *                        sDIFERENTE, sMAIOR, sMENOR, sMAIORIG, sMENORIG,
 *                        sAND, sOR, sNEG, sIMPLIC, sPTOPTO
 * - Delimitadores        : sABREPAR, sFECHAPAR, sABRECOL, sFECHACOL,
 *                        sVIRG, sDPTO, sPVIRG
 * - Meta                 : sEOF, sERRO
 *
 * Importante: os delimitadores (sABREPAR, sFECHAPAR, sABRECOL, sFECHACOL, sVIRG, sDPTO, sPVIRG) não possuem categoria nomeada na especificação SAL;
 * Esses foram criados internamente para melhor controle e diagnóstico de erros.
 */
typedef enum TokenType {
    sEOF = 0,
    sERRO,

    sIDENTIF,
    sCTEINT,
    sCTECHAR,
    sSTRING,

    sMODULE,
    sGLOBALS,
    sLOCALS,
    sPROC,
    sFN,
    sMAIN,
    sSTART,
    sEND,
    sINT,
    sBOOL,
    sCHAR,
    sIF,
    sELSE,
    sMATCH,
    sWHEN,
    sOTHERWISE,
    sFOR,
    sTO,
    sSTEP,
    sDO,
    sLOOP,
    sWHILE,
    sUNTIL,
    sPRINT,
    sSCAN,
    sRETURN,
    sTRUE,
    sFALSE,

    sATRIB,
    sSOMA,
    sSUBRAT,
    sMULT,
    sDIV,

    sIGUAL,
    sDIFERENTE,
    sMAIOR,
    sMENOR,
    sMAIORIG,
    sMENORIG,

    sAND,
    sOR,
    sNEG,

    sIMPLIC,
    sPTOPTO,

    sABREPAR,
    sFECHAPAR,
    sABRECOL,
    sFECHACOL,
    sVIRG,
    sDPTO,
    sPVIRG
} TokenType;

/*
 * Essa struct representa um token retornado pelo analisador léxico.
 *
 * Os atributos são os seguintes:
 * - type   : categoria do token, conforme TokenType.
 * - lexeme : texto original extraído do arquivo-fonte (até TOKEN_LEXEME_MAX - 1 caracteres).
 * - line   : número da linha do arquivo-fonte onde o token foi encontrado.
 */
typedef struct Token {
    TokenType type;
    char lexeme[TOKEN_LEXEME_MAX];
    int line;
} Token;

/*
 * Cria e retorna um Token com o tipo, lexema e linha fornecidos.
 * Os parâmetros são os seguintes:
 * - type   : categoria do token, conforme TokenType.
 * - lexeme : texto original extraído do arquivo-fonte (até TOKEN_LEXEME_MAX - 1 caracteres).
 * - line   : número da linha do arquivo-fonte onde o token foi encontrado.
 */
Token token_make(TokenType type, const char *lexeme, int line);

/*
 * Retorna o nome interno da categoria do token (ex: "sIDENTIF", "sPVIRG").
 * Usado na geração do arquivo de tokens (.tk).
 */
const char *token_type_name(TokenType type);

/*
 * Retorna o nome legível da categoria do token (ex: "identificador", ";").
 * Usado nas mensagens de erro emitidas pelo módulo de diagnósticos.
 */
const char *token_display_name(TokenType type);

#endif /* TOKEN_H */