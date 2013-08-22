#ifndef LLCC_CODEGEN_HEADER
#define LLCC_CODEGEN_HEADER

#include <llvm-c/Core.h>
#include "syntax.h"

LLVMModuleRef gen_module(struct llcc_module module);
LLVMValueRef gen_function(LLVMModuleRef module, struct llcc_function function);

#endif
