#include "parser.h"

#include "diag.h"
#include "lex.h"
#include "symtab.h"
#include "token.h"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static Token g_current;
static Token g_lookahead;
static int g_has_lookahead = 0;
static int g_block_counter = 0;

/* controle basico */
static void parser_advance(void);
static void parser_expect(TokenType type);
static int parser_accept(TokenType type);

/* utilidades */
static int parser_is_relop(TokenType type);
static int parser_command_starts(TokenType type);
static int parser_elem_starts(TokenType type);
static int parser_expr_starts(TokenType type);

static void parser_make_block_scope_name(char *out, size_t out_size);

/* gramatica principal */
static void parse_ini(void);
static void parse_glob(void);
static void parse_decls(void);
static void parse_func(void);
static void parse_proc(void);
static void parse_princ(void);
static void parse_param_list(int *param_count);
static void parse_bco(int create_scope, int allow_locals);
static void parse_cmd(void);

/* comandos */
static void parse_out(void);
static void parse_inp(void);
static void parse_if(void);
static void parse_mat(void);
static void parse_wlst(void);
static void parse_wh_after_loop(void);
static void parse_othr(void);
static void parse_wcnd(void);
static void parse_witem(void);
static void parse_wint(void);
static void parse_fr(void);
static void parse_rpt_after_loop(void);
static void parse_atr(void);
static void parse_ret(void);
static void parse_whn(void);

/* declaracoes / tipos */
static DataType parse_tpo(void);
static void parse_decl_item_names(char names[][256], int extras[], int *count);

/* expressoes */
static void parse_expr(void);
static void parse_exlog(void);
static void parse_exrel(void);
static void parse_exari(void);
static void parse_exarp(void);
static void parse_fact(void);
static void parse_primary(void);

/* elementos auxiliares */
static void parse_elem(void);
static void parse_id_only(char *out_name, int out_size);
static void parse_id_or_vec_name(char *out_name, int out_size, int *is_vec_use);
static void parse_call_after_name(const char *name);
static void parse_vec_after_name(const char *name);

/* semantica leve / ts */
static void parser_require_declared_identifier(const char *name);
static void parser_insert_decl_group(char names[][256],
                                     int extras[],
                                     int count,
                                     DataType type,
                                     int line);

void parse_program(void) {
    g_has_lookahead = 0;
    parser_advance();
    parse_ini();

    if (g_current.type != sEOF) {
        diag_syntax_error_expected("fim de arquivo", &g_current);
    }
}

static void parser_advance(void) {
    if (g_has_lookahead) {
        g_current = g_lookahead;
        g_has_lookahead = 0;
    } else {
        g_current = lex_next();
    }

    if (g_current.type == sERRO) {
        diag_lex_error(g_current.line, g_current.lexeme);
    }

    /* Registra o token no log de tokens, se habilitado */
    log_write_token(&g_current);
}

static TokenType parser_peek(void) {
    if (!g_has_lookahead) {
        g_lookahead = lex_next();
        if (g_lookahead.type == sERRO) {
            diag_lex_error(g_lookahead.line, g_lookahead.lexeme);
        }
        g_has_lookahead = 1;
    }
    return g_lookahead.type;
}

static void parser_expect(TokenType type) {
    if (g_current.type != type) {
        diag_syntax_error(type, &g_current);
    }
    parser_advance();
}

static int parser_accept(TokenType type) {
    if (g_current.type == type) {
        parser_advance();
        return 1;
    }
    return 0;
}

static int parser_is_relop(TokenType type) {
    return type == sMAIOR || type == sMAIORIG || type == sIGUAL ||
           type == sMENOR || type == sMENORIG || type == sDIFERENTE;
}

static int parser_command_starts(TokenType type) {
    return type == sPRINT || type == sSCAN || type == sIF ||
           type == sMATCH || type == sFOR || type == sLOOP ||
           type == sIDENTIF || type == sRETURN || type == sSTART;
}

static int parser_elem_starts(TokenType type) {
    return type == sSTRING || type == sCTEINT || type == sCTECHAR ||
           type == sIDENTIF || type == sABREPAR || type == sNEG ||
           type == sSUBRAT || type == sTRUE || type == sFALSE;
}

