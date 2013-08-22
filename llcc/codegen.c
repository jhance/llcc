#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>

#include "codegen.h"
#include "hashmap.h"
#include "syntax.h"

LLVMTypeRef convert_type(struct llcc_type type);
LLVMTypeRef gen_struct(LLVMModuleRef module, struct llcc_struct_declaration s);
LLVMValueRef gen_expr(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_expr expr);
void gen_block(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_block block);
void gen_statement(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_statement statement);

LLVMTypeRef convert_type(struct llcc_type type) {
    switch(type.category) {
        case LLCC_TYPE_CATEGORY_POINTER:
            return LLVMPointerType(convert_type(*(type.data.pointer_data)), 0);
        case LLCC_TYPE_CATEGORY_PRIMITIVE:
            switch(type.data.primitive_data) {
                case LLCC_TYPE_PRIMITIVE_CHAR:
                    return LLVMInt8Type();
                case LLCC_TYPE_PRIMITIVE_INT:
                    return LLVMInt64Type();
                default:
                    fprintf(stderr, "Switch Error [Defualt] convert_type: llcc_type.category.primitive_data");
                    exit(1);
        }
        case LLCC_TYPE_CATEGORY_STRUCT:
            return NULL; /* TODO Fix this. */
        default:
            fprintf(stderr, "Switch Error [Default] convert_type: llcc_type.category");
            exit(1);
    }
}

LLVMModuleRef gen_module(struct llcc_module module) {
    LLVMModuleRef lmodule;
    size_t i;

    lmodule = LLVMModuleCreateWithName(module.name);

    for(i = 0; i < module.structs_size; i++) {
        gen_struct(lmodule, module.structs[i]);
    }

    for(i = 0; i < module.functions_size; i++) {
        gen_function(lmodule, module.functions[i]);
    }
    
    return lmodule;
}

LLVMValueRef gen_function(LLVMModuleRef module, struct llcc_function function) {
    LLVMTypeRef return_type;
    LLVMTypeRef *param_types;
    LLVMTypeRef function_type;
    LLVMValueRef lfunction;
    LLVMBuilderRef builder;
    LLVMBasicBlockRef primary_block;
    struct hashmap *localmap;
    size_t i;

    if(function.return_type) {
        return_type = convert_type(*(function.return_type));
    }
    else {
        return_type = LLVMVoidType();
    }

    param_types = malloc(sizeof(*param_types) * function.param_types_size);
    for(i = 0; i < function.param_types_size; i++) {
        param_types[i] = convert_type(function.param_types[i]);
    }

    function_type = LLVMFunctionType(return_type, param_types,
            function.param_types_size, 0);

    lfunction = LLVMAddFunction(module, function.name, function_type);
    builder = LLVMCreateBuilder();
    primary_block = LLVMAppendBasicBlock(lfunction, "primary");

    LLVMPositionBuilderAtEnd(builder, primary_block);

    localmap = hashmap_alloc(64);
    gen_block(builder, localmap, function.block);
    hashmap_free_values(localmap);
    hashmap_free(localmap);
    localmap = NULL;

    if(!function.return_type) {
        LLVMBuildRetVoid(builder);
    }

    LLVMDisposeBuilder(builder);

    return lfunction;
}

void gen_declaration(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_declaration decl) {
    LLVMValueRef v = LLVMBuildAlloca(builder, convert_type(decl.type),
            decl.identifier);
    struct variable_info *vinfo = malloc(sizeof(struct variable_info));
    vinfo->allocation = v;
    vinfo->identifier = decl.identifier;
    vinfo->type = decl.type;
    hashmap_put(localmap, decl.identifier, vinfo);
}


void gen_block(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_block block) {
    size_t i;
    for(i = 0; i < block.declarations_size; i++) {
        gen_declaration(builder, localmap, block.declarations[i]);
    }
    for(i = 0; i < block.statements_size; i++) {
        gen_statement(builder, localmap, block.statements[i]);
    }
}

void gen_statement_assign(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_statement_assign st) {
    struct variable_info *vinfo = hashmap_get(localmap, st.identifier);
    LLVMValueRef location = vinfo->allocation;
    LLVMValueRef value = gen_expr(builder, localmap, &st.expr);
    LLVMBuildStore(builder, value, location);
}

void gen_statement(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_statement statement) {
    switch(statement.type) {
        case LLCC_STATEMENT_ASSIGN:
            gen_statement_assign(builder, localmap, statement.data.assign_data);
            break;
        case LLCC_STATEMENT_BLOCK:
            gen_block(builder, localmap, statement.data.block_data);
        case LLCC_STATEMENT_EXPR:
            break;
        case LLCC_STATEMENT_IF:
            break;
        case LLCC_STATEMENT_FOR:
            break;
        case LLCC_STATEMENT_WHILE:
            break;
    }
}

LLVMValueRef gen_expr_literal(struct llcc_expr_literal e) {
    switch(e.type) {
        case LLCC_TYPE_PRIMITIVE_CHAR:
            return LLVMConstInt(LLVMInt8Type(), e.data.char_data, 0);
        case LLCC_TYPE_PRIMITIVE_INT:
            return LLVMConstInt(LLVMInt64Type(), e.data.int_data, 1);
    }
}

LLVMValueRef gen_expr(LLVMBuilderRef builder, struct hashmap *localmap,
        struct llcc_expr *expr) {
    derive_type(expr, localmap);
    switch(expr.type) {
        case LLCC_EXPR_LITERAL:
            return gen_expr_literal(expr.data.literal_data);
        default:
            fprintf(stderr, "gen_expr default (bug): %d\n", expr.type);
            return NULL;
    }
}

LLVMTypeRef gen_struct(LLVMModuleRef module, struct llcc_struct_declaration s) {
    LLVMTypeRef *types = malloc(sizeof(*types) * s.elements_size);
    size_t i;
    
    for(i = 0; i < s.elements_size; i++) {
        types[i] = convert_type(s.elements[i].type);
    }

    LLVMTypeRef t =
        LLVMStructCreateNamed(LLVMGetGlobalContext(), s.identifier);
    LLVMStructSetBody(t, types, s.elements_size, 0);

    return t;
}
