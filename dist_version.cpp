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

map<string, int> countWords(int fd, unsigned fileSize, int commRank, int commSize) {
   map<string, int> words; 
   char *pch;
   unsigned size = fileSize/commSize;
   char buf;
   unsigned start = size * commRank;
   unsigned end = size * (commRank + 1);

   lseek(fd, start, SEEK_SET);

   if (commRank) {
      read(fd, &buf, 1);
      //printf("---------------------%c %i\n", buf, commRank);
      while (! strchr(" ,{}\"/=_()-<>`\'!.?:;\n ", buf)) {
         if (start == size * (commRank)) {
            start++;
         }
         start++;
         read(fd, &buf, 1);
      }
   }
   lseek(fd, end, SEEK_SET);

   if (commRank != commSize - 1) {
      read(fd, &buf, 1);
      while (!strchr(" ,{}\"/=_()-<>`\'!.?:;\n ", buf)) {
         if (end == size * (commRank + 1))
            end++;
         end++;
         read(fd, &buf, 1);
      } 
   }
   else 
      end = fileSize;

   //fileSize = commRank == commSize - 1 ? size + commSize : size;
   lseek(fd, start, SEEK_SET);
   char *fileData = (char *)malloc(sizeof(char) * (end - start + commSize));
   if (!fileData) {
      perror("Malloc");
      exit(1);
   }
   
   read(fd, fileData, end - start);

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
   

   //printf("%s\n", mpiOut.c_str());

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
   char buf[50000];
   char t[150];
   int count = 0;
   FILE * in;
   string s;

   for(int i = 1; i < commSize; i++) {
      sprintf(t, "out%d.hist", i);
      string temp(t);
      in = fopen(temp.c_str(), "r");
      //cout << "Combining map: " << i << "\n";
      while(fscanf(in, "%s %d", buf, &count) != EOF) {
         string ts(buf);
         words[ts] += count;
      }
      //cout << "Finished combining map: " << i << "\n"; 
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
   unsigned fileSize = lseek(fd, 0, SEEK_END);

   //cout << "Before CountWords, CommRank: " << commRank << "\n";
   words = countWords(fd, fileSize, commRank, commSize);
   //cout << "After Countwords, CommRank: " << commRank << "\n";

   if (commRank) {
      //cout << "Entering mpiPrint, CommRank: " << commRank << "\n";
      mpiPrint(commRank, words);
      //cout << "Finished mpiPrint, CommRank: " << commRank << "\n";
   }

   MPI_Barrier(MPI_COMM_WORLD);
   
   //cout << "Passed Barrier, CommRank: " << commRank << "\n";
   
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