static int parser_expr_starts(TokenType type) {
    return parser_elem_starts(type);
}

static void parser_make_block_scope_name(char *out, size_t out_size) {
    const char *base = ts_current_scope_name();
    g_block_counter++;
    snprintf(out, out_size, "%s.block#%d", base, g_block_counter);
}

static void parse_ini(void) {
    char module_name[256];

    diag_info("enter <ini>");

    parser_expect(sMODULE);
    parse_id_only(module_name, (int)sizeof(module_name));
    parser_expect(sPVIRG);

    ts_enter_scope("global");

    if (g_current.type == sGLOBALS) {
        parse_glob();
    }

    /*
     * Sub-rotinas opcionais: fn's e proc's que nao sejam o main.
     * Usamos parser_peek() para distinguir "proc main" de "proc outroNome"
     * sem quebrar o estado do lexico.
     */
    while (g_current.type == sFN ||
           (g_current.type == sPROC && parser_peek() != sMAIN)) {
        if (g_current.type == sFN) {
            parse_func();
        } else {
            parse_proc();
        }
    }

    parse_princ();

    ts_leave_scope();
    diag_info("exit <ini>");
}

static void parse_glob(void) {
    diag_info("enter <glob>");
    parser_expect(sGLOBALS);

    while (g_current.type == sIDENTIF) {
        parse_decls();
    }

    diag_info("exit <glob>");
}

static void parse_decls(void) {
    char names[64][256];
    int extras[64];
    int count = 0;
    DataType type;
    int line = g_current.line;

    diag_info("enter <decls>");

    parse_decl_item_names(names, extras, &count);
    parser_expect(sDPTO);
    type = parse_tpo();
    parser_expect(sPVIRG);

    parser_insert_decl_group(names, extras, count, type, line);

    diag_info("exit <decls>");
}

static void parse_decl_item_names(char names[][256], int extras[], int *count) {
    char name[256];
    int is_vec = 0;
    int extra = 0;

    parse_id_only(name, 256);

    if (parser_accept(sABRECOL)) {
        if (g_current.type != sCTEINT) {
            diag_syntax_error(sCTEINT, &g_current);
        }
        extra = atoi(g_current.lexeme);
        parser_advance();
        parser_expect(sFECHACOL);
        is_vec = 1;
    }

    strcpy(names[*count], name);
    extras[*count] = is_vec ? extra : 0;
    (*count)++;

    while (parser_accept(sVIRG)) {
        parse_id_only(name, 256);
        is_vec = 0;
        extra = 0;

        if (parser_accept(sABRECOL)) {
            if (g_current.type != sCTEINT) {
                diag_syntax_error(sCTEINT, &g_current);
            }
            extra = atoi(g_current.lexeme);
            parser_advance();
            parser_expect(sFECHACOL);
            is_vec = 1;
        }

        strcpy(names[*count], name);
        extras[*count] = is_vec ? extra : 0;
        (*count)++;
    }
}

static DataType parse_tpo(void) {
    DataType type;

    if (g_current.type == sINT) {
        parser_advance();
        type = TYPE_INT;
    } else if (g_current.type == sBOOL) {
        parser_advance();
        type = TYPE_BOOL;
    } else if (g_current.type == sCHAR) {
        parser_advance();
        type = TYPE_CHAR;
    } else {
        diag_syntax_error_expected("tipo", &g_current);
        return TYPE_UNDEF;
    }

    return type;
}

static void parser_insert_decl_group(char names[][256],
                                     int extras[],
                                     int count,
                                     DataType type,
                                     int line) {
    int i;

    for (i = 0; i < count; i++) {
        SymbolCategory cat = (extras[i] > 0) ? SYM_VECTOR : SYM_VAR;
        if (!ts_insert(names[i], cat, type, extras[i], line)) {
            diag_syntax_error_expected("identificador nao duplicado no escopo atual", &g_current);
        }
    }
}

