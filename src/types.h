#pragma once

#include <cstdint>

using Bitboard = uint64_t;

enum Color : uint8_t { WHITE, BLACK };
enum PieceType : uint8_t { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum { WHITE_OO = 1, WHITE_OOO = 2, BLACK_OO = 4, BLACK_OOO = 8 };

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
	
};
