SERFLAGS=-O2 -o cpu_version

all: serial

serial: cpu_version.cpp
	nvcc $(SERFLAGS) $^

clean: 
	rm -f *.o cpu_version
