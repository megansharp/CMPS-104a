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

//const string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}


// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         //printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, inputname);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         const string* str = string_set::intern (token);
         DEBUGF('s',"intern (\"%s\") returned %p->\"%s\"\n", token, str, str->c_str());
         
         //printf ("[%s]\n", token);

      }
      ++linenr;
   }
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