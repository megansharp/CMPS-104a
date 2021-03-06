// $Id: main.cpp,v 1.2 2016-08-18 15:13:48-07 - - $
// Megan Sharp mesharp@ucsc.edu
// Michelle Ly mly8@ucsc.edu

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


#include "stringset.h"
#include "oc.h"
#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"
 string CPP = "/usr/bin/cpp -nostdinc ";
 extern FILE* yyin; 
 extern int yy_flex_debug;
 extern int yydebug;
 extern astree* yyparse_astree;
 
int main(int argc, char** argv) {
    
   int opt;
    yy_flex_debug=0;
    string dstring ="";
    while ((opt= getopt(argc, argv, "ly@:D:")) != -1) {
        switch (opt) {
            case 'l': yy_flex_debug = 1; break;
            case 'y': yydebug=1; break;
            case '@': set_debugflags(optarg); break;
            case 'D': dstring= "-" + std::string(optarg); break;
            default:  errprintf("%:bad option (%c)\n", optopt); break;
        }
    }
    string execname = basename (argv[0]);
    exec::execname = basename (argv[0]);
   DEBUGF ('f',"Execname is %s \n", execname);
   int exit_status = EXIT_SUCCESS;

      char* filename = argv[optind];
      //DEBUGF('f', "optarg is %s \n" , dstring);
      string command = CPP + dstring + " " + filename;
     
      string command1=filename;
      DEBUGF('f', "filename is %s \n", command1.c_str());

      if(command1.substr(command1.find_last_of(".")+1) != "oc"){
        exit_status= EXIT_FAILURE;
        string filenmerr= "Error: wrong file type, please use .oc file";
        fprintf(stderr, "%s \n ", filenmerr.c_str());
        return exit_status;
      }
      FILE* pipe = popen(command.c_str(), "r");
      
      yyin = popen(command.c_str(), "r");
      DEBUGF('f',"I've made it to the pipe check \n");
      if (pipe == NULL) {
         DEBUGF('f',"Pipe is null \n");
         exit_status = EXIT_FAILURE;
         fprintf (stderr, "%s: %s: %s\n",
                execname.c_str(), command.c_str(), strerror (errno));
         return exit_status;
      }else{
         DEBUGF('f', "I've made it to the yyin check \n");
      
         DEBUGF ('f', "I have made it cpplines \n");
         lexer::newfilename (command);
         DEBUGF ('f', 
            "I have added lexer newfile name \n");
         cpplines(filename);
         yyparse();
         DEBUGF('f', "I have preformed cpplines \n");
         close_tfile();
         command1.erase(command1.end()-3, command1.end());
         string command2= command1;
         string command3= command1;
         command1.append(".str");
         command2.append(".tok");
         command3.append(".ast");
         FILE *nfile= fopen(command1.c_str(), "w+");
         DEBUGF ('f', "I have opened .str");
         string_set::dump(nfile);
         int fclose_st = fclose(nfile);
         eprint_status (command1.c_str(), fclose_st);
         if (fclose_st != 0) exit_status= EXIT_FAILURE;
         DEBUGF('f', "I have dumped to .str\n");
         FILE *afile=fopen(command3.c_str(), "w+");
         DEBUGF('f', "I have opened .ast\n");
         astree::print(afile, yyparse_astree);
         DEBUGF('f', "I have printed to .ast\n");
         //string_set::dump(stdout);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
         if (pclose_rc != 0) exit_status = EXIT_FAILURE;
         
        //int tclose_st = fclose(tfile);
        //eprint_status (command2.c_str(), tclose_st);
        //if (tclose_st != 0) exit_status= EXIT_FAILURE;
        int aclose_st = fclose(afile);
        eprint_status (command3.c_str(), aclose_st);
        if(aclose_st != 0) exit_status= EXIT_FAILURE;
         //fclose(nfile);
      
   }
   return exit_status;
}



