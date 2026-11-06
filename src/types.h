#pragma once

#include <cstdint>

using Bitboard = uint64_t;

enum Color : uint8_t { WHITE, BLACK };
enum PieceType : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum { WHITE_OO = 1, WHITE_OOO = 2, BLACK_OO = 4, BLACK_OOO = 8 };
// clang-format off
enum Square : uint8_t {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	NO_SQUARE
};
// clang-format on

constexpr Bitboard sq_bb(int square) { return Bitboard(1) << square; }

static_assert(sq_bb(A1) == 1);
static_assert(sq_bb(A2) == (1ULL << 8));
static_assert(sq_bb(H8) == (1ULL << 63));

template <typename... Sq> constexpr Bitboard bb(Sq... squares) { return (sq_bb(squares) | ...); }

static_assert(bb(A1, H8) == (1ULL | (1ULL << 63)));
static_assert(bb(B2, C4) == ((1ULL << 9) | (1ULL << 26)));

static_assert(A1 == 0);
static_assert(A2 == 8);
static_assert(H1 == 7);
static_assert(H8 == 63);
static_assert(D4 == 27);

constexpr uint8_t EMPTY = 0xFF;
constexpr uint8_t make_piece(Color color, PieceType type) { return (color << 3) | type; }

static_assert(make_piece(BLACK, KING) == 13);
static_assert(make_piece(WHITE, BISHOP) == 2);

constexpr int piece_color(int piece) { return piece >> 3; }

static_assert(piece_color(make_piece(WHITE, PAWN)) == WHITE);
static_assert(piece_color(make_piece(BLACK, KNIGHT)) == BLACK);

constexpr int piece_type(int piece) { return piece & 7; }

static_assert(piece_type(make_piece(BLACK, ROOK)) == ROOK);
static_assert(piece_type(make_piece(WHITE, PAWN)) == PAWN);