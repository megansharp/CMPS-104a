#ifndef __OC_H__
#define __OC_H__
// Megan Sharp mesharp@ucsc.edu
// Michelle Ly mly8@ucsc.edu
#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

//const string CPP = "/usr/bin/cpp -nostdinc";
//constexpr size_t LINESIZE = 1024;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim);

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename);

int bob (int argc, char** argv);

#endif