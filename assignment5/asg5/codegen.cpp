#include <string>
using namespace std;

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
#include "codegen.h"
//FILE* oilfile;
symbol_table* stable = new symbol_table;

int vregcount=0;
string get_vreg(astree* node){
    char typechar;
    if(node->attributes[ATTR_int]) { typechar = 'i'; }
    if(node->attributes[ATTR_string]) { typechar = 's'; }
    if(node->attributes[ATTR_struct]) { typechar = 'p'; }
    else{ typechar = 'a'; }
    node->vreg = typechar + to_string(vregcount);
    vregcount++;
    insert_symbol(stable, node);
    return node->vreg;
}

void genoil(FILE* outfile, astree* root){
    fprintf(outfile, "#define __OCLIB_C__ \n");
    fprintf(outfile, "#include \"oclib.oh\" \n");
    //oilfile=outfile;
    for(auto child:root->children){
        switch(child->symbol){
        case TOK_STRUCT:    
            codegen_struct(outfile, 
                *child->children[0]->lexinfo, child);
            break;
        case TOK_WHILE:
            codegen_while(outfile, child);
            break;
        case TOK_IF:
        case TOK_IFELSE:
        case TOK_FUNC:
            codegen_func(outfile, child);
        case TOK_IDENT:
            break;
        }
            
    }
}
void codegen_rec(FILE* outfile, astree* node){
    for(auto child:node->children)
        switch(node->symbol){
        case TOK_WHILE:
            codegen_main(outfile, node);
            return;
        default:
           codegen_rec(outfile, child);
           codegen_main(outfile, node);
           break;
        }
}
void codegen_main(FILE* outfile, astree* node){
    if(node==nullptr){return;}
    switch(node->symbol){
        case TOK_WHILE:
            codegen_while(outfile, node);
            break;
        case TOK_IF:
            break;
        case TOK_IFELSE:
            break;
        case TOK_VARDECL:
            get_vardecl(outfile, node);
            break;
        case TOK_RETURN:
            break;
        case TOK_CALL:
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case TOK_POS:
        case TOK_NEG:
        case TOK_INTCON:
        case TOK_IDENT:
        case TOK_CHARCON:
            get_expr(outfile, node);
            break;
        default:
               break;
    }
}
void codegen_struct(FILE* outfile, string struct_name, astree* node){
    fprintf(outfile, "struct s_%s {\n", struct_name.c_str());
    for(auto i = node->children.cbegin()+1; 
        i != node->children.cend(); i++){
        if((*i)->children[0] != nullptr && 
            (*i)->children[1] != nullptr){
        fprintf(outfile, "  ");
        fprintf(outfile, "%s",(*(*i)->lexinfo).c_str());
        fprintf(outfile, " f_%s_", 
            (*(*i)->children[1]->lexinfo).c_str());
        fprintf(outfile, "%s;\n", 
            (*(*i)->children[0]->lexinfo).c_str());
    }
    }
    fprintf(outfile, "}\n");
    codegen_main(outfile, node->children[1]);
} 

void get_ident(FILE* outfile, astree* node){
    if(node->blocknr == 0){
        fprintf(outfile, "__%s", (node->lexinfo)->c_str());
    }
    else{   
        fprintf(outfile, "_%zd_%s", (node->blocknr), 
            (*node->lexinfo).c_str());
    }
    fprintf(outfile, "\n");
}
void get_vardecl(FILE* outfile, astree* node){
    symbol* sym = lookup(stable, node);
    string s = get_vreg(node);
    if(sym == nullptr){
        fprintf(outfile,"%s _%zd_%s %s %s;\n",
        node->children[0]->lexinfo->c_str(), node->blocknr,
        node->children[0]->children[0]->lexinfo->c_str(), 
        node->lexinfo->c_str(), node->children[1]->lexinfo->c_str());
    }
    else{
        fprintf(outfile, "%s %s %s;\n",
        node->children[0]->vreg.c_str(),
        node->lexinfo->c_str(),
        node->vreg.c_str());
   
    }
}
void codegen_while(FILE* outfile, astree* node){
    fprintf(outfile, "while_%zd_%zd_%zd:;\n", node->lloc.filenr, 
        node->lloc.linenr, node->lloc.offset);
    fprintf(outfile, "  ");
    switch(node->children[0]->symbol){
        case TOK_IDENT:
        case TOK_CHARCON:
        case TOK_INTCON:
            
        default:
            get_expr(outfile, node->children[0]);
            
    }       
    //fprintf(outfile, "    ");
    fprintf(outfile, "if (!%s) goto break_%zd_%zd_%zd;\n", 
        node->children[1]->vreg.c_str(),
        node->children[1]->lloc.filenr,
        node->children[1]->lloc.linenr,
        node->children[1]->lloc.offset);
    codegen_main(outfile, node->children[1]);
}

void codegen_func(FILE* outfile, astree* node){
  fprintf(outfile, "%s __%s(\n", 
        (node->children[0]->lexinfo)->c_str(), 
        (node->children[0]->children[0]->lexinfo)->c_str());    
   size_t k;
    astree* rightkid;
  if(node->children.size() > 1){
     rightkid= node->children[1];
     k=0;
  }else{
    rightkid=node;
    k=1;
  }
 for( size_t child = k; child < rightkid->children.size(); child++){
    DEBUGF ('o', "child is %zd, and there are %zd childs \n",
    child, rightkid->children.size());
        fprintf(outfile, "  %s _%zd_%s,\n", 
            (rightkid->lexinfo)->c_str(), 
            node->blocknr, (rightkid->children[0]->lexinfo)->c_str());
    }
        
    fprintf(outfile, ")\n{\n");
    for(auto block : node->children[1]->children){
        codegen_rec(outfile, block);
    }
    fprintf(outfile, "}\n");
}

void get_binop(FILE* outfile, astree* node){
    //symbol* sym = lookup(stable, node);
    //string vreg = sym->vreg;
    fprintf(outfile, "%s %s %s", node->children[0]->vreg.c_str(), 
        node->lexinfo->c_str(), node->children[1]->vreg.c_str());
    
}

void get_expr(FILE* outfile, astree* node){
    if(node->attributes[ATTR_vreg]){
        get_binop(outfile, node);
    }
    else{
        switch(node->symbol){
        case TOK_IDENT:
            get_ident(outfile, node);
            break;
        case TOK_INTCON:
        case TOK_CHARCON:
            string v =  get_vreg(node);
            fprintf(outfile, "%s",v.c_str());
            break;
        }
    }
}
