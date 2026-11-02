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
    const int king_moves[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };
    for (int square = 0; square < 64; ++square) {
        const int line = square / 8;
        const int column = square % 8;
        Bitboard attacks = 0;
        for (const auto& move: king_moves) {
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
    const int knight_moves[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };
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

static_assert(pawn_attacks(WHITE, a2) == bb(b3));                               // white pawn on a2 can attack b3
static_assert(pawn_attacks(BLACK, h7) == bb(g6));                               // black pawn on h7 can attack g6
static_assert(pawn_attacks(WHITE, f3) == bb(e4, g4));                           // white pawn on f3 can attack e4 and g4
static_assert(pawn_attacks(BLACK, e6) == bb(d5, f5));                           // black pawn on d6 can attack c5 and e5

static_assert(knight_attacks(a1) == bb(b3, c2));                                // knight on a1 can attack b3 and c2
static_assert(knight_attacks(h8) == (bb(f7, g6)));                              // knight on h8 can attack f7 and g6
static_assert(knight_attacks(d4) == (bb(c2, e2, f3, f5, e6, c6, b5, b3)));      // knight on d4 can attack c2, e2, f3, f5, e6, c6, b5, b3

static_assert(king_attacks(e1) == (bb(d1, d2, e2, f2, f1)));                    // king on e1 can attack d1, d2, e2, f2, f1
static_assert(king_attacks(e8) == (bb(d8, d7, e7, f7, f8)));                    // king on e8 can attack d8, d7, e7, f7, f8
static_assert(king_attacks(a1) == (bb(a2, b2, b1)));                            // king on a1 can attack a2, b2, b1
static_assert(king_attacks(h8) == (bb(g8, g7, h7)));                            // king on h8 can attack g8, g7, h7
static_assert(king_attacks(d4) == (bb(c3, c4, c5, d3, d5, e3, e4, e5)));        // king on d4 can attack c3, c4, c5, d3, d5, e3, e4, e5

constexpr Bitboard pawn_attacks(Color color, int square) { return PAWN_ATTACKS[color][square]; }
constexpr Bitboard king_attacks(int square) { return KING_ATTACKS[square]; }
constexpr Bitboard knight_attacks(int square) { return KNIGHT_ATTACKS[square]; }