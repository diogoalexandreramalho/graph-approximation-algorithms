CXX      = g++
CXXFLAGS = -std=c++11 -Wall -O2 -Iinclude

SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
BIN  = graph-approx

all: $(BIN)

$(BIN): $(OBJS)
	$(CXX) $(OBJS) -o $@

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean
