#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
#include <string.h>
#include <stdlib.h>
using namespace std;
#include "symtable.h"
#include "astree.h"
#include "lyutils.h"
#include "auxlib.h"

size_t nextsym;
vector<symbol_table*> stack;
vector<size_t> block_stack;
size_t next_block= 0;
symbol_table* st = new symbol_table;
int block_num= 0;
symbol_table* type_name = new symbol_table;
symbol_table* field_name = new symbol_table;
symbol_table* ident_name = new symbol_table;
attr_bitset attr_prim;
attr_bitset attr_ref;
attr_bitset attr_prim_ref;
attr_bitset array;

//let us intilize 
//stack.push_back(new symbol_table);
//block_stack.push_back(0);

void push_block(){
   next_block++;
   block_stack.push_back(next_block);
   stack.push_back(nullptr);
}

void pop_block(){
   block_stack.pop_back();
   next_block--;
   stack.pop_back();    
}
symbol_table* stackTOP(){
  if(stack.back() !=nullptr){}
    else{
      stack.back() = new symbol_table;
    }
  return stack.back();
}

void define_ident(astree* node) {
    if (stack.back() == nullptr) {
        stack.back() = new symbol_table;
    }
    insert_symbol(stack.back(), node);
}

symbol* stack_find_ident(astree* node) {
    for (auto table : stack) {
        if (table != nullptr && !(table->empty())) {
            if (lookup(table, node) != nullptr) {
              //DEBUGF ('q', "there node in the table \n");
                return lookup(table, node);
            }

        }
    }
  //DEBUGF('q', "node wasn't found \n");
   return nullptr;
}

char* strd(const char* s){
  int slen = strlen(s);
  char* result = (char*) malloc(slen + 1);
  if(result == NULL){
    return NULL;
  }

  memcpy(result, s, slen+1);
  return result;
}
string get_attributes(attr_bitset attributes) {
    string str = "";

    if (attributes.test (ATTR_void))        str += "void ";
    if (attributes.test (ATTR_bool))        str += "bool ";
    if (attributes.test (ATTR_char))        str += "char ";
    if (attributes[ATTR_int] == 1)          str += "int ";
    if (attributes.test (ATTR_null))        str += "null ";
    if (attributes.test (ATTR_string))      str += "string ";
    if (attributes.test (ATTR_struct))      str += "struct ";
    if (attributes.test (ATTR_array))       str += "array ";
    if (attributes.test (ATTR_function))    str += "function ";
    if (attributes.test (ATTR_variable))    str += "variable ";
    if (attributes.test (ATTR_typeid))      str += "typeid ";
    if (attributes.test (ATTR_param))       str += "param ";
    if (attributes.test (ATTR_lval))        str += "lval ";
    if (attributes.test (ATTR_const))       str += "const ";
    if (attributes.test (ATTR_vreg))        str += "vreg ";
    if (attributes.test (ATTR_vaddr))       str += "vaddr ";
    //if (attributes.test (ATTR_field))       str += "field ";
    return (str.c_str());
}