static void parse_func(void) {
    char fn_name[256];
    char scope_name[264];
    DataType ret_type;
    int param_count = 0;
    int line = g_current.line;

    diag_info("enter <func>");

    parser_expect(sFN);
    parse_id_only(fn_name, 256);

    /* Insere provisoriamente com TYPE_UNDEF; sera atualizado apos parsear o tipo */
    if (!ts_insert(fn_name, SYM_FUNC, TYPE_UNDEF, 0, line)) {
        diag_syntax_error_expected("nome de funcao nao duplicado", &g_current);
    }

    snprintf(scope_name, sizeof(scope_name), "fn:%s", fn_name);
    ts_enter_scope(scope_name);

    parser_expect(sABREPAR);
    if (g_current.type == sIDENTIF) {
        parse_param_list(&param_count);
    }
    parser_expect(sFECHAPAR);
    parser_expect(sDPTO);

    ret_type = parse_tpo();

    /* Atualiza o tipo de retorno e a quantidade de parametros na tabela */
    ts_update_type(fn_name, ret_type);
    ts_update_extra(fn_name, param_count);

    parse_bco(0, 1);
    ts_leave_scope();

    diag_info("exit <func>");
}

static void parse_proc(void) {
    char proc_name[256];
    char scope_name[264];
    int param_count = 0;
    int line = g_current.line;

    diag_info("enter <proc>");

    parser_expect(sPROC);
    parse_id_only(proc_name, 256);

    if (!ts_insert(proc_name, SYM_PROC, TYPE_VOID, 0, line)) {
        diag_syntax_error_expected("nome de procedimento nao duplicado", &g_current);
    }

    snprintf(scope_name, sizeof(scope_name), "proc:%s", proc_name);
    ts_enter_scope(scope_name);

    parser_expect(sABREPAR);
    if (g_current.type == sIDENTIF) {
        parse_param_list(&param_count);
    }
    parser_expect(sFECHAPAR);

    parse_bco(0, 1);

    ts_leave_scope();
    diag_info("exit <proc>");
    (void)param_count;
}

static void parse_princ(void) {
    diag_info("enter <princ>");

    parser_expect(sPROC);
    parser_expect(sMAIN);
    parser_expect(sABREPAR);
    parser_expect(sFECHAPAR);

    /* Registra o main na tabela de simbolos como procedimento */
    ts_insert("main", SYM_PROC, TYPE_VOID, 0, g_current.line);

    ts_enter_scope("proc:main");
    parse_bco(0, 1);
    ts_leave_scope();

    diag_info("exit <princ>");
}

static void parse_param_list(int *param_count) {
    char name[256];
    DataType type;

    diag_info("enter <param>");

    parse_id_only(name, 256);
    parser_expect(sDPTO);
    type = parse_tpo();

    if (!ts_insert(name, SYM_PARAM, type, 0, g_current.line)) {
        diag_syntax_error_expected("parametro nao duplicado", &g_current);
    }
    (*param_count)++;

    while (parser_accept(sVIRG)) {
        parse_id_only(name, 256);
        parser_expect(sDPTO);
        type = parse_tpo();

        if (!ts_insert(name, SYM_PARAM, type, 0, g_current.line)) {
            diag_syntax_error_expected("parametro nao duplicado", &g_current);
        }
        (*param_count)++;
    }

    diag_info("exit <param>");
}

static void parse_bco(int create_scope, int allow_locals) {
    char scope_name[128];

    diag_info("enter <bco>");

    if (create_scope) {
        parser_make_block_scope_name(scope_name, sizeof(scope_name));
        ts_enter_scope(scope_name);
    }

    if (allow_locals && g_current.type == sLOCALS) {
        parser_advance();   /* consome 'locals' */
        while (g_current.type == sIDENTIF) {
            parse_decls();
        }
    }

    parser_expect(sSTART);

    while (parser_command_starts(g_current.type)) {
        parse_cmd();
        parser_expect(sPVIRG);
    }

    parser_expect(sEND);

    if (create_scope) {
        ts_leave_scope();
    }

    diag_info("exit <bco>");
}

