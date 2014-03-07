//MPI
#include <stdio.h>
#include <stdlib.h>


//c++ includes
/*
#include <thrust/transform_reduce.h>
#include <thrust/functional.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>*/
#include <cmath>


//#include <mpi.h>

using std;

int main(int argc, char **argv) {
   

   FILE *fp = fopen(argv[1],"r");
   fseek(fp, 0, SEEK_END);
   int fileSize = ftell(fp);
   int wordSize = 0;
   int wordLocation = 0;
   rewind(fp);

   char *fileData = (char *) malloc(fileSize);

   fread(fileData, sizeof(char), fileSize, fp);

   char **wordPtr = (char **) malloc (sizeof(char *) * fileSize/10);

   for (int i = 0; i < fileSize; i++) {
       *(fileData + i) = *(fileData + i) + 32;
       
       /*if((97 <= *(fileData + i) && *(fileData + i) <= 122))
          if (!wordSize) {
             *(wordPtr + wordLocation) = fileData + i;
              wordLocation++;
          }
          wordSize++;
       else if(wordSize) {
          *(fileData + i) = 0;
          wordSize = 0
       }*/
          

   values = (float*) malloc(sizeof(float) * num_values);
   fread(values,sizeof(float),num_values,fp);
   fclose(fp);

