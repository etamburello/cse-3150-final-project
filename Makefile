CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

SRC = src/main.cpp src/AS.cpp src/graph.cpp src/parser.cpp
OUT = program.out

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run:
	./$(OUT)

clean:
	rm -f $(OUT)
