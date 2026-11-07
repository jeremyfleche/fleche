#pragma once

#include "types.h"
#include <array>

constexpr int ROOK_DIRECTIONS[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
constexpr int BISHOP_DIRECTIONS[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

constexpr bool on_board(int line, int column) { return line >= 0 && line < 8 && column >= 0 && column < 8; }
constexpr bool is_occupied(Bitboard occ, int line, int column) { return (occ & sq_bb(line * 8 + column)) != 0; }

constexpr Bitboard ray_mask(int sq, const int (&directions)[4][2]) {
	Bitboard res = 0;
	for (const auto& d : directions) {
		int line = sq / 8 + d[0];
		int column = sq % 8 + d[1];
		while (on_board(line + d[0], column + d[1])) {
			res |= sq_bb(line * 8 + column);
			line += d[0];
			column += d[1];
		}
	}
	return res;
}

constexpr Bitboard ray_attacks(int sq, Bitboard occ, const int (&directions)[4][2]) {
	Bitboard res = 0;
	for (const auto& d : directions) {
		int line = sq / 8 + d[0];
		int column = sq % 8 + d[1];
		while (on_board(line, column)) {
			res |= sq_bb(line * 8 + column);
			if (is_occupied(occ, line, column)) {
				break;
			}
			line += d[0];
			column += d[1];
		}
	}
	return res;
}

constexpr Bitboard slow_rook_attacks(int sq, Bitboard occ) { return ray_attacks(sq, occ, ROOK_DIRECTIONS); }
constexpr Bitboard slow_bishop_attacks(int sq, Bitboard occ) { return ray_attacks(sq, occ, BISHOP_DIRECTIONS); }

constexpr std::array<Bitboard, 64> init_masks(const int (&directions)[4][2]) {
	std::array<Bitboard, 64> res{};
	for (int sq = 0; sq < 64; ++sq)
		res[sq] = ray_mask(sq, directions);
	return res;
}

inline constexpr std::array<Bitboard, 64> ROOK_MASK = init_masks(ROOK_DIRECTIONS);
inline constexpr std::array<Bitboard, 64> BISHOP_MASK = init_masks(BISHOP_DIRECTIONS);