EXE      = fleche
SRC      = $(wildcard src/*.cpp)
HDR      = $(wildcard src/*.h)
TEST_SRC = $(wildcard tests/*.cpp) $(filter-out src/main.cpp, $(SRC))
TEST_HDR = $(wildcard tests/*.h)

CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -fno-exceptions -fno-rtti
OPT      = -O3 -march=native -flto -DNDEBUG
DBG      = -O0 -g3 -fsanitize=address,undefined

ifeq ($(V),1)
  Q =
else
  Q = @
endif

BLUE  = \033[34m
GREEN = \033[32m
RESET = \033[0m

.PHONY: all debug test clean format magics

all: $(EXE)

$(EXE): $(SRC) $(HDR)
	@printf "  $(BLUE)CXX$(RESET)    %s $(GREEN)(release)$(RESET)\n" $@
	$(Q)$(CXX) $(CXXFLAGS) $(OPT) $(SRC) -o $@

debug: $(EXE)-debug

$(EXE)-debug: $(SRC) $(HDR)
	@printf "  $(BLUE)CXX$(RESET)    %s $(GREEN)(debug)$(RESET)\n" $@
	$(Q)$(CXX) $(CXXFLAGS) $(DBG) $(SRC) -o $@

test: $(EXE)-test
	$(Q)./$(EXE)-test

$(EXE)-test: $(TEST_SRC) $(HDR) $(TEST_HDR)
	@printf "  $(BLUE)CXX$(RESET)    %s $(GREEN)(tests)$(RESET)\n" $@
	$(Q)$(CXX) $(CXXFLAGS) $(DBG) -Isrc $(TEST_SRC) -o $@

magics:
	@printf "  $(BLUE)CXX$(RESET)    find_magics $(GREEN)(tool)$(RESET)\n"
	$(Q)$(CXX) $(CXXFLAGS) -O2 -Isrc tools/find_magics.cpp -o fleche-find_magics
	@printf "  $(BLUE)RUN$(RESET)    find_magics $(GREEN)(tool)$(RESET)\n"
		$(Q)./fleche-find_magics > src/magics.h.tmp && mv src/magics.h.tmp src/magics.h

clean:
	@printf "  $(BLUE)CLEAN$(RESET)\n"
	$(Q)rm -f $(EXE) $(EXE)-debug $(EXE)-test $(EXE)-find_magics src/magics.h.tmp

format:
	@printf "  $(BLUE)FORMAT$(RESET)\n"
	$(Q)clang-format -i $(SRC) $(HDR) $(wildcard tests/*.cpp) $(TEST_HDR) $(wildcard tools/*.cpp) $(wildcard tools/*.h)