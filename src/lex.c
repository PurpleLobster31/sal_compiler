#include "lex.h"

#include <ctype.h>
#include <string.h>

static FILE *g_source = NULL;
static int g_line = 1;

static int g_has_pending_error = 0;
static char g_pending_error_msg[TOKEN_LEXEME_MAX];

static int lex_getc(void);
static int lex_peekc(void);
static void lex_skip_spaces_and_comments(void);

static Token lex_read_identifier_or_keyword(int first_char);
static Token lex_read_number(int first_char);
static Token lex_read_string(void);
static Token lex_read_char(void);

static TokenType lex_keyword_type(const char *lexeme);

static Token lex_error_token(const char *msg);

int lex_init(FILE *source) {
    if (source == NULL) {
        return 0;
    }

    g_source = source;
    g_line = 1;
    g_has_pending_error = 0;
    g_pending_error_msg[0] = '\0';
    return 1;
}

void lex_close(void) {
    g_source = NULL;
    g_line = 1;
    g_has_pending_error = 0;
    g_pending_error_msg[0] = '\0';
}

int lex_line(void) {
    return g_line;
}

Token lex_next(void) {
    int c;

    if (g_source == NULL) {
        return token_make(sERRO, "lexer nao inicializado", 0);
    }

    lex_skip_spaces_and_comments();

    c = lex_getc();

    /* lex_skip_spaces_and_comments pode detectar um erro (ex: bloco não fechado) sem ter um token para devolver na hora.
     * O erro fica pendente e e emitido no proximo token que o parser tentar consumir. */
    if (g_has_pending_error) {
        g_has_pending_error = 0;
        return token_make(sERRO, g_pending_error_msg, g_line);
    }

    if (c == EOF) {
        return token_make(sEOF, "", g_line);
    }

    if (isalpha(c) || c == '_') {
        return lex_read_identifier_or_keyword(c);
    }

    if (isdigit(c)) {
        return lex_read_number(c);
    }

    if (c == '"') {
        return lex_read_string();
    }

    if (c == '\'') {
        return lex_read_char();
    }

    switch (c) {
        case '(':
            return token_make(sABREPAR, "(", g_line);
        case ')':
            return token_make(sFECHAPAR, ")", g_line);
        case '[':
            return token_make(sABRECOL, "[", g_line);
        case ']':
            return token_make(sFECHACOL, "]", g_line);
        case ',':
            return token_make(sVIRG, ",", g_line);
        case ';':
            return token_make(sPVIRG, ";", g_line);

        case ':':
            if (lex_peekc() == '=') {
                lex_getc();
                return token_make(sATRIB, ":=", g_line);
            }
            return token_make(sDPTO, ":", g_line);

        case '.':
            if (lex_peekc() == '.') {
                lex_getc();
                return token_make(sPTOPTO, "..", g_line);
            }
            return lex_error_token("simbolo '.' invalido");

        case '=':
            if (lex_peekc() == '>') {
                lex_getc();
                return token_make(sIMPLIC, "=>", g_line);
            }
            return token_make(sIGUAL, "=", g_line);

        case '<':
            if (lex_peekc() == '=') {
                lex_getc();
                return token_make(sMENORIG, "<=", g_line);
            }
            if (lex_peekc() == '>') {
                lex_getc();
                return token_make(sDIFERENTE, "<>", g_line);
            }
            return token_make(sMENOR, "<", g_line);

        case '>':
            if (lex_peekc() == '=') {
                lex_getc();
                return token_make(sMAIORIG, ">=", g_line);
            }
            return token_make(sMAIOR, ">", g_line);

        case '+':
            return token_make(sSOMA, "+", g_line);
        case '-':
            return token_make(sSUBRAT, "-", g_line);
        case '*':
            return token_make(sMULT, "*", g_line);
        case '/':
            return token_make(sDIV, "/", g_line);
        case '^':
            return token_make(sAND, "^", g_line);
        case '~':
            return token_make(sNEG, "~", g_line);

        case 'v':
            /* 'v' é o operador OR da SAL. Como é uma letra, normalmente seria lido como identificador.
             * O caso especial em lex_keyword_type trata isso.
             * Este case só seria atingido se 'v' chegasse aqui sem passar pelo caminho de identificadores. */
            return token_make(sOR, "v", g_line);

        default:
            break;
    }

    return lex_error_token("caractere invalido");
}

// g_line é incrementado aqui para que qualquer erro reportado após lex_getc já reflita a linha correta, mesmo antes de processar o token.
static int lex_getc(void) {
    int c = fgetc(g_source);

    if (c == '\n') {
        g_line++;
    }

    return c;
}

static int lex_peekc(void) {
    int c = lex_getc();

    if (c != EOF) {
        ungetc(c, g_source);
        if (c == '\n') {
            g_line--;
        }
    }

    return c;
}

static void lex_skip_line_comment(void) {
    int c;

    while ((c = lex_getc()) != EOF) {
        if (c == '\n') {
            break;
        }
    }
}

static int lex_skip_block_comment(void) {
    int c;
    int prev = 0;

    while ((c = lex_getc()) != EOF) {
        if (prev == '}' && c == '@') {
            return 1;
        }
        prev = c;
    }

    return 0;
}

static void lex_skip_spaces_and_comments(void) {
    int c;

    while (1) {
        c = lex_peekc();

        if (c == EOF) {
            return;
        }

        if (isspace(c)) {
            lex_getc();
            continue;
        }

        if (c == '@') {
            lex_getc();

            if (lex_peekc() == '{') {
                lex_getc();
                if (!lex_skip_block_comment()) {
                    g_has_pending_error = 1;
                    strcpy(g_pending_error_msg, "comentario de bloco nao terminado");
                    return;
                }
            } else {
                lex_skip_line_comment();
            }
            continue;
        }

        return;
    }
}

