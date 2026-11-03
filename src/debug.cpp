#include "types.h"
#include <cstdio>

void display_bitboard(Bitboard bb) {
	for (int rank = 7; rank >= 0; --rank) {
		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			std::printf("%c", (bb & sq_bb(square)) ? '1' : '.');
		}
		std::printf("\n");
	}
}