SERFLAGS=-O2 -o cpu_version

all: serial

serial: cpu_version.cpp
	g++ $(SERFLAGS) $^

clean: 
	rm -f *.o cpu_version
