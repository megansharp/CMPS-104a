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
//#include "yyparse.h"
#include "lyutils.h"
 string CPP = "/usr/bin/cpp -nostdinc ";
 extern FILE* yyin; 
 extern int yy_flex_debug;
 
int main(int argc, char** argv) {
    
   int opt;
    yy_flex_debug=0;
    string dstring ="";
    while ((opt= getopt(argc, argv, "ly@:D:")) != -1) {
        switch (opt) {
            case 'l': yy_flex_debug = 1; break;
            case 'y': break;
            case '@': set_debugflags(optarg); break;
            case 'D': dstring= "-" + std::string(optarg); break;
            default:  errprintf("%:bad option (%c)\n", optopt); break;
        }
    }
    string execname = basename (argv[0]);
    exec::execname = basename (argv[0]);
   DEBUGF ('f',"Execname is %s \n", execname);
   int exit_status = EXIT_SUCCESS;
//const string CPP = "/usr/bin/cpp -nostdinc -%s", dstring.c_str();
    
    //CPP.append(dstring);

   //const char* execname = basename (argv[0]);
   //int exit_status = EXIT_SUCCESS;
   //for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[optind];
      //DEBUGF('f', "optarg is %s \n" , dstring);
      string command = CPP + dstring + " " + filename;
     
      string command1=filename;
      DEBUGF('f', "filename is %s \n", command1.c_str());
      //const char *newfile = filename << ".str";
     
      //printf ("command=\"%s\"\n", command.c_str());  
     //don't know what this removes yet
      
     /* if (yyin== NULL) {
      syserrprintf (command.c_str());
      fprintf (stderr, "%s: %s: %s\n",
            execname, command.c_str(), strerror (errno));
         return exit_status;
      }else {
      if (yy_flex_debug) {
         fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                  command.c_str(), fileno (yyin));
      }
      lexer::newfilename (command);
   }*/
      //printf("hello1 \n");
      if(command1.substr(command1.find_last_of(".")+1) != "oc"){
        exit_status= EXIT_FAILURE;
        string filenmerr= "Error: wrong file type, please use .oc file";
        fprintf(stderr, "%s \n ", filenmerr.c_str());
        return exit_status;
        //sprintf("%s\n", filenmerr.c_str());
        //break;
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
      }
      if (yyin == NULL) {
      exit_status=EXIT_FAILURE;
      DEBUGF('f', "yyin is null \n");
      fprintf (stderr, "%s: %s: %s\n",
            execname.c_str(), command.c_str(), strerror (errno));
         return exit_status;
      }else{
         DEBUGF ('f', "I have made it cpplines \n");
         lexer::newfilename (command);
         DEBUGF ('f', 
            "I have added lexer newfile name \n");
         cpplines (pipe, filename);
         DEBUGF('f', "I have preformed cpplines \n");
         //printf ("%s /n", (command1.c_str()));
         //std::printf("%s \n", filename);
         //char* buffer[100];
         //snprintf(buffer, 100, "%s.str", command1.c_str());
         //int sizec= sizeof(command1)
         command1.erase(command1.end()-3, command1.end());
          //string command2= command1;
         command1.append(".str");
          //command2.append(".tok");
         FILE *nfile= fopen(command1.c_str(), "w+");
         string_set::dump(nfile);
         //string_set::dump(stdout);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
         if (pclose_rc != 0) exit_status = EXIT_FAILURE;
         int fclose_st = fclose(nfile);
         eprint_status (command1.c_str(), fclose_st);
         if (fclose_st != 0) exit_status= EXIT_FAILURE;
         //fclose(nfile);
      
   }
   return exit_status;
}



