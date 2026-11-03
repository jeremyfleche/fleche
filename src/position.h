#include "types.h"
#include <cassert>
#include <cstring>

class Position {
  public:
	Position()
	    : pieces_{}, colors_{}, side_to_move_(WHITE), castling_(0), ep_square_(NO_SQUARE), halfmove_(0), fullmove_(1),
	      hash_(0) {
		std::memset(board_, EMPTY, sizeof(board_));
	}

	Bitboard pieces(int type) const { return pieces_[type]; }
	Bitboard pieces(int color, int type) const { return pieces_[type] & colors_[color]; }
	Bitboard colors(int color) const { return colors_[color]; }
	int piece_on(int sq) const { return board_[sq]; }
	int side_to_move() const { return side_to_move_; }
	int ep_square() const { return ep_square_; }
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

	void move_piece(Color color, PieceType type, int from, int to) { move_piece((color << 3) | type, from, to); }

  private:
	Bitboard pieces_[6];
	Bitboard colors_[2];
	uint8_t board_[64]; // 0xFF = empty, otherwise piece code (color << 3 | type)

	uint8_t side_to_move_;
	uint8_t castling_;  // WHITE_OO, WHITE_OOO, BLACK_OO, BLACK_OOO
	uint8_t ep_square_; // en passant squares
	uint8_t halfmove_;  // 50 moves rule
	uint16_t fullmove_; // move number
	uint64_t hash_;
};