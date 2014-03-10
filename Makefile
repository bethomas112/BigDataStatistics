SERFLAGS=-O2 -o cpu_version
DISTFLAGS=-O2 -o dist_version

all: serial

serial: cpu_version.cpp
	nvcc $(SERFLAGS) $^

dist: dist_version.cpp
	nvcc $(DISTFLAGS) $^ 

clean: 
	rm -f *.o cpu_version