static Token lex_read_identifier_or_keyword(int first_char) {
    char lexeme[TOKEN_LEXEME_MAX];
    size_t len = 0;
    int c;
    TokenType type;

    lexeme[len++] = (char)first_char;

    while ((c = lex_peekc()) != EOF) {
        if (isalnum(c) || c == '_') {
            if (len < TOKEN_LEXEME_MAX - 1) {
                lexeme[len++] = (char)lex_getc();
            } else {
                lex_getc();
            }
        } else {
            break;
        }
    }

    lexeme[len] = '\0';
    type = lex_keyword_type(lexeme);

    return token_make(type, lexeme, g_line);
}

static Token lex_read_number(int first_char) {
    char lexeme[TOKEN_LEXEME_MAX];
    size_t len = 0;
    int c;

    lexeme[len++] = (char)first_char;

    while ((c = lex_peekc()) != EOF && isdigit(c)) {
        if (len < TOKEN_LEXEME_MAX - 1) {
            lexeme[len++] = (char)lex_getc();
        } else {
            lex_getc();
        }
    }

    lexeme[len] = '\0';
    return token_make(sCTEINT, lexeme, g_line);
}

static Token lex_read_string(void) {
    char lexeme[TOKEN_LEXEME_MAX];
    size_t len = 0;
    int start_line = g_line;
    int c;

    lexeme[len++] = '"';

    while ((c = lex_getc()) != EOF) {
        if (c == '\n') {
            return token_make(sERRO, "string nao terminada", start_line);
        }

        if (len < TOKEN_LEXEME_MAX - 1) {
            lexeme[len++] = (char)c;
        }

        if (c == '"') {
            lexeme[len] = '\0';
            return token_make(sSTRING, lexeme, start_line);
        }
    }

    return token_make(sERRO, "string nao terminada", start_line);
}

/* A linguagem só aceita exatamente um caractere entre aspas simples.
 * Qualquer coisa além disso (múltiplos chars ou fechamento ausente) é erro. */
static Token lex_read_char(void) {
    char lexeme[TOKEN_LEXEME_MAX];
    size_t len = 0;
    int start_line = g_line;
    int c;

    lexeme[len++] = '\'';

    c = lex_getc();
    if (c == EOF || c == '\n') {
        return token_make(sERRO, "constante char invalida", start_line);
    }

    if (len < TOKEN_LEXEME_MAX - 1) {
        lexeme[len++] = (char)c;
    }

    c = lex_getc();
    if (c != '\'') {
        return token_make(sERRO, "constante char invalida", start_line);
    }

    if (len < TOKEN_LEXEME_MAX - 1) {
        lexeme[len++] = '\'';
    }

    lexeme[len] = '\0';
    return token_make(sCTECHAR, lexeme, start_line);
}

static TokenType lex_keyword_type(const char *lexeme) {
    if (strcmp(lexeme, "module") == 0)     return sMODULE;
    if (strcmp(lexeme, "globals") == 0)    return sGLOBALS;
    if (strcmp(lexeme, "locals") == 0)     return sLOCALS;
    if (strcmp(lexeme, "proc") == 0)       return sPROC;
    if (strcmp(lexeme, "fn") == 0)         return sFN;
    if (strcmp(lexeme, "main") == 0)       return sMAIN;
    if (strcmp(lexeme, "start") == 0)      return sSTART;
    if (strcmp(lexeme, "end") == 0)        return sEND;
    if (strcmp(lexeme, "int") == 0)        return sINT;
    if (strcmp(lexeme, "bool") == 0)       return sBOOL;
    if (strcmp(lexeme, "char") == 0)       return sCHAR;
    if (strcmp(lexeme, "if") == 0)         return sIF;
    if (strcmp(lexeme, "else") == 0)       return sELSE;
    if (strcmp(lexeme, "match") == 0)      return sMATCH;
    if (strcmp(lexeme, "when") == 0)       return sWHEN;
    if (strcmp(lexeme, "otherwise") == 0)  return sOTHERWISE;
    if (strcmp(lexeme, "for") == 0)        return sFOR;
    if (strcmp(lexeme, "to") == 0)         return sTO;
    if (strcmp(lexeme, "step") == 0)       return sSTEP;
    if (strcmp(lexeme, "do") == 0)         return sDO;
    if (strcmp(lexeme, "loop") == 0)       return sLOOP;
    if (strcmp(lexeme, "while") == 0)      return sWHILE;
    if (strcmp(lexeme, "until") == 0)      return sUNTIL;
    if (strcmp(lexeme, "print") == 0)      return sPRINT;
    if (strcmp(lexeme, "scan") == 0)       return sSCAN;
    if (strcmp(lexeme, "ret") == 0)        return sRETURN;
    if (strcmp(lexeme, "true") == 0)       return sTRUE;
    if (strcmp(lexeme, "false") == 0)      return sFALSE;

    /* Novamente, 'v' é o operador OR da SAL. Por ser uma letra minúscula isolada, élido como sequência alfanumérica normal e chega aqui para classificação.
     * Sem este caso, seria classificado como identificador. */
    if (strcmp(lexeme, "v") == 0)          return sOR;

    return sIDENTIF;
}

static Token lex_error_token(const char *msg) {
    return token_make(sERRO, msg, g_line);
}