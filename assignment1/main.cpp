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

#include "stringset.h"
#include "oc.h"
#include "auxlib.h"
 string CPP = "/usr/bin/cpp -nostdinc ";

int main(int argc, char** argv) {
	int opt;
	
	string dstring ="";
	while ((opt= getopt(argc, argv, "@:D:")) != -1) {
        switch (opt) {
            case '@': set_debugflags(optarg); break;
            case 'D': dstring= "-D" + std::string(optarg); break;
            default:  errprintf("%:bad option (%c)\n", optopt); break;
        }
    }
    //"if"   -kw_if
//const string CPP = "/usr/bin/cpp -nostdinc -%s", dstring.c_str();
    
    //CPP.append(dstring);

   const char* execname = basename (argv[0]);
   int exit_status = EXIT_SUCCESS;
   //for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[optind];
      //DEBUGF('f', "optarg is %s \n" , dstring);
      string command = CPP + dstring + " " + filename;
     
      string command1=filename;
      DEBUGF('f', "filename is %s \n", command1.c_str());
      //const char *newfile = filename << ".str";
     
      //printf ("command=\"%s\"\n", command.c_str());   //don't know what this removes yet
      FILE* pipe = popen (command.c_str(), "r");
      //printf("hello1 \n");
      if(command1.substr(command1.find_last_of(".")+1) != "oc"){
      	exit_status= EXIT_FAILURE;
      	string filenmerr= "Error: wrong file type, please use .oc file";
      	fprintf(stderr, "%s \n ", filenmerr.c_str());
      	return exit_status;
      	//sprintf("%s\n", filenmerr.c_str());
      	//break;
      }
      if (pipe == NULL) {
         exit_status = EXIT_FAILURE;
         fprintf (stderr, "%s: %s: %s\n",
 				execname, command.c_str(), strerror (errno));
      }else {
         cpplines (pipe, filename);
         //printf ("%s /n", (command1.c_str()));
      	 //std::printf("%s \n", filename);
      	 //char* buffer[100];
      	 //snprintf(buffer, 100, "%s.str", command1.c_str());
      	 //int sizec= sizeof(command1)
      	 command1.erase(command1.end()-3, command1.end());
      	 //command1.erase(std::find(command1.begin(), command1.end(), '.oc'));
      	 command1.append(".str");
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
//}



