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
                res[0][square] |= 1ULL << ((line + 1) * 8 + column - 1);
            }
            if (column < 7) {
                res[0][square] |= 1ULL << ((line + 1) * 8 + column + 1);
            }
        }
        if (line > 0) {
            if (column > 0) {
                res[1][square] |= 1ULL << ((line - 1) * 8 + column - 1);
            }
            if (column < 7) {
                res[1][square] |= 1ULL << ((line - 1) * 8 + column + 1);
            }
        }
    }
    return res;
}

constexpr std::array<Bitboard, 64> init_king_attacks() {
    std::array<Bitboard, 64> res{};
    const int kingMoves[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };
    for (int square = 0; square < 64; ++square) {
        const int line = square / 8;
        const int column = square % 8;
        Bitboard attacks = 0;
        for (const auto& move: kingMoves) {
            int newLine = line + move[0];
            int newColumn = column + move[1];
            if (newLine >= 0 && newLine < 8 && newColumn >= 0 && newColumn < 8) {
                attacks |= (1ULL << (newLine * 8 + newColumn));
            }
        }
        res[square] = attacks;
    }
    return res;
}

constexpr std::array<Bitboard, 64> init_knight_attacks() {
    std::array<Bitboard, 64> res{};
    const int knightMoves[8][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };
    for (int square = 0; square < 64; ++square) {
        Bitboard attacks = 0;
        const int line = square / 8;
        const int column = square % 8;

        for (const auto& move : knightMoves) {
            int newLine = line + move[0];
            int newColumn = column + move[1];
            if (newLine >= 0 && newLine < 8 && newColumn >= 0 && newColumn < 8) {
                attacks |= (1ULL << (newLine * 8 + newColumn));
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