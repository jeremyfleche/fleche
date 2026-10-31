#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint64_t Bitboard;

enum { WHITE, BLACK };
enum { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum { WHITE_OO = 1, WHITE_OOO = 2, BLACK_OO = 4, BLACK_OOO = 8 };

#define EMPTY 0xFF
#define make_piece(color, type) ((color << 3) | (type))

typedef struct {
	Bitboard pieces[6];
	Bitboard colors[2];
	uint8_t board[64];		// 0xFF = empty, otherwise piece code (color << 3 | type)

	uint8_t side_to_move;
	uint8_t castling;		// WHITE_OO, WHITE_OOO, BLACK_OO, BLACK_OOO
	uint8_t ep_square;		// en passant squares
	uint8_t halfmove;		// 50 moves rule
	uint8_t fullmove;		// move number
} Position;

#endif
