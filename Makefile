EXE      = fleche
SRC      = $(wildcard src/*.cpp)
TEST_SRC = $(wildcard tests/*.cpp) $(filter-out src/main.cpp, $(SRC))
HDR      = $(wildcard src/*.h)

CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti
OPT      = -O3 -march=native -flto -DNDEBUG
DBG      = -O0 -g3 -fsanitize=address,undefined

.PHONY: all debug test clean format

all: $(EXE)

$(EXE): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(OPT) $(SRC) -o $@

debug: $(EXE)-debug

$(EXE)-debug: $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(DBG) $(SRC) -o $@

test: $(EXE)-test
	./$(EXE)-test

$(EXE)-test: $(TEST_SRC) $(HDR) tests/test.h
	$(CXX) $(CXXFLAGS) $(DBG) -Isrc $(TEST_SRC) -o $@

clean:
	rm -f $(EXE) $(EXE)-debug

format:
	clang-format -i src/*.cpp src/*.h tests/*.cpp tests/*.h