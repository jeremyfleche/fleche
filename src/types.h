#pragma once

#include <cstdint>

using Bitboard = uint64_t;
constexpr Bitboard sq_bb(int square) { return Bitboard(1) << square; }

template <typename... Sq>
constexpr Bitboard bb(Sq... squares) { return (sq_bb(squares) | ...); }

enum Color : uint8_t { WHITE, BLACK };
enum PieceType : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum { WHITE_OO = 1, WHITE_OOO = 2, BLACK_OO = 4, BLACK_OOO = 8 };
enum Square : uint8_t {
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8,
	NO_SQUARE
};

constexpr uint8_t EMPTY = 0xFF;
constexpr uint8_t make_piece(int color, int type) { return (color << 3) | type; }

struct Position {
	Bitboard pieces[6];
	Bitboard colors[2];
	uint8_t board[64];		// 0xFF = empty, otherwise piece code (color << 3 | type)

	uint8_t side_to_move;
	uint8_t castling;		// WHITE_OO, WHITE_OOO, BLACK_OO, BLACK_OOO
	uint8_t ep_square;		// en passant squares
	uint8_t halfmove;		// 50 moves rule
	uint16_t fullmove;		// move number
	uint64_t hash;
};
