#include "symtab.h"

#include <stdlib.h>
#include <string.h>

static Scope *g_current_scope = NULL;
static Symbol *g_symbols_head = NULL;
static Symbol *g_symbols_tail = NULL;

static void ts_free_scopes(Scope *scope);
static void ts_free_symbols(void);

void ts_init(void) {
    g_current_scope = NULL;
    g_symbols_head = NULL;
    g_symbols_tail = NULL;
}

void ts_close(void) {
    ts_free_symbols();
    ts_free_scopes(g_current_scope);

    g_current_scope = NULL;
    g_symbols_head = NULL;
    g_symbols_tail = NULL;
}

void ts_enter_scope(const char *name) {
    Scope *scope;

    scope = (Scope *)malloc(sizeof(Scope));
    if (scope == NULL) {
        return;
    }

    strncpy(scope->name, name, sizeof(scope->name) - 1);
    scope->name[sizeof(scope->name) - 1] = '\0';
    scope->parent = g_current_scope;
    scope->level = (g_current_scope == NULL) ? 0 : g_current_scope->level + 1;
    scope->block_index = 0;

    g_current_scope = scope;
}

void ts_leave_scope(void) {
    Scope *old_scope;

    if (g_current_scope == NULL) {
        return;
    }

    old_scope = g_current_scope;
    g_current_scope = g_current_scope->parent;
    free(old_scope);
}

const Scope *ts_current_scope(void) {
    return g_current_scope;
}

const char *ts_current_scope_name(void) {
    if (g_current_scope == NULL) {
        return "<sem_escopo>";
    }
    return g_current_scope->name;
}

int ts_insert(const char *lexeme,
              SymbolCategory category,
              DataType type,
              int extra,
              int decl_line) {
    Symbol *symbol;

    if (g_current_scope == NULL || lexeme == NULL) {
        return 0;
    }

    if (ts_lookup_current_scope(lexeme) != NULL) {
        return 0;
    }

    symbol = (Symbol *)malloc(sizeof(Symbol));
    if (symbol == NULL) {
        return 0;
    }

    strncpy(symbol->lexeme, lexeme, sizeof(symbol->lexeme) - 1);
    symbol->lexeme[sizeof(symbol->lexeme) - 1] = '\0';

    symbol->category = category;
    symbol->type = type;
    symbol->extra = extra;
    symbol->decl_line = decl_line;
    symbol->scope = g_current_scope;
    symbol->next = NULL;

    strncpy(symbol->scope_name, g_current_scope->name, sizeof(symbol->scope_name) - 1);
    symbol->scope_name[sizeof(symbol->scope_name) - 1] = '\0';

    /* g_symbols_tail evita percorrer a lista inteira a cada inserção */
    if (g_symbols_tail == NULL) {
        g_symbols_head = symbol;
        g_symbols_tail = symbol;
    } else {
        g_symbols_tail->next = symbol;
        g_symbols_tail = symbol;
    }

    return 1;
}

const Symbol *ts_lookup(const char *lexeme) {
    Scope *scope_iter;

    if (lexeme == NULL) {
        return NULL;
    }

    /* Busca do escopo mais interno para o mais externo, respeitando a visibilidade da linguagem.
     * Variáveis locais sobrepõem globais de mesmo nome */
    for (scope_iter = g_current_scope; scope_iter != NULL; scope_iter = scope_iter->parent) {
        Symbol *sym_iter = g_symbols_head;

        while (sym_iter != NULL) {
            if (strcmp(sym_iter->scope_name, scope_iter->name) == 0 &&
                strcmp(sym_iter->lexeme, lexeme) == 0) {
                return sym_iter;
            }
            sym_iter = sym_iter->next;
        }
    }

    return NULL;
}

const Symbol *ts_lookup_current_scope(const char *lexeme) {
    Symbol *sym_iter;

    if (g_current_scope == NULL || lexeme == NULL) {
        return NULL;
    }

    sym_iter = g_symbols_head;
    while (sym_iter != NULL) {
        if (strcmp(sym_iter->scope_name, g_current_scope->name) == 0 &&
            strcmp(sym_iter->lexeme, lexeme) == 0) {
            return sym_iter;
        }
        sym_iter = sym_iter->next;
    }

    return NULL;
}

const Symbol *ts_first_symbol(void) {
    return g_symbols_head;
}

const char *ts_symbol_category_name(SymbolCategory category) {
    switch (category) {
        case SYM_VAR:    return "var";
        case SYM_VECTOR: return "vector";
        case SYM_PARAM:  return "param";
        case SYM_PROC:   return "proc";
        case SYM_FUNC:   return "fn";
    }

    return "unknown";
}

const char *ts_data_type_name(DataType type) {
    switch (type) {
        case TYPE_INT:   return "int";
        case TYPE_BOOL:  return "bool";
        case TYPE_CHAR:  return "char";
        case TYPE_VOID:  return "void";
        case TYPE_UNDEF: return "undef";
    }

    return "undef";
}

static void ts_free_scopes(Scope *scope) {
    Scope *parent;

    while (scope != NULL) {
        parent = scope->parent;
        free(scope);
        scope = parent;
    }
}

static void ts_free_symbols(void) {
    Symbol *curr;
    Symbol *next;

    curr = g_symbols_head;
    while (curr != NULL) {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

int ts_update_type(const char *lexeme, DataType type) {
    Symbol *sym = g_symbols_head;

    while (sym != NULL) {
        if (strcmp(sym->lexeme, lexeme) == 0) {
            sym->type = type;
            return 1;
        }
        sym = sym->next;
    }

    return 0;
}

int ts_update_extra(const char *lexeme, int extra) {
    Symbol *sym = g_symbols_head;

    while (sym != NULL) {
        if (strcmp(sym->lexeme, lexeme) == 0) {
            sym->extra = extra;
            return 1;
        }
        sym = sym->next;
    }

    return 0;
}