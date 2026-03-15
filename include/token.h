#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

#define TOKEN_LEXEME_MAX 256

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

typedef struct Token {
    TokenType type;
    char lexeme[TOKEN_LEXEME_MAX];
    int line;
} Token;

Token token_make(TokenType type, const char *lexeme, int line);
const char *token_type_name(TokenType type);
const char *token_display_name(TokenType type);

#endif