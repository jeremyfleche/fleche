#pragma once

#include "types.h"
#include <cstdint>
#include <cstdio>

void display_bitboard(Bitboard bb, const char* label = nullptr) {
	if (label)
		std::printf("  %s\n", label);

	std::printf("    ┌─────────────────┐\n");
	for (int rank = 7; rank >= 0; --rank) {
		std::printf("  %d │ ", rank + 1);
		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			std::printf("%s ", ((bb >> square) & 1ULL) ? "●" : "·");
		}
		std::printf("│\n");
	}
	std::printf("    └─────────────────┘\n");
	std::printf("      A B C D E F G H\n\n");

	std::printf("  count : %d\n", __builtin_popcountll(bb));

	std::printf("  cases : ");
	for (Bitboard b = bb; b; b &= b - 1) {
		int sq = __builtin_ctzll(b);
		std::printf("%c%d ", 'A' + (sq & 7), (sq >> 3) + 1);
	}
	std::printf("\n");
}