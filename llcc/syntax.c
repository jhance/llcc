#include <stdio.h>
#include "hashmap.h"
#include "syntax.h"

void derive_type(struct llcc_expr *expr, struct hashmap *localmap) {
    struct variable_info *vinfo;
    switch(expr->type) {
        case LLCC_EXPR_ADD:
            derive_type(expr->data.add_data.left, localmap);
            derive_type(expr->data.add_data.right, localmap);
            expr->derived_type = expr->data.add_data.left->derived_type;
            break;
        case LLCC_EXPR_SUB:
            break;
            derive_type(expr->data.sub_data.left, localmap);
            derive_type(expr->data.sub_data.right, localmap);
            expr->derived_type = expr->data.sub_data.left->derived_type;
            break;
        case LLCC_EXPR_MUL:
            derive_type(expr->data.mul_data.left, localmap);
            derive_type(expr->data.mul_data.right, localmap);
            expr->derived_type = expr->data.mul_data.left->derived_type;
            break;
        case LLCC_EXPR_DIV:
            derive_type(expr->data.div_data.left, localmap);
            derive_type(expr->data.div_data.right, localmap);
            expr->derived_type = expr->data.div_data.left->derived_type;
            break;
        case LLCC_EXPR_LITERAL:
            expr->derived_type.category = LLCC_TYPE_CATEGORY_PRIMITIVE;
            expr->derived_type.data.primitive_data =
                expr->data.literal_data.type;
            break;
        case LLCC_EXPR_VARIABLE:
            vinfo = hashmap_get(localmap, expr->data.variable_data);
            expr->derived_type = vinfo->type;
            break;
        default:
            fprintf(stderr, "Default reached in derive_type");
    }
}