symbol* new_symbol (astree* node) {
    symbol* sym = new symbol();
    sym->attributes = node->attributes;
    sym->fields = nullptr;
    sym->lloc.filenr = node->lloc.filenr;
    sym->lloc.linenr = node->lloc.linenr;
    sym->lloc.offset = node->lloc.offset;
    sym->blocknr = node->blocknr;
    sym->node = node;
    node->sym = sym;
    return sym;
}
symbol* insert_symbol(symbol_table* symtab, astree* node){
      if ((symtab!=NULL) && (node!=NULL)) {
        symbol* sym = new_symbol(node);
        symtab->insert( make_pair((string*)node->lexinfo, sym));
        return sym;
    }
    return nullptr;
}
symbol* get_declar_sym(attr_bitset testAttr, 
                      symbol_table* table1, astree* node, 
                      size_t blockNum) 
{ DEBUGF('q', "check g.d.s 1 \n");
        if ((node->symbol == TOK_VOID) && 
           (!testAttr.test(ATTR_function))) {
                errprintf
                   ("Error! There cannot be a void declaration!\n");
                return nullptr;
        }
         DEBUGF('q', "check g.d.s 2 \n");
        astree* declar;
        const string* declarType;
       DEBUGF('q', "check g.d.s 3 \n");
        if (testAttr.test(ATTR_array)) {
                switch (node->children[0]->symbol) {
                        case TOK_VOID:
                                errprintf
                           ("Error! There cannot be a void array!\n");
                                return nullptr;
                        case TOK_INT:
                                testAttr.set(ATTR_int);
                                break;
                        case TOK_STRING:
                                testAttr.set(ATTR_string);
                                break;
                        case TOK_TYPEID:
                                testAttr.set(ATTR_struct);
                                break;
                        case TOK_ARRAY:
                                testAttr.set(ATTR_array);
                                break;
                        default:
                                break;
                }
                DEBUGF('q', "check g.d.s 4 \n");
                declar = node->children[1];
                declarType = node->children[0]->lexinfo;

        }
        else {DEBUGF('q', "check g.d.s 5 \n");
                declar = node->children[0];
                declarType = node->lexinfo;
        }
        DEBUGF('q', "check g.d.s 6 \n");
        symbol* sym = lookup(table1, declar);
        if (sym != nullptr) {
                if (testAttr.test(ATTR_function)) {
                }
                else {
                        errprintf
            ("Error! There cannot be a duplicate declaration!\n");
                        return nullptr;
                }
        }

        sym = insert_symbol(table1, declar);
        sym->attributes = testAttr;
        sym->blocknr = blockNum;
        node->blocknr = blockNum;
        declar->blocknr = blockNum;

        if (testAttr.test(ATTR_struct)) {
                declar->strType = declarType;
                sym->strType = declarType;
        }
        return sym;
}
symbol* getSymbol( attr_bitset testAttr, symbol_table* fields,
                  astree* node, size_t blockNum)
{
   if ((node->symbol == TOK_VOID) && 
        (!testAttr.test(ATTR_function))) {
        errprintf("Error! There cannot be a void declaration!\n");
        return nullptr;
    }

    astree* declar;
    const string* declarType;

    if (testAttr.test(ATTR_array)) {
        switch (node->children[0]->symbol) {
            case TOK_VOID:
                errprintf("Error! There cannot be a void array!\n");
                return nullptr;
            case TOK_INT:
                testAttr.set(ATTR_int);
                break;
            case TOK_STRING:
                testAttr.set(ATTR_string);
                break;
            case TOK_TYPEID:
                testAttr.set(ATTR_struct);
                break;
            case TOK_ARRAY:
                testAttr.set(ATTR_array);
                break;
            default:
                break;
        }
        declar = node->children[1];
        declarType = node->children[0]->lexinfo;
    }
    else {
        declar = node->children[0];
        declarType = node->lexinfo;
    }

    symbol* sym = stack_find_ident(declar);
    if (sym != nullptr) {
        if (testAttr.test(ATTR_function)) {
        }
        else {
            errprintf
            ("Error! There cannot be a duplicate declaration!\n");
            return nullptr;
        }
    }

    sym = insert_symbol(fields, declar);
    sym->attributes = testAttr;
    sym->blocknr = blockNum;
    node->blocknr = blockNum;
    declar->blocknr = blockNum;

    if (testAttr.test(ATTR_struct)) {
        declar->strType = declarType;
        sym->strType = declarType;
    } 
    return sym;
}
symbol* lookup(symbol_table* symtab, astree* node) {
   if(symtab->count(node->lexinfo) == 0){
      return nullptr;      
   }
   else
   return(symtab->find(node->lexinfo))->second;
}
astree* current_struct = nullptr;
void print_symbol(FILE* outfile, astree* node) {
    //attr_bitset att = node->attributes;

    if (node->attributes[ATTR_struct]) {
        fprintf (outfile, "\n");
    } else {
        fprintf (outfile, "    ");
    }
    if (node->attributes[ATTR_field]) {
        fprintf (outfile, "%s (%zu.%zu.%zu) field {%s} ",
            (node->lexinfo)->c_str(),
            node->lloc.linenr, node->lloc.filenr, node->lloc.offset,
            (current_struct->lexinfo)->c_str());
    } else {
          
        fprintf (outfile, "%s (%zu.%zu.%zu) {%zu} ",
            (node->lexinfo)->c_str(),
            node->lloc.linenr, node->lloc.filenr, node->lloc.offset,
            node->blocknr);
    }
    if (node->attributes[ATTR_struct]) {
        fprintf (outfile, "struct \"%s\" ", 
            ( node->lexinfo)->c_str());
        current_struct = node;
    }

    fprintf (outfile, "%s\n", 
    (get_attributes(node->attributes)).c_str());
}

