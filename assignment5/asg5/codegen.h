
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

#include "symtable.h"
#include "astree.h"
#include "lyutils.h"
#include "auxlib.h"

void genoil(FILE* outfile, astree* root);
void codegen_struct(FILE* outfile, string struct_name, astree* node);
void get_ident(FILE* outfile, astree* node);
void codegen_while(FILE* outfile, astree* node);
void codegen_func(FILE* outfile, astree* node);
void get_param(FILE* outfile, astree* node);
void get_expr(FILE* outfile, astree* node);
void get_binop(FILE* outfile, astree* node);
void codegen_rec(FILE* outfile, astree* node);
void codegen_main(FILE* outfile, astree* node);
void get_vardecl(FILE* outfile, astree* node);
