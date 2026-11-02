EXE      = fleche
SRC      = $(wildcard src/*.cpp)
HDR      = $(wildcard src/*.h)

CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti
OPT      = -O3 -march=native -flto -DNDEBUG
DBG      = -O0 -g3 -fsanitize=address,undefined

.PHONY: all debug clean

all: $(EXE)

$(EXE): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(OPT) $(SRC) -o $@

debug: $(EXE)-debug

$(EXE)-debug: $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(DBG) $(SRC) -o $@

clean:
	rm -f $(EXE) $(EXE)-debug