void check_block(astree* node) {
    if (node->symbol == TOK_BLOCK) {
        push_block();
    }
    node->blocknr = block_num;
    for (auto child : node->children) {
        check_block (child);
    }
}

void check_prototype(FILE* outfile, astree* node) {
    node->children[0]->children[0]->attributes[ATTR_function]=1;
    push_block(); 

    for (auto child : node->children[1]->children) {
        child->children[0]->attributes.set (ATTR_variable);
        child->children[0]->attributes.set (ATTR_lval);
        child->children[0]->attributes.set (ATTR_param);
        child->children[0]->blocknr = block_num;
        define_ident (child);
        print_symbol(outfile, child->children[0]);
    } 
    pop_block();

}
void do_block(FILE* outfile, astree* node) {
    astree* rightRChild = node->children[2];
    if (rightRChild == nullptr) {
        return;
    }
    rightRChild->blocknr = block_stack.back();

    for (size_t childIdx = 0; childIdx < rightRChild->children.size(); 
        childIdx++) {
        semantic_analysis(outfile, rightRChild->children[childIdx]);
    }

    fprintf(outfile, "\n");
}
symbol* do_parameter(FILE* outfile, 
  symbol_table* symStackTop, astree* node) {
       DEBUGF('q', "check d_p node is %s \n",(node->lexinfo)->c_str());
        attr_bitset tempAttrList;
        tempAttrList.set(ATTR_param);
        tempAttrList.set(ATTR_variable);
        tempAttrList.set(ATTR_lval);
        DEBUGF('q', "check d_p1 \n");
        switch(node->symbol) {
                case TOK_VOID:
                        tempAttrList.set(ATTR_void);
                        break;
                case TOK_INT:
                        tempAttrList.set(ATTR_int);
                        break;
                case TOK_STRING:
                        tempAttrList.set(ATTR_string);
                        break;
                case TOK_TYPEID:
                        tempAttrList.set(ATTR_struct);
                        break;
                case TOK_ARRAY:
                        tempAttrList.set(ATTR_array);
                        break;
                default:
                        break;
        }
        DEBUGF('q', "check d_p2 \n");
        symbol* param_sym = get_declar_sym(tempAttrList, symStackTop, 
                                    node, block_stack.back());
        if (param_sym == nullptr) {
                return nullptr;
        } DEBUGF('q', "I am printing %s \n", (node->lexinfo)->c_str());
    fprintf(outfile, "    %s (%ld.%ld.%ld) {%ld} %s\n", 
            param_sym->node->lexinfo->c_str(), 
            param_sym->node->lloc.filenr, param_sym->node->lloc.linenr, 
            param_sym->node->lloc.offset, param_sym->blocknr, 
            get_attributes(param_sym->attributes).c_str());
        return param_sym;
}

void parameter_traversal(FILE* outfile, symbol* sym, astree* node) {
   DEBUGF('q', "check 2 \n");
    astree* rightChild = node->children[1];
    rightChild->blocknr = block_stack.back();
    symbol* param_sym;

    for (size_t childIdx = 0; childIdx < rightChild->children.size(); 
         childIdx++) {
        param_sym = do_parameter(outfile, stackTOP(), 
                                 rightChild->children[childIdx]);
        if (param_sym != nullptr) {
          //(sym->node->parameters).push_back(param_sym);
           
        }
    }

    fprintf(outfile, "\n");
}

