#pragma once

#include <array>
#include "types.h"

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

constexpr std::array<Bitboard, 64> init_king_attacks() {
	std::array<Bitboard, 64> res{};
	const int king_moves[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
	for (int square = 0; square < 64; ++square) {
		const int line = square / 8;
		const int column = square % 8;
		Bitboard attacks = 0;
		for (const auto& move : king_moves) {
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

constexpr std::array<Bitboard, 64> init_knight_attacks() {
	std::array<Bitboard, 64> res{};
	const int knight_moves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
	for (int square = 0; square < 64; ++square) {
		Bitboard attacks = 0;
		const int line = square / 8;
		const int column = square % 8;

		for (const auto& move : knight_moves) {
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

inline constexpr std::array<std::array<Bitboard, 64>, 2> PAWN_ATTACKS = init_pawn_attacks();
inline constexpr std::array<Bitboard, 64> KING_ATTACKS = init_king_attacks();
inline constexpr std::array<Bitboard, 64> KNIGHT_ATTACKS = init_knight_attacks();

constexpr Bitboard pawn_attacks(Color color, int square) { return PAWN_ATTACKS[color][square]; }
constexpr Bitboard king_attacks(int square) { return KING_ATTACKS[square]; }
constexpr Bitboard knight_attacks(int square) { return KNIGHT_ATTACKS[square]; }

static_assert(pawn_attacks(WHITE, A2) == bb(B3));
static_assert(pawn_attacks(BLACK, H7) == bb(G6));
static_assert(pawn_attacks(WHITE, F3) == bb(E4, G4));
static_assert(pawn_attacks(BLACK, E6) == bb(D5, F5));

static_assert(knight_attacks(A1) == bb(B3, C2));
static_assert(knight_attacks(H8) == (bb(F7, G6)));
static_assert(knight_attacks(D4) == (bb(C2, E2, F3, F5, E6, C6, B5, B3)));

static_assert(king_attacks(E1) == (bb(D1, D2, E2, F2, F1)));
static_assert(king_attacks(E8) == (bb(D8, D7, E7, F7, F8)));
static_assert(king_attacks(A1) == (bb(A2, B2, B1)));
static_assert(king_attacks(H8) == (bb(G8, G7, H7)));
static_assert(king_attacks(D4) == (bb(C3, C4, C5, D3, D5, E3, E4, E5)));
