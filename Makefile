all :
	g++-5 -pg -g -O0 -mavx2 SearcherAVX2.cpp SearcherSSE.cpp main.cpp -std=gnu++11 -o sg_debug
	g++-5 -pg -g -O3 -mavx2 SearcherAVX2.cpp SearcherSSE.cpp main.cpp -std=gnu++11 -o sg_release
