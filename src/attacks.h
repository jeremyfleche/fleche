#pragma once

#include "position.h"
#include <array>

constexpr std::array<std::array<Bitboard, 64>, 2> init_pawn_attacks() {
	std::array<std::array<Bitboard, 64>, 2> res{};
	for (int square = 0; square < 64; ++square) {
		const int line = square / 8;
		const int column = square % 8;
		if (line < 7) {
			if (column > 0) {
				res[WHITE][square] |= sq_bb((line + 1) * 8 + column - 1);
			}
			if (column < 7) {
				res[WHITE][square] |= sq_bb((line + 1) * 8 + column + 1);
			}
		}
		if (line > 0) {
			if (column > 0) {
				res[BLACK][square] |= sq_bb((line - 1) * 8 + column - 1);
			}
			if (column < 7) {
				res[BLACK][square] |= sq_bb((line - 1) * 8 + column + 1);
			}
		}
	}
	return res;
}

constexpr std::array<Bitboard, 64> apply_moves_to_bitboard(const int (&moves)[8][2]) {
	std::array<Bitboard, 64> res{};
	for (int square = 0; square < 64; ++square) {
		const int line = square / 8;
		const int column = square % 8;
		Bitboard attacks = 0;
		for (const auto& move : moves) {
			int new_line = line + move[0];
			int new_column = column + move[1];
			if (new_line >= 0 && new_line < 8 && new_column >= 0 && new_column < 8) {
				attacks |= sq_bb(new_line * 8 + new_column);
			}
		}
		res[square] = attacks;
	}
	return res;
}

constexpr std::array<Bitboard, 64> init_king_attacks() {
	const int king_moves[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
	return apply_moves_to_bitboard(king_moves);
}

constexpr std::array<Bitboard, 64> init_knight_attacks() {
	const int knight_moves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
	return apply_moves_to_bitboard(knight_moves);
}

struct Magic {
	Bitboard mask;   // relevant blocker squares
	Bitboard magic;  // magic multiplier
	uint32_t offset; // start of this square's table in the big array
	uint8_t shift;   // 64 - number of bits in the mask
};

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

constexpr int ROOK_DIRECTIONS[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
constexpr int BISHOP_DIRECTIONS[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

constexpr std::array<Bitboard, 64> init_masks(const int (&directions)[4][2]) {
	std::array<Bitboard, 64> res{};
	for (int sq = 0; sq < 64; ++sq)
		res[sq] = ray_mask(sq, directions);
	return res;
}

constexpr Bitboard slow_rook_attacks(int sq, Bitboard occ) { return ray_attacks(sq, occ, ROOK_DIRECTIONS); }
constexpr Bitboard slow_bishop_attacks(int sq, Bitboard occ) { return ray_attacks(sq, occ, BISHOP_DIRECTIONS); }

inline constexpr std::array<Bitboard, 64> ROOK_MASK = init_masks(ROOK_DIRECTIONS);

inline constexpr std::array<Bitboard, 64> BISHOP_MASK = init_masks(BISHOP_DIRECTIONS);

extern std::array<Bitboard, 102400> ROOK_TABLE;
extern std::array<Bitboard, 5248> BISHOP_TABLE;

void attacks_init();

inline constexpr std::array<std::array<Bitboard, 64>, 2> PAWN_ATTACKS = init_pawn_attacks();
inline constexpr std::array<Bitboard, 64> KING_ATTACKS = init_king_attacks();
inline constexpr std::array<Bitboard, 64> KNIGHT_ATTACKS = init_knight_attacks();

constexpr Bitboard pawn_attacks(Color color, int square) { return PAWN_ATTACKS[color][square]; }

constexpr Bitboard king_attacks(int square) { return KING_ATTACKS[square]; }

constexpr Bitboard knight_attacks(int square) { return KNIGHT_ATTACKS[square]; }
