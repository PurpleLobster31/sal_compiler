# SALc - Compilador para a Linguagem SAL

**Autor:** Matheus Mendonça Lopes - RA 10443495  
**Turma:** 06N - 2026.1  
**Disciplina:** Compiladores - Prof. Leandro Carlos Fernandes

---

## Compilação

A compilação do projeto segue as orientações fornecidas no enunciado do projeto, através do comando abaixo:

```bash
make
```

Esse comando compila todos os módulos com as flags `-Wall -Wextra -std=c99` e gera o binário `salc` dentro da raiz do projeto. Os objetos intermediários ficam em `obj/`. Todas essas informações foram construídas dentro do arquivo `makefile`, que foi construído com base na estrutura do projeto, considerando a existências das pastas `src`, com os arquivos `.c`, e `include`, com os arquivos `.h`.

---

## Execução

Também seguindo as requesições do enunciado, a execução do projeto é feita da seguinte forma:

```
./salc <arquivo.sal> [--tokens] [--symtab] [--trace]
```

As flags são opcionais, mas elas geram arquivos com conteúdos específicos, que são explicados abaixo:

| Flag | Arquivo gerado | Conteúdo |
|---|---|---|
| `--tokens` | `<nome>.tk` | Lista de tokens reconhecidos pelo analisador léxico |
| `--symtab` | `<nome>.ts` | Tabela de símbolos consolidada, organizada por escopo |
| `--trace` | `<nome>.trc` | Rastreamento da análise sintática (entrada/saída de não-terminais) |

As flags são independentes e podem ser combinadas livremente. Se preferir não informar, o compilador apenas valida o fonte e reporta erros, sem gerar artefatos.

---

## Estrutura do Projeto (pós-compilação)

```
sal_compiler/
├── src/          Código-fonte dos módulos
├── include/      Cabeçalhos públicos (.h)
├── obj/          Objetos intermediários (gerado pelo make)
├── salc          Binário compilado
└── makefile
```

### Módulos

| Módulo | Arquivo | Funcionalidades |
|---|---|---|
| `main` | `main.c` | Faz a orquestração como um todo; CLI, inicialização e encerramento dos módulos. Além disso, segue o que foi pedido no enunciado de não conter lógica direta. |
| `lex` | `lex.c` | Atua como analisador léxico e retorna um token por chamada a `lex_next()`. |
| `parser` | `parser.c` | Atua como analisador sintático, nesse caso ASDR, e implementa a gramática SAL. Além disso gerencia o criação e encerramento de escopos. |
| `symtab` | `symtab.c` | Faz o gerenciamento da tabela de símbolos com suporte a escopos aninhados. Faz também o controle de inserção, busca e ciclo de vida dos escopos via `ts_insert`, `ts_lookup`, `ts_enter_scope` e `ts_leave_scope`. |
| `diag` | `diag.c` | Centraliza o tratamento de erros e o rastreamento da análise. Além disso, faz com que erros léxicos e sintáticos encerram o processo com mensagem clara; o rastreamento é emitido via `diag_info`. |
| `opt` | `opt.c` | Interpreta a linha de comando e disponibiliza as opções ao compilador via `opts_get`. Através dele são reconhecidas as flags `--tokens`, `--symtab` e `--trace`. |
| `log` | `log.c` | Cria e gerencia os arquivos auxiliares de saída conforme as opções ativas. Faz a escrita dos tokens, tabela de símbolos e rastreamento nos arquivos gerados (`.tk`, `.ts`, `.trc`). |
| `token` | `token.c` | Faz a definição dos tipos de token, suas representações internas usadas no `.tk` (`token_type_name`) e os nomes legíveis usados nas mensagens de erro (`token_display_name`). |

---

## Particularidades da Implementação

### Tabela de Símbolos e Escopos

A Tabela de Símbolos (TS) guarda, pra cada identificador declarado, cinco atributos: lexema, categoria, tipo, escopo e um campo `extra`. O campo `extra` carrega, para vetores, o tamanho declarado, e a quantidade de parâmetros no caso de sub-rotinas.

Os nomes de escopo registrados no arquivo `.ts` seguem o formato:

| Escopo | Exemplo | Criação |
|---|---|---|
| `global` | `global` | Ao iniciar o programa |
| `fn:<nome>` | `fn:SOMA` | Ao entrar na definição de uma função |
| `proc:<nome>` | `proc:main` | Ao entrar na definição de um procedimento |
| `<pai>.block#N` | `proc:main.block#2` | A cada bloco `start/end` interno, numerado em ordem de aparição |

Parâmetros e variáveis locais de uma sub-rotina são inseridos no escopo dela. Variáveis globais ficam sempre no escopo `global` e são visíveis em qualquer sub-rotina.

### Distinção entre `proc <nome>` e `proc main`

Para explicar esse ponto, o parser usa um token de lookahead (`parser_peek()`) para distinguir `proc main` de qualquer outro `proc`, sem consumir o identificador antes da hora.

### Operador de disjunção `v`

O operador lógico `v` é uma letra isolada, o que poderia causar conflito por fazer parte do léxico de variáveis, funções, etc. O analisador léxico reconhece como palavra reservada (`sOR`) ao invés de identificador, e faz uma verificação explícita após a leitura de sequências alfanuméricas.

### Formato dos arquivos de log

- **`.tk`** - `<linha>  <CATEGORIA>  "<lexema>"`, um token por linha
- **`.ts`** - `SCOPE=<escopo>  id="<lexema>"  cat=<categoria>  tipo=<tipo>  extra=<valor>`
- **`.trc`** - `enter <não-terminal>` / `exit <não-terminal>`, refletindo a derivação completa da gramática

### Tokens de delimitadores e pontuação

A especificação da linguagem SAL define os delimitadores e símbolos de pontuação, como `(`, `)`, `[`, `]`, `,`, `:`, `;`, `=>` e `..`, apenas na gramática, e não atribui pra eles categorias léxicas nomeadas (tokens do tipo `sNOME`). Então esses caracteres aparecem na EBNF como literais entre aspas que o parser simplesmente espera encontrar.

Pra esse projeto, optou-se em criar tipos de token explícitos para cada um deles (`sABREPAR`, `sFECHAPAR`, `sABRECOL`, `sFECHACOL`, `sVIRG`, `sDPTO`, `sPVIRG`, `sIMPLIC`, `sPTOPTO`). Isso faz com que o analisador léxico os classifique e os entregue ao parser com um tipo bem definido, tornou a construção mais amigável, e acabou fazendo que as mensagens de erro indiquem qual delimitador estava sendo esperado. Isso fez com que os diagnósticos fossem mais informativos sem alterar a semântica da linguagem.

