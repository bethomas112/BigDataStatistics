//C libraries
#include <stdio.h>

//Thrust Libraries
#include <thrust/device_vector.h>
#include <thrust/sort.h>

//C++ Libraries
#include <iostream>

#include "dist_kernel.h"

using namespace std;

void sortAndPrintMap(map<string, int> words, FILE *outfile) {
   thrust::device_vector<const char *> d_keys;
   thrust::device_vector<int> d_values;

   for (map<string, int>::iterator it = words.begin(); it!=words.end();
    it++){
      d_keys.push_back(it->first.c_str());
      d_values.push_back(it->second);
   }

   thrust::host_vector<const char *> keys;
   thrust::host_vector<int> values;

   thrust::sort_by_key(d_values.begin(), d_values.end(), d_keys.begin(),
    thrust::greater<int>());

   keys = d_keys;
   values = d_values;
   thrust::host_vector<int>::iterator vit = values.begin();
   for (thrust::host_vector<const char *>::iterator it = keys.begin();
    it != keys.end(); it++, vit++) {
      fprintf(outfile, "%s, %d\n", *it, *vit);
   }
   
}
