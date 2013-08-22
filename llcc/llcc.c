#include <stdio.h>
#include <stdlib.h>
#include <llvm-c/Core.h>
#include <llvm-c/BitWriter.h>
#include "codegen.h"
#include "syntax.h"
#include "parser_out.h"

int yyparse();

int main(int argc, char **argv) {
    LLVMModuleRef lmodule;
    size_t i;

    struct llcc_module module;

    if(argc > 1) {
        printf("Source file: %s\n", argv[1]);
    }
    else {
        printf("Usage: llcc <source-file>\n");
    }

    yyparse();
    module = parser_output;
    module.name = "main";
    lmodule = gen_module(module);
    LLVMWriteBitcodeToFile(lmodule, "out.bc");

    return 0;
}