void check_function(FILE* outfile, astree* node, astree* left) {
   DEBUGF('q', "check 1 \n");
  astree* typen;
  if(left->symbol == TOK_ARRAY){
    typen = left->children[1];
  }
  else{
    typen = left -> children[0];
  }
  DEBUGF('q', "check 1.1 \n");
  if (stack_find_ident(typen) !=nullptr){
    return;
  }
  attr_bitset tempAttr;
  tempAttr.set(ATTR_function);
  DEBUGF('q', "check 1.2 \n");
  switch(left->symbol){
    case TOK_VOID:
      tempAttr.set(ATTR_void);
      break;
    case TOK_INT:
      tempAttr.set(ATTR_int);
      break;
    case TOK_STRING:
      tempAttr.set(ATTR_string);
      break;
    case TOK_TYPEID:
      tempAttr.set(ATTR_struct);
      break;
    case TOK_ARRAY:
      tempAttr.set(ATTR_array);
      break;
    default:
      break;
  }
    symbol* sym = get_declar_sym(tempAttr, st, 
                            left, block_stack.back());
    if (sym == nullptr) {
        return;
    }
    // Print out the symbol
    fprintf(outfile, "%s (%ld.%ld.%ld) {%ld} %s\n", 
            sym->node->lexinfo->c_str(), sym->node->lloc.filenr, 
            sym->node->lloc.linenr, sym->node->lloc.offset, 
            sym->blocknr, get_attributes(sym->attributes).c_str());
        // Enter the block
        push_block();
        // Store the name of the current struct
        current_struct = sym->node;
        // Traverse the parameters
        parameter_traversal(outfile, sym, node);
        if (node->symbol == TOK_FUNC) {
                do_block(outfile, node);
        }
        pop_block();
        current_struct = nullptr;

}
void process_fields(FILE* outfile, symbol_table* fields, astree* node){
  attr_bitset testAttr;
  testAttr.set(ATTR_field);
  DEBUGF ('q', "check 3.2 node is \"%s\" \n", (node->lexinfo)->c_str());
    switch (node->symbol) {
        case TOK_VOID:
            testAttr.set(ATTR_void);
            break;
        case TOK_INT:
            testAttr.set(ATTR_int);         
            break;
        case TOK_STRING:
            testAttr.set(ATTR_string);          
            break;
        case TOK_TYPEID:
            testAttr.set(ATTR_struct);   
            break;
        case TOK_ARRAY:
            testAttr.set(ATTR_array);           
            break;
        default:
            break;
    }
    symbol* sym = getSymbol(testAttr, fields, node, 0);
    DEBUGF('q', "3.9 %s, attributes: %s \n" , 
      (sym->node->lexinfo)->c_str(), 
      (get_attributes(sym->attributes)).c_str());
    if (sym != nullptr){
      //print_symbol(outfile, sym->node);
    fprintf(outfile, "    %s (%zu.%zu.%zu) field {%s} %s \n", 
                (node->children[0]->lexinfo)->c_str(), 
                sym->node->lloc.filenr, 
                sym->node->lloc.linenr, sym->node->lloc.offset, 
                (current_struct->lexinfo)->c_str(), 
                (get_attributes(sym->attributes)).c_str());
    }
}

void traverse_fields(FILE* outfile, symbol* sym, astree* parent){
  //block_num++;
  size_t k;
  current_struct = parent->children[0];
  astree* rightkid;
  if(parent->children.size() >1){
     rightkid= parent->children[1];
     k=0;
  }else{
    rightkid=parent;
    k=1;
  }
 for( size_t child = k; child < rightkid->children.size(); child++){
    DEBUGF ('q', "child is %zd, and there are %zd childs %s\n",
    child, rightkid->children.size(), (sym->node->lexinfo)->c_str());
    process_fields(outfile, sym->fields, rightkid->children[child]);
  }
    //block_num--;
    current_struct = nullptr;
    fprintf(outfile, "\n" );

}


/*
symbol* symbol_for_type(astree* node, bool isArray) {
   
}

*/
bool compatible(astree* left, astree* right) {
   for(size_t i = 0; i < ATTR_function; i++){
      if(left->attributes[i] == 1 && right->attributes[i] == 1){
         return true;            
      }
   }
   return false;
}

