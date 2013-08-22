CFLAGS = -Wall -Wextra -pedantic -std=c89 -D __STDC_LIMIT_MACROS -D __STDC_CONSTANT_MACROS -g
LDFLAGS = -lLLVM-3.1
YFLAGS = -d

bin_PROGRAMS = llcc
llcc_SOURCES = llcc.c hashmap.c codegen.c parser.y lexer.l syntax.c
