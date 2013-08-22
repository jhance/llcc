%start root
%token TOKEN_COMMA
%token TOKEN_SEMICOLON
%token TOKEN_LBRACE
%token TOKEN_RBRACE
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_ASSIGN
%token TOKEN_INT
%token TOKEN_CHAR
%token TOKEN_STRUCT
%token TOKEN_IDENTIFIER
%token TOKEN_LITERAL_INT

%error-verbose

%{
#include <stdlib.h>
#include <stdio.h>
#include "syntax.h"
#include "list.h"
#include "parser_out.h"
#include "module_body.h"

%}

%union {
    int int_data;
    char *string_data;
    struct list list_data;
    struct llcc_block block_data;
    struct llcc_function *function_data;
    struct llcc_declaration *declaration_data;
    struct llcc_statement *statement_data;
    struct llcc_struct *struct_data;
    struct llcc_expr expr_data;

    struct module_body module_body_data;
}

%type <int_data> TOKEN_LITERAL_INT
%type <string_data> TOKEN_IDENTIFIER
%type <list_data> statements declarations
%type <block_data> block
%type <declaration_data> declaration
%type <function_data> function
%type <statement_data> statement
%type <struct_data> struct
%type <expr_data> expression
%type <module_body_data> module_declarations

%{
struct list list_append(struct list l, void *val);
struct list list_new(void);

struct llcc_module parser_output;

%}

%%

root:
    module_declarations {
        struct module_body m = $1;
        struct llcc_module module;
        size_t i;

        module.functions_size = m.functions.size;
        module.functions =
            malloc(sizeof(struct llcc_function) * m.functions.size);
        module.structs_size = m.structs.size;
        module.structs = 
            malloc(sizeof(struct llcc_struct_declaration) * m.structs.size);

        for(i = 0; i < m.functions.size; i++) {
            module.functions[i] =
                *((struct llcc_function *) m.functions.arr[i]);
            free(m.functions.arr[i]);
        }
        free(m.functions.arr);

        for(i = 0; i < m.structs.size; i++) {
            module.structs[i] =
                *((struct llcc_struct_declaration *) m.structs.arr[i]);
            free(m.structs.arr[i]);
        }
        free(m.structs.arr);

        parser_output = module;
    }

module_declarations:
    module_declarations function {
        struct module_body m = $1;
        m.functions = list_append(m.functions, $2);
        $$ = m;
    }
    | module_declarations struct {
        struct module_body m = $1;
        m.structs = list_append(m.structs, $2);
        $$ = m;
    }
    | function {
        struct module_body m;
        m.functions = list_new();
        m.functions = list_append(m.functions, $1);
        m.structs = list_new();
        $$ = m;
    }
    | struct {
        struct module_body m;
        m.functions = list_new();
        m.structs = list_new();
        m.structs = list_append(m.structs, $1);
        $$ = m;
    }

struct:
    TOKEN_STRUCT TOKEN_IDENTIFIER TOKEN_LBRACE declarations TOKEN_RBRACE
    TOKEN_SEMICOLON {
        char *identifier = $2;
        struct list declarations = $4;
        struct llcc_struct_declaration *s =
            malloc(sizeof(struct llcc_struct_declaration));
        size_t i;

        s->elements_size = declarations.size;
        s->elements =
            malloc(sizeof(struct llcc_declaration) * declarations.size);
        for(i = 0; i < declarations.size; i++) {
            s->elements[i] = *((struct llcc_declaration *) declarations.arr[i]);
            free(declarations.arr[i]);
        }
        free(declarations.arr);

        $$ = s;
    }

function:
    TOKEN_IDENTIFIER TOKEN_LPAREN TOKEN_RPAREN block {
        struct llcc_function *f = malloc(sizeof(struct llcc_function));

        f->name = $1;
        f->block = $4;

        f->param_types_size = 0;
        f->param_types = NULL;

        f->return_type = NULL;

        $$ = f;
    }

block:
    TOKEN_LBRACE declarations statements TOKEN_RBRACE {
        struct llcc_block b;
        struct list declarations = $2;
        struct list statements = $3;
        size_t i;

        b.declarations_size = declarations.size;
        b.declarations =
            malloc(sizeof(struct llcc_declaration) * declarations.size);
        for(i = 0; i < declarations.size; i++) {
            b.declarations[i] =
                *((struct llcc_declaration *) declarations.arr[i]);
            free(declarations.arr[i]);
        }
        free(declarations.arr);

        b.statements_size = statements.size;
        b.statements = malloc(sizeof(struct llcc_statement) * statements.size);
        for(i = 0; i < statements.size; i++) {
            b.statements[i] = *((struct llcc_statement *) statements.arr[i]);
            free(statements.arr[i]);
        }
        free(statements.arr);

        $$ = b;
    }

declarations:
    declarations declaration { $$ = list_append($1, $2); }
    | declaration { $$ = list_new(); $$ = list_append($$, $1); }

declaration:
    TOKEN_INT TOKEN_IDENTIFIER TOKEN_SEMICOLON {
        struct llcc_declaration *d = malloc(sizeof(struct llcc_declaration));
        d->type.category = LLCC_TYPE_CATEGORY_PRIMITIVE;
        d->type.data.primitive_data = LLCC_TYPE_PRIMITIVE_INT;
        d->identifier = $2;

        $$ = d;
    }

statements:
    statements statement { $$ = list_append($1, $2); }
    | statement { $$ = list_new(); $$ = list_append($$, $1); }

statement:
    TOKEN_IDENTIFIER TOKEN_ASSIGN expression TOKEN_SEMICOLON {
        struct llcc_statement *st = malloc(sizeof(struct llcc_statement));
        st->type = LLCC_STATEMENT_ASSIGN;
        st->data.assign_data.identifier = $1;
        st->data.assign_data.expr = $3;
        $$ = st;
    }
    | block {
        struct llcc_statement *st = malloc(sizeof(struct llcc_statement));
        st->type = LLCC_STATEMENT_BLOCK;
        st->data.block_data = $1;
        $$ = st;
    }

expression:
    TOKEN_LITERAL_INT {
        struct llcc_expr expr;
        expr.type = LLCC_EXPR_LITERAL;
        expr.data.literal_data.type = LLCC_TYPE_PRIMITIVE_INT;
        expr.data.literal_data.data.int_data = $1;
        $$ = expr;
    }
%%

struct list list_append(struct list l, void *val) {
    if(l.capacity > l.size) {
        l.arr[l.size] = val;
        l.size++;
    }
    else {
        l.capacity *= 2;
        l.arr = realloc(l.arr, l.capacity);
        l.arr[l.size] = val;
        l.size++;
    }

    return l;
}

struct list list_new(void) {
    struct list l;
    l.arr = malloc(sizeof(struct llcc_function) * 16);
    l.capacity = 16;
    l.size = 0;

    return l;
}

yyerror(char const *err) {
    fprintf(stderr, "Error: %s\n", err);
}
