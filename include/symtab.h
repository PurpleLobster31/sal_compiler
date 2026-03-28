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

void ts_init(void);
void ts_close(void);

void ts_enter_scope(const char *name);
void ts_leave_scope(void);

const Scope *ts_current_scope(void);
const char *ts_current_scope_name(void);

int ts_insert(const char *lexeme,
              SymbolCategory category,
              DataType type,
              int extra,
              int decl_line);

const Symbol *ts_lookup(const char *lexeme);
const Symbol *ts_lookup_current_scope(const char *lexeme);
const Symbol *ts_first_symbol(void);

const char *ts_symbol_category_name(SymbolCategory category);
const char *ts_data_type_name(DataType type);

int ts_update_type(const char *lexeme, DataType type);
int ts_update_extra(const char *lexeme, int extra);

#endif /* SYMTAB_H */