# sample Makefile
RES = src/main.cpp
EXE = pa1
all:
	g++ -std=c++11 $(RES) -o $(EXE) -O2
clean:
	rm $(EXE)
