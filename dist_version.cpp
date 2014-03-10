//C libraries
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


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

map<string, int> countWords(int fd, int fileSize, int commRank, int commSize) {
   map<string, int> words; 
   char *pch;
   int size = fileSize/commSize + 4;

   lseek(fd, size * commRank, SEEK_SET);

   //fileSize = commRank == commSize - 1 ? size + commSize : size;
   char *fileData = (char *)malloc(size);
   if (!fileData) {
      perror("Malloc");
      exit(1);
   }
   
   read(fd, fileData, size);

   pch = strtok(fileData, " ,{}\"/=_()-<>`\'!.?:;\n ");
   while (pch) {
      if (IsAlpha(pch)) {
         words[string(pch)]++;
      }
      pch = strtok(NULL, " ,{}\"/=_()-<>`\'!.?:;\n");
   }
   
   return words;
}

void mpiPrint(int commRank, map<string, int> words) {
   char t[15];

   sprintf(t, "out%d.hist", commRank);
   string mpiOut(t);
   

   printf("%s\n", mpiOut.c_str());

   FILE *outfile = fopen(mpiOut.c_str(), "w");
   if (!outfile) {
      perror("MPI fopen");
      exit(1);
   }

   for (map<string, int>::iterator it=words.begin(); it!=words.end(); it++){
      fprintf(outfile,"%s %d\n", it->first.c_str(), it->second);
   }

   fclose(outfile);
}

map<string, int> combineMaps(map<string, int> words,int commSize) {
   char buf[100];
   char t[15];
   int count = 0;
   FILE * in;
   string s;

   for(int i = 1; i < commSize; i++) {
      sprintf(t, "out%d.hist", i);
      string temp(t);
      in = fopen(temp.c_str(), "r");
      while(fscanf(in, "%s %d", buf, &count)) {
         string ts(buf);
         words[ts] += count;
      }
      fclose(in);
    }

   return words;
}

int main(int argc, char **argv) {
   map<string, int> words;
   int commSize, commRank;

   if (argc < 3) {
      fprintf(stderr, "Usage: %s <infile> <outfile.hist>\n", *argv);
      exit(1);
   }
   
   MPI_Init(&argc, &argv);
    
   MPI_Comm_size(MPI_COMM_WORLD, &commSize);
   MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
   
   int fd = open(argv[1], O_RDONLY);
   if (0 > fd) {
      perror("fd");
      exit(1);
   }
   int fileSize = lseek(fd, 0, SEEK_END);

   cout << "Before CountWords, CommRank: " << commRank << "\n";
   words = countWords(fd, fileSize, commRank, commSize);
   cout << "After Countwords, CommRank: " << commRank << "\n";

   if (commRank) {
      mpiPrint(commRank, words);
   }

   MPI_Barrier(MPI_COMM_WORLD);

   if (!commRank) {
      words = combineMaps(words, commSize);
      FILE *outfile = fopen(argv[2], "w");
      if (!outfile) {
         perror("fopen");
         exit(1);
      }

      sortAndPrintMap(words, outfile);
      fclose(outfile);
   }
   



   MPI_Finalize();

   close(fd);
}
