/*
 * syntax.c
 *
 * Copyright 2012 Jared Hance
 */
#ifndef LLCC_SYNTAX_HEADER
#define LLCC_SYNTAX_HEADER

#include <llvm-c/Core.h>
#include "hashmap.h"

/* Enums */
enum llcc_statement_type {
    LLCC_STATEMENT_ASSIGN,
    LLCC_STATEMENT_BLOCK,
    LLCC_STATEMENT_EXPR,
    LLCC_STATEMENT_IF,
    LLCC_STATEMENT_FOR,
    LLCC_STATEMENT_WHILE
};

enum llcc_expr_type {
    /* Math */
    LLCC_EXPR_ADD,
    LLCC_EXPR_SUB,
    LLCC_EXPR_MUL,
    LLCC_EXPR_DIV,

    LLCC_EXPR_LITERAL,
    LLCC_EXPR_VARIABLE,

    LLCC_EXPR_FUNCTION_CALL
};

enum llcc_type_primitive {
    LLCC_TYPE_PRIMITIVE_CHAR,
    LLCC_TYPE_PRIMITIVE_INT
};

enum llcc_type_category {
    LLCC_TYPE_CATEGORY_POINTER,
    LLCC_TYPE_CATEGORY_PRIMITIVE,
    LLCC_TYPE_CATEGORY_STRUCT
};

/* Structs */
struct llcc_type {
    enum llcc_type_category category;
    union {
        struct llcc_type *pointer_data;
        enum llcc_type_primitive primitive_data;
        char *struct_data;
    } data;
};

/* Advance declare because statement-type-data can include other statements */
struct llcc_statement;
struct llcc_expr;

struct llcc_declaration {
    char *identifier;
    struct llcc_type type;
};

/*
 * A struct is essentially a declaration-only block. Later it could be useful
 * to make a hash table of the declarations, but for now its easier just to
 * build a vector and do sequential search.
 */
struct llcc_struct_declaration {
    char *identifier;

    size_t elements_size;
    struct llcc_declaration *elements;
};

struct llcc_block {
    size_t declarations_size;
    struct llcc_declaration *declarations;
    
    size_t statements_size;
    struct llcc_statement *statements;
};

struct llcc_expr_function_call {
    char *function_name;

    size_t params_size;
    struct llcc_expr *params;
};

struct llcc_expr_literal {
    enum llcc_type_primitive type;
    union {
        int int_data;
        char char_data;
    } data;
};

struct llcc_expr_binary_math {
    struct llcc_expr *left;
    struct llcc_expr *right;
};

struct llcc_expr {
    enum llcc_expr_type type;
    struct llcc_type derived_type;
    union {
        struct llcc_expr_function_call function_call_data;
        struct llcc_expr_literal literal_data;
        struct llcc_expr_binary_math add_data;
        struct llcc_expr_binary_math sub_data;
        struct llcc_expr_binary_math mul_data;
        struct llcc_expr_binary_math div_data;
        char *variable_data;
    } data;
};

struct llcc_statement_assign {
    char *identifier;
    struct llcc_expr expr;
};

struct llcc_statement_if {
    struct llcc_expr condition;
    
    size_t statements_size;
    struct llcc_statement *statements;
};

struct llcc_statement_for {
    struct llcc_statement *initialization;
    struct llcc_expr condition;
    struct llcc_statement *iteration;

    size_t statements_size;
    struct llcc_statement *statements;
};

struct llcc_statement_while {
    struct llcc_expr condition;
    
    size_t statements_size;
    struct llcc_statement *statements;
};

struct llcc_statement {
    enum llcc_statement_type type;
    union {
        struct llcc_statement_assign assign_data;
        struct llcc_block block_data;
        struct llcc_expr expr_data;
        struct llcc_statement_if if_data;
        struct llcc_statement_for for_data;
        struct llcc_statement_while while_data;
    } data;
};

struct llcc_function {
    char *name;

    size_t param_types_size;
    struct llcc_type *param_types;

    struct llcc_block block;

    /* NULL if there is no return type (void) */
    struct llcc_type *return_type;
};

/*
 * Corresponds essentially to both a C file and a LLVM module.
 */
struct llcc_module {
    char *name;

    size_t functions_size;
    struct llcc_function *functions;

    size_t structs_size;
    struct llcc_struct_declaration *structs;
};

struct variable_info {
    LLVMValueRef allocation;
    char *identifier;
    struct llcc_type type;
};

void derive_type(struct llcc_expr *expr, struct hashmap *localmap);

#endif
