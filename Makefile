all: build

build: pp_ps.cc
	g++ -o pp_ps -std=c++11  pp_ps.cc
clean:
	rm pp_ps
