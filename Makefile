NVFLAGS=-g -O2 -gencode arch=compute_20,code=sm_20 -gencode arch=compute_30,code=sm_30 -gencode arch=compute_35,code=sm_35
SERFLAGS=-O2 -o cpu_version
DISTFLAGS=-O2 -o dist_version
DISTCU=dist_kernel.o
MPI_LIB= -L/usr/local/cuda/lib64 -lcudart

all: serial dist

serial: cpu_version.cpp
	nvcc $(SERFLAGS) $^

dist: $(DISTCU) dist_version.cpp
	mpic++ $(DISTFLAGS) $^ $(MPI_LIB)

$(DISTCU): dist_kernel.cu
	nvcc $(NVFLAGS) $^ -c $(DISTCU)

clean: 
	rm -f *.o cpu_version
