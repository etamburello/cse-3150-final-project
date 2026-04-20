CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -DNDEBUG -Iinclude

SRC = src/main.cpp src/AS.cpp src/graph.cpp src/bgp.cpp src/rov.cpp
OUT = program.out

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