static void parse_cmd(void) {
    switch (g_current.type) {
        case sPRINT:  parse_out(); break;
        case sSCAN:   parse_inp(); break;
        case sIF:     parse_if(); break;
        case sMATCH:  parse_mat(); break;
        case sFOR:    parse_fr(); break;
        case sLOOP:
            parser_expect(sLOOP);

            if (g_current.type == sWHILE) {
                parse_wh_after_loop();
            } else {
                parse_rpt_after_loop();
            }
            break;
        case sIDENTIF: {
            Token ident = g_current;
            parser_advance();

            if (g_current.type == sABREPAR) {
                parse_call_after_name(ident.lexeme);
            } else {
                if (g_current.type == sABRECOL || g_current.type == sATRIB) {
                    if (g_current.type == sABRECOL) {
                        parse_vec_after_name(ident.lexeme);
                    }
                    parser_expect(sATRIB);
                    parse_elem();
                } else {
                    diag_syntax_error_expected("chamada ou atribuicao", &g_current);
                }
            }
            break;
        }
        case sRETURN: parse_ret(); break;
        case sSTART:  
            parse_bco(1, 0);
            break;
        default:
            diag_syntax_error_expected("comando", &g_current);
            break;
    }
}

static void parse_out(void) {
    parser_expect(sPRINT);
    parser_expect(sABREPAR);
    parse_elem();
    while (parser_accept(sVIRG)) {
        parse_elem();
    }
    parser_expect(sFECHAPAR);
}

static void parse_inp(void) {
    char name[256];
    int is_vec = 0;

    parser_expect(sSCAN);
    parser_expect(sABREPAR);
    parse_id_or_vec_name(name, 256, &is_vec);
    parser_expect(sFECHAPAR);

    parser_require_declared_identifier(name);
    (void)is_vec;
}

static void parse_if(void) {
    diag_info("enter <if>");          
    parser_expect(sIF);
    parser_expect(sABREPAR);
    parse_expr();
    parser_expect(sFECHAPAR);
    parse_cmd();
    if (parser_accept(sELSE)) {
        parse_cmd();
    }
    diag_info("exit <if>");           
}

static void parse_mat(void) {
    diag_info("enter <match>");       
    parser_expect(sMATCH);
    parser_expect(sABREPAR);
    parse_expr();
    parser_expect(sFECHAPAR);
    parse_wlst();
    parser_expect(sEND);
    diag_info("exit <match>");        
}

static void parse_wlst(void) {
    if (g_current.type != sWHEN) {
        diag_syntax_error(sWHEN, &g_current);
    }

    while (g_current.type == sWHEN) {
        parse_whn();
    }

    if (g_current.type == sOTHERWISE) {
        parse_othr();
    }
}

static void parse_whn(void) {
    parser_expect(sWHEN);
    parse_wcnd();
    parser_expect(sIMPLIC);
    parse_cmd();
    parser_expect(sPVIRG);
}

static void parse_othr(void) {
    parser_expect(sOTHERWISE);
    parser_expect(sIMPLIC);
    parse_cmd();
    parser_expect(sPVIRG);
}

static void parse_wcnd(void) {
    parse_witem();
    while (parser_accept(sVIRG)) {
        parse_witem();
    }
}

static void parse_witem(void) {
    parse_wint();
    if (parser_accept(sPTOPTO)) {
        parse_wint();
    }
}

static void parse_wint(void) {
    parser_accept(sSUBRAT);
    parser_expect(sCTEINT);
}

static void parse_fr(void) {
    diag_info("enter <for>");         
    parser_expect(sFOR);
    parse_atr();
    if (g_current.type != sTO) {
        diag_syntax_error_expected("to", &g_current);
    }
    parser_advance();
    parse_elem();
    if (g_current.type == sSTEP) {
        parser_advance();
        if (g_current.type == sIDENTIF) {
            parser_require_declared_identifier(g_current.lexeme);
            parser_advance();
        } else if (g_current.type == sCTEINT) {
            parser_advance();
        } else {
            diag_syntax_error_expected("identificador ou constante inteira", &g_current);
        }
    }
    if (g_current.type != sDO) {
        diag_syntax_error_expected("do", &g_current);
    }
    parser_advance();
    parse_cmd();
    diag_info("exit <for>");          
}

static void parse_wh_after_loop(void) {
    diag_info("enter <loop while>");  
    parser_expect(sWHILE);
    parser_expect(sABREPAR);
    parse_expr();
    parser_expect(sFECHAPAR);
    parse_cmd();
    diag_info("exit <loop while>");   
}

