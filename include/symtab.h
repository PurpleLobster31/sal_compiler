#ifndef SYMTAB_H
#define SYMTAB_H

/*
 * Esse enum representa as categorias possíveis de um símbolo na tabela.
 *
 * Os tipos possíveis são os seguintes:
 * - SYM_VAR    : variável simples.
 * - SYM_VECTOR : vetor; o campo extra armazena seu tamanho declarado.
 * - SYM_PARAM  : parâmetro formal de sub-rotina.
 * - SYM_PROC   : procedimento (não retorna valor).
 * - SYM_FUNC   : função (retorna um valor de tipo básico).
 */
typedef enum SymbolCategory {
    SYM_VAR,
    SYM_VECTOR,
    SYM_PARAM,
    SYM_PROC,
    SYM_FUNC
} SymbolCategory;

/*
 * Esse enum representa os tipos de dados primitivos suportados pela linguagem SAL.
 *
 * Os tipos possíveis são os seguintes:
 * - TYPE_INT   : tipo inteiro.
 * - TYPE_BOOL  : tipo booleano.
 * - TYPE_CHAR  : tipo caractere.
 * - TYPE_UNDEF : tipo ainda não resolvido (estado intermediário durante o parsing de funções).
 * - TYPE_VOID  : ausência de tipo de retorno; usado internamente para procedimentos.
 */
typedef enum DataType {
    TYPE_UNDEF = 0,
    TYPE_INT,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_VOID
} DataType;

typedef struct Scope Scope;
typedef struct Symbol Symbol;

/*
 * Essa struct representa um escopo ativo na pilha de escopos da tabela de símbolos.
 *
 * Os atributos são os seguintes:
 * - name        : nome descritivo do escopo (ex: "global", "fn:SOMA", "proc:main.block#1").
 * - level       : profundidade de aninhamento (0 = global).
 * - block_index : índice incremental para nomear blocos start/end internos.
 * - parent      : ponteiro para o escopo pai na pilha.
 */
struct Scope {
    char name[128];
    int level;
    int block_index;
    Scope *parent;
};

/*
 * Essa struct representa um símbolo inserido na tabela durante a análise.
 *
 * Os atributos são os seguintes:
 * - lexeme     : nome do identificador conforme declarado no fonte.
 * - category   : categoria do símbolo (variável, vetor, parâmetro, etc.).
 * - type       : tipo de dado associado.
 * - scope_name : nome do escopo em que o símbolo foi declarado.
 * - extra      : tamanho do vetor (SYM_VECTOR) ou número de parâmetros (SYM_FUNC/SYM_PROC).
 * - decl_line  : linha do arquivo-fonte onde o símbolo foi declarado.
 * - scope      : ponteiro para o escopo de declaração.
 * - next       : ponteiro para o próximo símbolo na lista encadeada global.
 */
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
 * Faz a inicialização da tabela de símbolos, preparando as estruturas internas.
 */
void ts_init(void);

/*
 * Faz a finalização da tabela de símbolos e libera todos os recursos alocados.
 */
void ts_close(void);

/*
 * Abre um novo escopo com o nome fornecido, empilhando-o sobre o escopo atual.
 */
void ts_enter_scope(const char *name);

/*
 * Fecha o escopo atual, retornando ao escopo pai.
 */
void ts_leave_scope(void);

/*
 * Retorna um ponteiro para o escopo atualmente ativo.
 */
const Scope *ts_current_scope(void);

/*
 * Retorna o nome do escopo atualmente ativo.
 */
const char *ts_current_scope_name(void);

/*
 * Insere um novo símbolo no escopo atual com os dados fornecidos.
 * Retorna 1 se inserido com sucesso, ou 0 se já existe um símbolo com o mesmo lexema no escopo atual (declaração duplicada).
 */
int ts_insert(const char *lexeme,
              SymbolCategory category,
              DataType type,
              int extra,
              int decl_line);

/*
 * Busca um símbolo pelo lexema percorrendo todos os escopos, do mais interno ao mais externo.
 * Retorna um ponteiro para o símbolo encontrado, ou NULL se não declarado.
 */
const Symbol *ts_lookup(const char *lexeme);

/*
 * Busca um símbolo pelo lexema somente no escopo atual.
 * Usada para detectar declarações duplicadas no mesmo escopo.
 * Retorna um ponteiro para o símbolo encontrado, ou NULL se não existir.
 */
const Symbol *ts_lookup_current_scope(const char *lexeme);

/*
 * Retorna o primeiro símbolo da lista global para iteração sequencial.
 * A ordem de iteração reflete a ordem de inserção durante a análise.
 */
const Symbol *ts_first_symbol(void);

/*
 * Retorna o nome da categoria do símbolo como string (ex: "var", "func").
 * Usado na geração do arquivo de tabela de símbolos (.ts).
 */
const char *ts_symbol_category_name(SymbolCategory category);

/*
 * Retorna o nome do tipo de dados como string (ex: "int", "bool").
 * Usado na geração do arquivo de tabela de símbolos (.ts).
 */
const char *ts_data_type_name(DataType type);

/*
 * Atualiza o tipo de retorno de um símbolo já inserido.
 * Usada após a resolução do tipo de retorno de funções durante o parsing.
 * Retorna 1 se o símbolo foi encontrado e atualizado, 0 caso contrário.
 */
int ts_update_type(const char *lexeme, DataType type);

/*
 * Atualiza o campo extra de um símbolo já inserido.
 * Usada para registrar a contagem final de parâmetros de sub-rotinas.
 * Retorna 1 se o símbolo foi encontrado e atualizado, 0 caso contrário.
 */
int ts_update_extra(const char *lexeme, int extra);

#endif /* SYMTAB_H */