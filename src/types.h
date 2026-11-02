#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>

using Bitboard = uint64_t;
constexpr Bitboard sq_bb(int square) { return Bitboard(1) << square; }

template <typename... Sq> constexpr Bitboard bb(Sq... squares) { return (sq_bb(squares) | ...); }

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

constexpr uint8_t EMPTY = 0xFF;
constexpr uint8_t make_piece(int color, int type) { return (color << 3) | type; }
constexpr int piece_color(int piece) { return piece >> 3; }
constexpr int piece_type(int piece) { return piece & 7; }

static_assert(A1 == 0);
static_assert(A2 == 8);
static_assert(H1 == 7);
static_assert(H8 == 63);
static_assert(D4 == 27);

static_assert(sq_bb(A1) == 1);
static_assert(sq_bb(A2) == (1ULL << 8));
static_assert(sq_bb(H8) == (1ULL << 63));

static_assert(bb(A1, H8) == (1ULL | (1ULL << 63)));
static_assert(bb(B2, C4) == ((1ULL << 9) | (1ULL << 26)));

static_assert(make_piece(BLACK, KING) == 13);
static_assert(make_piece(WHITE, BISHOP) == 2);

static_assert(piece_color(make_piece(WHITE, PAWN)) == WHITE);
static_assert(piece_type(make_piece(BLACK, ROOK)) == ROOK);

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