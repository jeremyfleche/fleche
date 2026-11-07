#pragma once

#include "types.h"
#include <cstdint>
#include <cstdio>
#include <random>

static std::mt19937_64 rng(1234); // fixed seed: failures are reproducible

// Random bitboard: about 32, 16 or 8 bits set depending on density (0, 1 or 2).
inline Bitboard random_bits(int density) {
	Bitboard bits = rng();
	for (int i = 0; i < density; ++i)
		bits &= rng();
	return bits;
}

inline void display_bitboard(Bitboard bb, const char* label = nullptr) {
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