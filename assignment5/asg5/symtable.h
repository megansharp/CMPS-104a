#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>

#include "auxlib.h"
#include "lyutils.h"
#include "astree.h"

using symbol_table = unordered_map<const string*, symbol*>;
using symbol_entry = pair<const string*, symbol*>;
struct symbol {
   attr_bitset attributes;
   symbol_table* fields;
   location lloc;
   size_t blocknr, linenr, offset, filenr;
   vector<symbol_table*> parameters;
   astree* node;
   const string* strType;

};

//vector<symbol_table*> stack;
//vector<size_t> block_stack;
//size_t next_block =0;
void push_block();
void pop_block();
void define_ident(astree* node);
symbol* stack_find_ident(astree* node);
char* strd(const char* s);
string get_attributes(attr_bitset attributes);
void check_prototype(FILE* outfile, astree* node);
void check_block(astree* node);
void check_function(FILE* outfile, astree* node, astree* left);
void print_symbol(FILE* outfile, astree* node);
symbol* new_symbol(astree* node);
symbol* insert_symbol(symbol_table* sym, astree* node);
symbol* lookup(symbol_table* symtab, astree* node);
bool compatible(astree* left, astree* right);
void adopt_type(astree* child, astree* parent);
void adopt_attribute(astree* child, astree* parent);
void semantic_analysis(FILE* outfile, astree* node);
void rtypecheck(FILE* outfile, astree* node);
void typecheck(FILE* outfile, astree* node);
#endif
