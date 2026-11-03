#pragma once

#include "types.h"
#include <cassert>
#include <cstring>

class Position {
  public:
	Position() { std::memset(board_, EMPTY, sizeof(board_)); }

	Bitboard pieces(int type) const { return pieces_[type]; }
	Bitboard pieces(int color, int type) const { return pieces_[type] & colors_[color]; }
	Bitboard colors(int color) const { return colors_[color]; }
	int piece_on(Square sq) const { return board_[sq]; }
	Color side_to_move() const { return side_to_move_; }
	Square ep_square() const { return ep_square_; }
	uint64_t hash() const { return hash_; }

	void put_piece(int piece, int square) {
		assert(board_[square] == EMPTY);
		pieces_[piece_type(piece)] |= sq_bb(square);
		colors_[piece_color(piece)] |= sq_bb(square);
		board_[square] = piece;
	}

	void put_piece(Color color, PieceType type, int square) { put_piece(make_piece(color, type), square); }

	void remove_piece(int piece, int square) {
		assert(board_[square] == piece);
		pieces_[piece_type(piece)] ^= sq_bb(square);
		colors_[piece_color(piece)] ^= sq_bb(square);
		board_[square] = EMPTY;
	}

	void remove_piece(Color color, PieceType type, int square) { remove_piece(make_piece(color, type), square); }

	void move_piece(int piece, int from, int to) {
		assert(board_[from] == piece && board_[to] == EMPTY);
		Bitboard fromto = sq_bb(from) | sq_bb(to);
		pieces_[piece_type(piece)] ^= fromto;
		colors_[piece_color(piece)] ^= fromto;
		board_[from] = EMPTY;
		board_[to] = piece;
	}

	void move_piece(Color color, PieceType type, int from, int to) { move_piece(make_piece(color, type), from, to); }

  private:
	Bitboard pieces_[6] = {};
	Bitboard colors_[2] = {};
	uint8_t board_[64]; // 0xFF = empty, otherwise piece code (color << 3 | type)

	Color side_to_move_ = WHITE;
	uint8_t castling_ = 0;         // WHITE_OO, WHITE_OOO, BLACK_OO, BLACK_OOO
	Square ep_square_ = NO_SQUARE; // en passant square
	uint8_t halfmove_ = 0;         // 50 moves rule
	uint16_t fullmove_ = 1;        // move number
	uint64_t hash_ = 0;            // zobrist hash
};