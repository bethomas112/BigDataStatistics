//C libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//C++ libraries
#include <map>
#include <string>
#include <iostream>
#include <vector>

//Mpi
#include <mpi.h>

#include "dist_kernel.h"

using namespace std;


/* Returns true if string is all alphabetical*/
bool IsAlpha(char *toCheck) {
   int count;
   int len = strlen(toCheck);
   bool flag = true;

   for (count = 0; count < len && flag; count++) {
      if (flag = isalpha(toCheck[count])) {
         toCheck[count] = tolower(toCheck[count]);
      }
   }
   return flag;
}

map<string, int> countWords(FILE *fp, int fileSize) {
   map<string, int> words;
   char *fileData = (char *)malloc(fileSize);
   string temp;
   if (!fileData) {
      perror("Malloc");
      exit(1);
   }
   
   fread(fileData, sizeof(char),fileSize, fp);
   char *pch = strtok(fileData, " ,{}\"/=_()-<>`\'!.?:;\n ");
   while (pch) {
      if (IsAlpha(pch)) {
         words[string(pch)]++;
      }
      pch = strtok(NULL, " ,{}\"/=_()-<>`\'!.?:;\n");
   }
   
   return words;
}


int main(int argc, char **argv) {
   map<string, int> words;

   if (argc < 3) {
      fprintf(stderr, "Usage: %s <infile> <outfile.hist>\n", *argv);
      exit(1);
   }
   
   FILE *fp = fopen(argv[1],"r");
   fseek(fp, 0, SEEK_END);
   int fileSize = ftell(fp);
   rewind(fp);

   words = countWords(fp, fileSize); 
   FILE *outfile = fopen(argv[2], "w");
   if (!outfile) {
      perror("fopen");
      exit(1);
   }
   
   sortAndPrintMap(words, outfile);

   fclose(outfile);
   fclose(fp);
}