void adopt_type(astree* parent, astree* child){
  //DEBUGF('q', "child type\"%s\" \n",
          //get_attributes(child->attributes).c_str());
   for(size_t i = 0; i < ATTR_function; i++){
      if(child->attributes[i] == 1){
         parent->attributes.set(i);
         //DEBUGF('q', "parent adopting type \"%s\" \n",
          //get_attributes(parent->attributes).c_str());       
      }
   }
}
void adopt_attribute(astree* parent, astree* child){
  //DEBUGF('q', "child attributes \"%s\" \n",
          //get_attributes(child->attributes).c_str());
   for(size_t i = 0; i < ATTR_bitset_size; i++){
      if(child->attributes[i] == 1){
         parent->attributes.set(i); 
          //DEBUGF('q', "child attributes \"%s\" \n",
          //get_attributes(child->attributes).c_str());      
      }
   }    
}

void semantic_analysis(FILE* outfile, astree* node) {
   astree* left = nullptr;
   astree* right = nullptr;
   symbol* sym = nullptr;
   //fprintf(outfile, "we are in semantic analysis\n");
   if(node->children.size() > 0){
      left = node->children[0];  
      //DEBUGF('q', "has left child \n");  
   }
   if(node->children.size() > 1){
      right = node->children[1]; 
      //DEBUGF('q', "has right child \n");          
   }
   //fprintf(outfile, "we are in semantic analysis\n");
   DEBUGF('q', "node->symbol is %d, or %s \n", 
           node->symbol, parser::get_tname(node->symbol));
   switch(node->symbol){
      case TOK_VOID:
         if(left==nullptr) break;  
        //DEBUGF ('q', "Check 1 \n");
         //left->attributes[ATTR_void]=1;
          //DEBUGF ('q', "Check 1 \n");
         break;
      case TOK_CHAR:
         if(left==nullptr) break;
        // left->attributes[ATTR_char]=1;
         //adopt_type(node, left);
         break;
      case TOK_INT:
         if(left==nullptr) break;
         //left->attributes.set(ATTR_int);
         //adopt_type(node, left);
         break;
      case TOK_STRING:
         if(left==nullptr) break;
         //left->attributes[ATTR_string]=1;
         //adopt_type(node, left);
         break;               
      case TOK_IF:            
      case TOK_ELSE:              
      case TOK_IFELSE:
        /* if(left->attributes[ATTR_bool] == 0){
            errprintf("Error, (%zu.%zu.%zu) must be bool\n",
              node->lloc.filenr, node->lloc.linenr,
              node->lloc.offset);   
         }*/
         break;
      case TOK_WHILE:
        /* if(left->attributes[ATTR_bool] == 0){
            errprintf("Error, (%zu.%zu.%zu) must be bool\n",
              node->lloc.filenr, node->lloc.linenr,
              node->lloc.offset);   
         }*/
         break;
      case TOK_RETURN:
         //fprintf(outfile, "we are in return\n");
         break;               
      case TOK_STRUCT:
         left->attributes[ATTR_struct]=1;
         sym= stack_find_ident(left);
         if(sym != nullptr){
          errprintf (
            "Error, (%zu.%zu.%zu) can't have duplicate struct\n",
              node->lloc.filenr, node->lloc.linenr,
              node->lloc.offset);
              break;
         }
         //print this shit
          fprintf(outfile, "%s (%zu.%zu.%zu) {0} struct \"%s\"\n", 
                   (left->lexinfo)->c_str(), node->lloc.filenr, 
                   node->lloc.linenr, node->lloc.offset, 
                   (left->lexinfo)->c_str());
          //insert that bitch so we can look it up
         sym =insert_symbol(st, left);
         sym->attributes.set(ATTR_typeid);
         sym->fields = new symbol_table;
         sym->blocknr=0;
         left->sym= sym;
         left->blocknr=0;
         traverse_fields(outfile, sym, node);
         break;
      case TOK_NULL:
         node->attributes[ATTR_null]=1;
         node->attributes[ATTR_const]=1;
         break;               
      case TOK_NEW:
          //adopt_attribute(node, left);
          break;
      case TOK_ARRAY:
        /* left->attributes[ATTR_array]=1;
         if (left == nullptr || 
                left->children.empty())                     
                break;
            left->children[0]->attributes.set(ATTR_array); */
            break;                    
      case TOK_EQ:
      case TOK_NE:   
      case TOK_LT:            
      case TOK_LE:   
      case TOK_GT:   
      case TOK_GE:
        /* if(compatible(left, right)){
            node->attributes[ATTR_bool]=1;       
         }
         else{
            errprintf("Error, (%zu.%zu.%zu) incompatible types \n",
              node->lloc.filenr, node->lloc.linenr,
              node->lloc.offset);   
         }
         break;*/
      case TOK_IDENT: 
         sym = stack_find_ident(node);

            if (sym == nullptr) {
                sym= lookup(st, node);
            }
         if(sym == nullptr){
             /*   errprintf(
                 "Error (%zu.%zu.%zu), identifier \"%s\" not found\n",
                  node->lloc.filenr, 
                  node->lloc.linenr, node->lloc.offset,
                  (node->lexinfo)->c_str()); */
                break;
         }
         node->attributes = sym->attributes;          
         break;
      case TOK_INTCON: 
         node->attributes[ATTR_int]=1;
         node->attributes[ATTR_const]=1;
         break;   
      case TOK_CHARCON: 
         node->attributes[ATTR_char]=1;
         node->attributes[ATTR_const]=1;
         break;     
      case TOK_STRINGCON:
         node->attributes[ATTR_string]=1;
         node->attributes[ATTR_const]=1;
         break;    
      case TOK_BLOCK: 
         push_block();
         for(size_t i=0; i< node->children.size(); i++){
               semantic_analysis(outfile, node->children[i]);
            }
         pop_block();
         break;
      case TOK_INITDECL: 
         break;     
      case TOK_FUNC:
         check_function(outfile, node, left);
         break; 
      case TOK_NEG:   
      case TOK_POS:
              if(right == nullptr){
                break;
              } 
              if(!(right->attributes[ATTR_int])){
                errprintf(
                "Error, (%zu.%zu.%zu) right side must be int\n",
              node->lloc.filenr, node->lloc.linenr,
              node->lloc.offset);
                break;
              }      
              node->attributes.set(ATTR_int);
              node->attributes.set(ATTR_vreg);
              break;              
      case TOK_NEWARRAY:
         //node->attributes[ATTR_vreg]=1;
         //node->attributes[ATTR_array]=1;
         //adopt_type(node, left);
         break;               
      case TOK_TYPEID: 
         //node->attributes[ATTR_typeid]=1;
         break;               
      case TOK_FIELD: 
        /*node->attributes[ATTR_field]=1;
         if(left != nullptr){
            left->attributes[ATTR_field]=1;
            adopt_type(node, left);
         }*/
         break;               
      case TOK_INDEX:
        // node->attributes[ATTR_lval]=1;
         //node->attributes[ATTR_vaddr]=1;
         break;               
      case TOK_ORD: 
         //node->attributes[ATTR_int]=1;
         //node->attributes[ATTR_vreg]=1;
         break;               
      case TOK_CHR: 
         //node->attributes[ATTR_char]=1;
         //node->attributes[ATTR_vreg]=1;
         break;
      case TOK_ROOT: 
                DEBUGF('q', "we are in root \n");             
      case TOK_DECLID: 
         break;               
      case TOK_VARDECL: 
         /*left->children[0]->attributes[ATTR_lval] = 1;
         left->children[0]->attributes[ATTR_variable] = 1;
         //DEBUGF ('q', "check 1 \n");
         adopt_attribute(node, left);
         if (stack_find_ident(left->children[0])) {
            errprintf(
              "Error (%zu.%zu.%zu), variable already declared\n",
               node->lloc.filenr, node->lloc.linenr, 
               node->lloc.offset);     
         }
         //DEBUGF ('q', "Check 2 \n");
         //define_ident(left->children[0]);
         //DEBUGF ('q', "Check 3 \n");
         //print_ symbol(outfile, left->children[0]);
         //DEBUGF ('q', "Check 4 \n");*/
         break;               
      case TOK_PRAM:
        // node->attributes[ATTR_param]=1;
         break;               
      case TOK_RETURNVOID: 
         break;               
      case TOK_PROTOTYPE:
         //DEBUGF ('q', "check 0 \n"); 
         check_function(outfile, node, left);
         //DEBUGF ('q', "check 1 \n"); 
         break;               
      case TOK_NEWSTRING:
         //node->attributes[ATTR_vreg]=1;
         //node->attributes[ATTR_string]=1;
         break;     
      case TOK_CALL:    
         //sym = lookup(st, node->children.back());
         //DEBUGF ('q', "check 1 \n");
        /* if (sym == nullptr) {
             errprintf (
             "Error (%zu.%zu.%zu), function \"%s\" not found\n", 
                  node->lloc.filenr, 
                  node->lloc.linenr, node->lloc.offset,
                  (node->lexinfo)->c_str());
             break;
         }*/
         if(left->sym != nullptr){
            node->attributes= left->sym->attributes;
            node->attributes.set(ATTR_vreg);
            node->attributes.flip(ATTR_function);
            node->strType = left->sym->strType;
         }
         break;
      case '(':
      case '[':
         break;
      case '+':
      case '-':
        
         if ((right == nullptr) || (left == nullptr)) break;
         if ((left->attributes[ATTR_int]) ||
            (right->attributes[ATTR_int])) {
              node->attributes[ATTR_int]=1;
              node->attributes[ATTR_vreg]=1;
              break; 
            }
         errprintf (
                "Error, (%zu.%zu.%zu) type int required \n",
                 node->lloc.filenr, node->lloc.linenr,
                 node->lloc.offset);
                 break;

      case '*':
      case '/':       
      case '%':
          if ((right == nullptr) || (left == nullptr)) break;
         if ((left->attributes[ATTR_int]) ||
            (right->attributes[ATTR_int])) {
              node->attributes[ATTR_int]=1;
              node->attributes[ATTR_vreg]=1; 
              break; 
            }
         errprintf (
                "Error, (%zu.%zu.%zu) type int required \n",
                 node->lloc.filenr, node->lloc.linenr,
                 node->lloc.offset);
                 break;
      case '!':
         if((right==nullptr)){
          break;
         }
         if (!(right->attributes[ATTR_int])) {
                errprintf(
                  "Error, (%zu.%zu.%zu) type bool required \n",
                    node->lloc.filenr, node->lloc.linenr,
                    node->lloc.offset);
                break;
         }
         node->attributes.set (ATTR_vreg);
         node->attributes.set (ATTR_bool);
         break;
      case '^':
         break;
      case '.':
         //node->attributes.set (ATTR_lval);
         //node->attributes.set (ATTR_vaddr);
         //sym = lookup(st, node);
         //adopt_type(node, left);                      
         break;
      case '=':
         if ((left == nullptr)||(right == nullptr)) break;
            if (!(left->attributes[ATTR_lval])){
                break;
            }
            node->attributes = (node->attributes & attr_prim_ref);
            node->attributes.set(ATTR_vreg);
            break;
      default:
            errprintf ("Error, invalid token \"%s\"",
                parser::get_tname(node->symbol));
            break;
   }
}
void rtypecheck(FILE* outfile, astree* node){
   for(auto child: node->children){
      rtypecheck(outfile, child);          
   }
   semantic_analysis(outfile, node);
}
void typecheck(FILE* outfile, astree* node){
   stack.push_back(new symbol_table);
   block_stack.push_back(0);
    attr_prim.set(ATTR_int);
    attr_ref.set(ATTR_array);
    attr_ref.set(ATTR_string);
    attr_ref.set(ATTR_struct);
    attr_ref.set(ATTR_array);
    attr_prim_ref.set(ATTR_int);
    attr_prim_ref.set(ATTR_array);
    attr_prim_ref.set(ATTR_string);
    attr_prim_ref.set(ATTR_struct);
    attr_prim_ref.set(ATTR_null);
   rtypecheck(outfile, node);
   while(!stack.empty()){
      pop_block();         
   }
}


