all :
	g++-4.9 -pg -g -O0 -march=native SearcherAVX2.cpp SearcherCPU.cpp main.cpp -std=gnu++11 -o sg_debug
	g++-4.9 -pg -g -O3 -march=native SearcherAVX2.cpp SearcherCPU.cpp main.cpp -std=gnu++11 -o sg_release
