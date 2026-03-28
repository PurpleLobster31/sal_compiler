#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum SymbolCategory {
    SYM_VAR,
    SYM_VECTOR,
    SYM_PARAM,
    SYM_PROC,
    SYM_FUNC
} SymbolCategory;

typedef enum DataType {
    TYPE_UNDEF = 0,
    TYPE_INT,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_VOID
} DataType;

typedef struct Scope Scope;
typedef struct Symbol Symbol;

struct Scope {
    char name[128];
    int level;
    int block_index;
    Scope *parent;
};

struct Symbol {
    char lexeme[256];
    SymbolCategory category;
    DataType type;
    char scope_name[128];
    int extra;
    int decl_line;
    Scope *scope;
    Symbol *next;
};

/*
 * Inicializa a tabela de símbolos.
 */
void ts_init(void);
/*
 * Finaliza a tabela de símbolos e libera recursos.
 */
void ts_close(void);

/*
 * Entra em um novo escopo com o nome especificado.
 */
void ts_enter_scope(const char *name);
/*
 * Sai do escopo atual.
 */
void ts_leave_scope(void);

/*
 * Retorna o escopo atual.
 */
const Scope *ts_current_scope(void);
/*
 * Retorna o nome do escopo atual.
 */
const char *ts_current_scope_name(void);

/*
 * Insere um novo símbolo na tabela com os dados fornecidos.
 * Retorna 1 se inserido com sucesso, 0 se já existe.
 */
int ts_insert(const char *lexeme,
              SymbolCategory category,
              DataType type,
              int extra,
              int decl_line);

/*
 * Busca um símbolo pelo lexema em todos os escopos.
 */
const Symbol *ts_lookup(const char *lexeme);
/*
 * Busca um símbolo pelo lexema apenas no escopo atual.
 */
const Symbol *ts_lookup_current_scope(const char *lexeme);
/*
 * Retorna o primeiro símbolo da tabela para iteração.
 */
const Symbol *ts_first_symbol(void);

/*
 * Retorna o nome da categoria do símbolo como string.
 */
const char *ts_symbol_category_name(SymbolCategory category);
/*
 * Retorna o nome do tipo de dados como string.
 */
const char *ts_data_type_name(DataType type);

/*
 * Atualiza o tipo de um símbolo existente.
 * Retorna 1 se atualizado, 0 se não encontrado.
 */
int ts_update_type(const char *lexeme, DataType type);
/*
 * Atualiza o campo extra de um símbolo existente.
 * Retorna 1 se atualizado, 0 se não encontrado.
 */
int ts_update_extra(const char *lexeme, int extra);

#endif /* SYMTAB_H */