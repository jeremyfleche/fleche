#pragma once

#include "types.h"

enum MoveFlag : uint8_t {
	QUIET = 0,
	DOUBLE_PUSH = 1,
	KING_CASTLE = 2,
	QUEEN_CASTLE = 3,
	CAPTURE = 4,
	EN_PASSANT = 5,
	KNIGHT_PROMO = 8,
	BISHOP_PROMO = 9,
	ROOK_PROMO = 10,
	QUEEN_PROMO = 11,
	KNIGHT_PROMO_CAPTURE = 12,
	BISHOP_PROMO_CAPTURE = 13,
	ROOK_PROMO_CAPTURE = 14,
	QUEEN_PROMO_CAPTURE = 15,
};

inline constexpr const char* FLAG_NAMES[16] = {
    "quiet",
    "double push",
    "king castle",
    "queen castle",
    "capture",
    "en passant",
    "?",
    "?",
    "knight promo",
    "bishop promo",
    "rook promo",
    "queen promo",
    "knight promo x",
    "bishop promo x",
    "rook promo x",
    "queen promo x",
};

class Move {
  public:
	constexpr Move() = default;
	constexpr Move(Square from, Square to, MoveFlag flag = QUIET)
	    : data_((from & 0x3F) | ((to & 0x3F) << 6) | ((flag & 0x0F) << 12)) {}

	constexpr Square from() const { return static_cast<Square>(data_ & 0x3F); }
	constexpr Square to() const { return static_cast<Square>((data_ >> 6) & 0x3F); }
	constexpr MoveFlag flags() const { return static_cast<MoveFlag>((data_ >> 12) & 0xF); }
	constexpr bool is_capture() const { return (flags() & 4) != 0; }
	constexpr bool is_promotion() const { return (flags() & 8) != 0; }
	constexpr PieceType promotion_type() const { return static_cast<PieceType>(KNIGHT + (flags() & 3)); }

	constexpr bool operator==(const Move&) const = default;

  private:
	// (0-5) from, (6-11) to, (12-15) flags
	uint16_t data_ = 0;
};

struct MoveList {
	Move moves[256];
	int size = 0;

	void add(Move move) { moves[size++] = move; }

	Move* begin() { return moves; }
	Move* end() { return moves + size; }

	const Move* begin() const { return moves; }
	const Move* end() const { return moves + size; }
};