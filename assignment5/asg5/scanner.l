/* $Id: lexer.l,v 1.7 2016-10-27 19:45:59-07 - - $ */
/* Megan Sharp mesharp@ucsc.edu */
/* Michelle Ly mly8@ucsc.edu */
%{
#include "auxlib.h"
#include "lyutils.h"


#define YY_USER_ACTION  { lexer::advance(); }



%}

%option 8bit
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
/*%option verbose*/

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          ({MANTISSA}{EXPONENT}?)
NOTNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
CHARCON         ('([^\\'\n]|\\[\\'"0nt])')
STRINGCON       (\"([^\\"\n]|\\[\\'"0nt])*\")
BADCHAR         ('([^\\'\n]|\\[\\'"0nt]))
BADSTRING       (\"([^\\"\n]|\\[\\'"0nt])*)
BADIDENT        ({DIGIT}({LETTER}|{DIGIT})*)


%%

"#".*           { lexer::include(); }
[ \t]+          { }
\n              { lexer::newline(); }

"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }
"%"             { return yylval_token ('%');}
"!"             { return yylval_token('!');}
"=="            { return yylval_token(TOK_EQ);}
"!="            { return yylval_token(TOK_NE);}
"<"             { return yylval_token(TOK_LT);}
"<="            { return yylval_token(TOK_LE);}
">"             { return yylval_token(TOK_GT);}
">="            { return yylval_token(TOK_GE);}
","             { return yylval_token(',');}
"."             { return yylval_token('.');}
"[]"            { return yylval_token(TOK_ARRAY);}
"["             { return yylval_token('[');}
"]"             { return yylval_token(']');}
"{"             { return yylval_token('{');}
"}"             { return yylval_token('}');}
"if"            { return yylval_token(TOK_IF);}
"else"          { return yylval_token(TOK_ELSE);}
"while"         { return yylval_token(TOK_WHILE);}
"return"        { return yylval_token(TOK_RETURN);}
"int"           { return yylval_token(TOK_INT);}
"string"        { return yylval_token(TOK_STRING);}
"struct"        { return yylval_token(TOK_STRUCT);}
"void"          { return yylval_token(TOK_VOID);}
"new"           { return yylval_token(TOK_NEW);}
"null"          { return yylval_token(TOK_NULL);}
"char"          { return yylval_token(TOK_CHAR);}


{CHARCON}       {return yylval_token(TOK_CHARCON);}
{STRINGCON}     {return yylval_token(TOK_STRINGCON);}
{NUMBER}        {return yylval_token(TOK_INTCON); }
{IDENT}         {return yylval_token(TOK_IDENT); }


{NOTNUMBER}     { lexer::badtoken (yytext);
                  return yylval_token (TOK_INTCON); }
{BADSTRING}     {lexer::badtoken(yytext);
                    return yylval_token(TOK_STRINGCON);}
{BADCHAR}       {lexer::badtoken(yytext);
                    return yylval_token(TOK_CHARCON);}
{BADIDENT}      {lexer::badtoken(yytext);
                    return yylval_token(TOK_IDENT);}

.               { lexer::badchar (*yytext); }

%%
