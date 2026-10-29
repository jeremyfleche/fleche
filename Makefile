EXE    = fleche
SRC    = $(wildcard src/*.c)
HDR    = $(wildcard src/*.h)

CFLAGS = -std=c17 -Wall -Wextra -Wpedantic
OPT    = -O3 -march=native -flto -DNDEBUG
DBG    = -O0 -g3 -fsanitize=address,undefined

.PHONY: all debug clean

all: $(EXE)

$(EXE): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(OPT) $(SRC) -o $@ -lm

debug: $(EXE)-debug

$(EXE)-debug: $(SRC) $(HDR)
	$(CC) $(CFLAGS) $(DBG) $(SRC) -o $@ -lm

clean:
	rm -f $(EXE) $(EXE)-debug