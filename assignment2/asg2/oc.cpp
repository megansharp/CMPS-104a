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
#include "yyparse.h"
#include "lyutils.h"
#include "astree.h"
//const string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;

//extern char* yytext;
//int yylex();

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}


// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename) {
   string command1= filename;
   string command= filename;
   DEBUGF('s', 
      "I am in CPPlines, command1 is: %s, and command is: %s \n",
    command1.c_str(), command.c_str());
   command1.erase(command1.end()-3, command1.end());
   command1.append(".tok");
   FILE *tfile= fopen(command1.c_str(), "w+");
   //string header = lexer::filename(lexer::lloc.filenr);
   //yyparse();
   fprintf(tfile,"#%zd  \"%s\" \n", 
      lexer::lloc.linenr, command.c_str());
   DEBUGF ('s', "I have made the tok file \n");
   int linenr = 1;
   char inputname[LINESIZE];
   int dirc=0;
   int tokenct=0;
   strcpy (inputname, filename);
   for (;;) {
      DEBUGF('s', "I am in th for loop \n");
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      DEBUGF('s', "fgets_rc is not null \n");
      chomp (buffer, '\n');

      //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         //printf ("DIRECTIVE: line %d file \"%s\"\n", 
         //linenr, inputname);
         dirc++;
         continue;
      }
      //string_set::intern (yylex());
      //char* savepos = NULL;
     //char* bufptr = buffer;
      //int token;
     int n;
     while((n=yylex())!=0) {
         DEBUGF('s', "I have made it to the while loop \n");
         char* token= yytext;
         //bufptr= NULL;
         if (token== NULL) break;
         const string* str = string_set::intern (token);
         string ttype = parser::get_tname(n);
         fprintf(tfile, "%zd %2zd.%.03zd  %-3d  %-15s (%s)\n", 
            lexer::lloc.filenr, lexer::lloc.linenr, 
            lexer::lloc.offset, n, ttype.c_str(),token);
         DEBUGF('s',"intern (\"%s\") returned %p->\"%s\"", 
            token, str, str->c_str());
         tokenct++; 
         //printf ("[%s]\n", token);

     } 
      ++linenr;
   }
   int fclose_st = fclose(tfile);
   eprint_status (command1.c_str(), fclose_st);
   DEBUGF('s', "file close statues is %d \n", fclose_st);
   
}



int bob (int argc, char** argv) {
   for (int i = 1; i < argc; ++i) {
      const string* str = string_set::intern (argv[i]);
      printf ("intern (\"%s\") returned %p->\"%s\"\n",
              argv[i], str, str->c_str());
   }
   string_set::dump (stdout);
   return EXIT_SUCCESS;
}
