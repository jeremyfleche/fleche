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

inline constexpr std::array<std::array<Bitboard, 64>, 2> PAWN_ATTACKS = init_pawn_attacks();
inline constexpr std::array<Bitboard, 64> KING_ATTACKS = init_king_attacks();
inline constexpr std::array<Bitboard, 64> KNIGHT_ATTACKS = init_knight_attacks();

constexpr Bitboard pawn_attacks(Color color, int square) { return PAWN_ATTACKS[color][square]; }
constexpr Bitboard king_attacks(int square) { return KING_ATTACKS[square]; }
constexpr Bitboard knight_attacks(int square) { return KNIGHT_ATTACKS[square]; }