static void parse_rpt_after_loop(void) {
    diag_info("enter <loop until>");  
    while (parser_command_starts(g_current.type)) {
        parse_cmd();
        parser_expect(sPVIRG);
    }
    parser_expect(sUNTIL);
    parser_expect(sABREPAR);
    parse_expr();
    parser_expect(sFECHAPAR);
    diag_info("exit <loop until>");   
}

static void parse_atr(void) {
    diag_info("enter <atr>");         
    char name[256];
    int is_vec = 0;

    parse_id_or_vec_name(name, 256, &is_vec);
    parser_require_declared_identifier(name);
    parser_expect(sATRIB);
    parse_elem();
    (void)is_vec;
    diag_info("exit <atr>");          
}

static void parse_ret(void) {
    parser_expect(sRETURN);
    parse_elem();
}

static void parse_expr(void) {
    parse_exlog();
    while (parser_accept(sOR)) {
        parse_exlog();
    }
}

static void parse_exlog(void) {
    parse_exrel();
    while (parser_accept(sAND)) {
        parse_exrel();
    }
}

static void parse_exrel(void) {
    parse_exari();
    while (parser_is_relop(g_current.type)) {
        parser_advance();
        parse_exari();
    }
}

static void parse_exari(void) {
    parse_exarp();
    while (g_current.type == sSOMA || g_current.type == sSUBRAT) {
        parser_advance();
        parse_exarp();
    }
}

static void parse_exarp(void) {
    parse_fact();
    while (g_current.type == sMULT || g_current.type == sDIV) {
        parser_advance();
        parse_fact();
    }
}

static void parse_fact(void) {
    if (parser_accept(sNEG)) {
        parse_fact();
        return;
    }

    if (parser_accept(sSUBRAT)) {
        parse_fact();
        return;
    }

    if (parser_accept(sABREPAR)) {
        parse_expr();
        parser_expect(sFECHAPAR);
        return;
    }

    parse_primary();
}

static void parse_primary(void) {
    char name[256];

    switch (g_current.type) {
        case sSTRING:
        case sCTEINT:
        case sCTECHAR:
        case sTRUE:
        case sFALSE:
            parser_advance();
            break;

        case sIDENTIF:
            strcpy(name, g_current.lexeme);
            parser_advance();

            if (g_current.type == sABREPAR) {
                parse_call_after_name(name);
            } else if (g_current.type == sABRECOL) {
                parse_vec_after_name(name);
                parser_require_declared_identifier(name);
            } else {
                parser_require_declared_identifier(name);
            }
            break;

        default:
            diag_syntax_error_expected("elemento de expressao", &g_current);
            break;
    }
}

static void parse_elem(void) {
    parse_expr();
}

static void parse_id_only(char *out_name, int out_size) {
    if (g_current.type != sIDENTIF) {
        diag_syntax_error(sIDENTIF, &g_current);
    }

    strncpy(out_name, g_current.lexeme, (size_t)out_size - 1);
    out_name[out_size - 1] = '\0';
    parser_advance();
}

static void parse_id_or_vec_name(char *out_name, int out_size, int *is_vec_use) {
    parse_id_only(out_name, out_size);
    *is_vec_use = 0;

    if (g_current.type == sABRECOL) {
        *is_vec_use = 1;
        parse_vec_after_name(out_name);
    }
}

static void parse_call_after_name(const char *name) {
    diag_info("enter <call>: %s", name);   
    parser_require_declared_identifier(name);
    parser_expect(sABREPAR);
    if (parser_expr_starts(g_current.type)) {
        parse_expr();
        while (parser_accept(sVIRG)) {
            parse_expr();
        }
    }
    parser_expect(sFECHAPAR);
    diag_info("exit <call>: %s", name);    
}

static void parse_vec_after_name(const char *name) {
    parser_expect(sABRECOL);
    if (g_current.type == sCTEINT) {
        parser_advance();
    } else if (g_current.type == sIDENTIF) {
        parser_require_declared_identifier(g_current.lexeme);
        parser_advance();
    } else {
        diag_syntax_error_expected("indice de vetor", &g_current);
    }
    parser_expect(sFECHACOL);
    (void)name;
}

static void parser_require_declared_identifier(const char *name) {
    if (ts_lookup(name) == NULL) {
        diag_syntax_error_expected("identificador declarado", &g_current);
    }
}