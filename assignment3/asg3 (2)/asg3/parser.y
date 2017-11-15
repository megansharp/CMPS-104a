%{
// Dummy parser for scanner project.

#include <cassert>

#include "lyutils.h"
#include "astree.h"


%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token ROOT IDENT NUMBER
%token TOK_VOID TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GEs
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL TOK_FUNC
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD TOK_INDEX
%token TOK_ORD TOK_CHR TOK_ROOT TOK_DECLID TOK_VARDECL TOK_PRAM
%token TOK_RETURNVOID TOK_PROTOTYPE TOK_NEWSTRING

%right TOK_IF TOK_ELSE
%right  '='
%left TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left   '+' '-'
%left   '*' '/' '%'
%right  '^'
%right  TOK_POS TOK_NEG '!' TOK_NEW
%left TOK_NEWARRAY TOK_FIELD TOK_FUNC
%left '[' '.'
%nonassoc '('

%start start


%%
start : program                  {yyparse_astree = $1; }
    ;
program : program structdef      { $$ = $1->adopt ($2); }
    | program function           { $$ = $1->adopt ($2); }
    | program statement          { $$ = $1->adopt ($2); }
    | program error '}'          { $$ = $1; }
    | program error ';'          { $$ = $1; }
    |                            { $$ = new_parseroot (); }
    ;

statement: block                 {$$ = $1; }
     | vardecl                   {$$ = $1; }
     | while                     {$$ = $1; }
     | ifelse                    {$$ = $1; }
     | return                    {$$ = $1; }
     | expr ';'                  {$$ = $1; }
     | ';'                       {$$ = $1; }
     ;
expr : expr '=' expr             { $$ = $2->adopt ($1, $3); }
     | expr '+' expr             { $$ = $2->adopt ($1, $3); }
     | expr '-' expr             { $$ = $2->adopt ($1, $3); }
     | expr '*' expr             { $$ = $2->adopt ($1, $3); }
     | expr '/' expr             { $$ = $2->adopt ($1, $3); }
     | expr '^' expr             { $$ = $2->adopt ($1, $3); }
     | expr TOK_EQ expr          { $$ = $2->adopt ($1, $3); }
     | expr TOK_NE expr          { $$ = $2->adopt ($1, $3); }
     | expr TOK_LT expr          { $$ = $2->adopt ($1, $3); }
     | expr TOK_LE expr          { $$ = $2->adopt ($1, $3); }
     | expr TOK_GT expr          { $$ = $2->adopt ($1, $3); }
     | expr TOK_GE expr          { $$ = $2->adopt ($1, $3); }
     | '+' expr %prec TOK_POS    { $$ = $1->adopt_sym ($2, TOK_POS); }
     | '-' expr %prec TOK_NEG    { $$ = $1->adopt_sym ($2, TOK_NEG); }
     | '!' expr                  { $$ = $1->adopt($2); }
     | '(' expr ')'              { $$ = $2; }
     | allocator                 { $$ = $1; }
     | constant                  { $$ = $1; }
     | variable                  { $$ = $1; }
     | call                      { $$ = $1; }
     ;
        
structdef: TOK_STRUCT IDENT  fielddecls '}'             
                                 { $$ = $1->adopt_sym2($2, TOK_TYPEID); 
                                 $$= $1 -> adopt($3);}
    | TOK_STRUCT IDENT '{' '}'   { $$ = $1->adopt_sym2($2, 
                                 TOK_TYPEID);} 
    ;
fielddecls: fielddecls  fielddecl ';'                   
                                 { $$ = $1->adopt($2); }
    |  '{' fielddecl ';'         { $$ = $1->adopt($2); }
    ;
fielddecl: basetype TOK_NEWARRAY IDENT                  
                                 { $$ = $2->adopt_sym2($3, TOK_FIELD); 
                                 $$=$2->adopt($1);}
    | basetype IDENT             { $$ = $1->adopt_sym2($2, TOK_FIELD); }
    ;
basetype: TOK_VOID               { $$ = $1; }
     | TOK_INT                   { $$ = $1; }
     | TOK_STRING                { $$ = $1; }
     | TOK_CHAR                  { $$ = $1; }
     | IDENT                     { $$ = swap_symb($1, TOK_TYPEID);}
     ;
function : identdecl  identdecls ')' block              
                                {$$= new astree(TOK_FUNC,  
                                {$1->lloc.filenr,
                                $1->lloc.linenr, $1->lloc.offset},"");
                                $$=$$->adopt($1, $2); 
                                $$=$$->adopt($4);  }
     | identdecl '(' ')' block                          
                                { $2=swap_symb($2, TOK_PRAM);
                                $$= new astree(TOK_FUNC, 
                                {$1->lloc.filenr,
                                $1->lloc.linenr, $1->lloc.offset},"");
                                $$=$$->adopt($1, $2); $$=$$->adopt($4);}
     | identdecl '(' ')' ';'                            
                                {$2=swap_symb($2, TOK_PRAM);
                                $$= new astree(TOK_PROTOTYPE, 
                                {$1->lloc.filenr,
                                $1->lloc.linenr, $1->lloc.offset},"");
                                $$=$$->adopt($1, $2);}
    | identdecl identdecls ')' ';'                      
                                {$$= new astree(TOK_PROTOTYPE, 
                                {$1->lloc.filenr,
                                $1->lloc.linenr, $1->lloc.offset},"");
                                $$=$$->adopt($1, $2); 
                                $$=$$->adopt($4);}
    ;
identdecls: identdecls ',' identdecl                    
                                { $$ = $1->adopt($3); }
    | '(' identdecl             { $$ = $1->adopt_sym($2, TOK_PRAM); }
    ;
identdecl: basetype TOK_NEWARRAY IDENT                  
                                { $$ = $1->adopt_sym2($2, TOK_DECLID); 
                                $$ = $1->adopt($2); }
    | basetype IDENT            { $$ = $1->adopt_sym2($2, TOK_DECLID);}
    ;
block:  statements '}'          { $$ = $1; }
    | '{' '}'                   { $$ = $$->adopt_sym2($1, TOK_BLOCK);}
    ;
statements: statements statement                        
                                { $$ = $1->adopt($2); }
    | '{' statement             { $$= $1->adopt_sym($2, TOK_BLOCK);}
    ;
vardecl : identdecl '=' expr ';'                        
                                { $$ = $2->adopt_sym($1, TOK_VARDECL);
                                $$=$2->adopt($3);} 
    ;
while   : TOK_WHILE '(' expr ')' statement              
                                { $$ = $1->adopt($3,$5); }
    ;
ifelse  : TOK_IF '(' expr ')' statement %prec TOK_ELSE                
                                { $$ = $1->adopt($3,$5); }
    | TOK_IF '(' expr ')' statement TOK_ELSE statement  
                                { $$ = $1->adopt_sym($3,TOK_IFELSE);
                                $$= $1->adopt($5, $7); }
    ;
return  : TOK_RETURN expr ';'   { $$ = $1->adopt($2); }
    | TOK_RETURN ';'            { $$ = swap_symb($1, TOK_RETURNVOID); }
    ;
allocator: TOK_NEW IDENT '(' ')'
                                { $$ = $1->adopt_sym2($2, TOK_TYPEID); }
    | TOK_NEW TOK_STRING '(' expr ')'                   
                                { $$ = $2->adopt_sym($4, 
                                TOK_NEWSTRING);}
    | TOK_NEW basetype '[' expr ']'                     
                                { $$ = $1->adopt_sym($2, TOK_NEWARRAY); 
                                $$ = $1->adopt($3);}
    ; 
call: exprs ')'                 { $$ = $1; }
    | IDENT '(' ')'             { $$ = $2->adopt_sym($1, TOK_CALL); }
    ;
exprs: exprs ',' expr           { $$ = $1; $$=$3; }
    | IDENT '(' expr            { $$ = $2->adopt_sym($1, TOK_CALL); 
                                $$=$2->adopt($3);}
    ;
variable: IDENT                 { $$ = $1; }
    | expr '[' expr ']'         { $$ = $2->adopt_sym($3, TOK_INDEX); 
                                $$=$2->adopt($1); }
    | expr '.' IDENT            { $$ = $2->adopt_sym2($3,TOK_FIELD);
                                $$ = $2-> adopt($1); }
    ;
constant: TOK_INTCON            { $$ = $1; }
    | TOK_CHARCON               { $$ = $1; }
    | TOK_STRINGCON             { $$ = $1; }
    | TOK_NULL                  { $$ = $1; }
    ;
%%


const char *parser::get_tname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}


bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

/*
static void* yycalloc (size_t size) {
   void* result = calloc (1, size);
   assert (result != nullptr);
   return result;
}
*/

