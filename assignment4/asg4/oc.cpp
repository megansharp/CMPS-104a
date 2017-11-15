// $Id: cppstrtok.cpp,v 1.7 2016-09-30 11:45:04-07 - - $
// Megan Sharp mesharp@ucsc.edu
// Michelle Ly mly8@ucsc.edu
// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include "auxlib.h"
#include "stringset.h"
#include "oc.h"
#include "lyutils.h"
#include "astree.h"
//const string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
//extern char* yytext;
//int yylex();
FILE* tfile;
string command1= "";
// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}


// Run cpp against the lines of the file.
void cpplines (const char* filename) {
   string command1= filename;
   string command= filename;
   DEBUGF('s', 
      "I am in CPPlines, command1 is: %s, and command is: %s \n",
    command1.c_str(), command.c_str());
   command1.erase(command1.end()-3, command1.end());
   command1.append(".tok");
   tfile= fopen(command1.c_str(), "w+");
   //string header = lexer::filename(lexer::lloc.filenr);
   //yyparse();
   fprintf(tfile,"#%zd  \"%s\" \n", 
      lexer::lloc.linenr, command.c_str());
    DEBUGF ('s', "I have made the tok file \n");

}



void tokenmakr (int n, char* token) {
         DEBUGF('s', "I am in token maker with %s \n", token);
       //char* token= yytext;
         //bufptr= NULL;
         const string* str = string_set::intern (token);
         string ttype = parser::get_tname(n);
         //astree(n, lexer::lloc, token);
         fprintf(tfile, "%zd %2zd.%.03zd  %-3d  %-15s (%s)\n", 
            lexer::lloc.filenr, lexer::lloc.linenr, 
            lexer::lloc.offset, n, ttype.c_str(),token);
         DEBUGF('s',"intern (\"%s\") returned %p->\"%s\"", 
            token, str, str->c_str());
} 
void close_tfile(){
       int tclose_st = fclose(tfile);
        //eprint_status (command1.c_str(), tclose_st);
        DEBUGF ('s', " I colsed t-file %d \n", tclose_st);

}
