#include "move.h"

// Size: a MoveList of 256 moves must stay small.
static_assert(sizeof(Move) == 2);

// Default move: everything is zero.
static_assert(Move().from() == A1);
static_assert(Move().to() == A1);
static_assert(Move().flags() == QUIET);
static_assert(Move() == Move(A1, A1, QUIET));

// Simple encoding / decoding.
static_assert(Move(E2, E4, DOUBLE_PUSH).from() == E2);
static_assert(Move(E2, E4, DOUBLE_PUSH).to() == E4);
static_assert(Move(E2, E4, DOUBLE_PUSH).flags() == DOUBLE_PUSH);
static_assert(Move(G1, F3).flags() == QUIET); // default flag

// Extreme values: every bit of every field set.
static_assert(Move(H8, H8, QUEEN_PROMO_CAPTURE).from() == H8);
static_assert(Move(H8, H8, QUEEN_PROMO_CAPTURE).to() == H8);
static_assert(Move(H8, H8, QUEEN_PROMO_CAPTURE).flags() == QUEEN_PROMO_CAPTURE);

// Fields do not overlap.
static_assert(Move(H8, A1).from() == H8 && Move(H8, A1).to() == A1);
static_assert(Move(A1, H8).from() == A1 && Move(A1, H8).to() == H8);
static_assert(Move(A1, A1, QUEEN_PROMO_CAPTURE).from() == A1 && Move(A1, A1, QUEEN_PROMO_CAPTURE).to() == A1);
static_assert(Move(H8, H8, QUIET).flags() == QUIET);

// Every square and every flag survives a round trip.
static constexpr bool round_trip_all() {
	constexpr MoveFlag flags[] = {QUIET,
	                              DOUBLE_PUSH,
	                              KING_CASTLE,
	                              QUEEN_CASTLE,
	                              CAPTURE,
	                              EN_PASSANT,
	                              KNIGHT_PROMO,
	                              BISHOP_PROMO,
	                              ROOK_PROMO,
	                              QUEEN_PROMO,
	                              KNIGHT_PROMO_CAPTURE,
	                              BISHOP_PROMO_CAPTURE,
	                              ROOK_PROMO_CAPTURE,
	                              QUEEN_PROMO_CAPTURE};
	for (int from = A1; from <= H8; ++from)
		for (int to = A1; to <= H8; ++to)
			for (MoveFlag flag : flags) {
				const Move m(Square(from), Square(to), flag);
				if (m.from() != from || m.to() != to || m.flags() != flag)
					return false;
			}
	return true;
}
static_assert(round_trip_all());

// Captures and promotions.
static_assert(!Move(E2, E3, QUIET).is_capture() && !Move(E2, E3, QUIET).is_promotion());
static_assert(!Move(E2, E4, DOUBLE_PUSH).is_capture() && !Move(E2, E4, DOUBLE_PUSH).is_promotion());
static_assert(!Move(E1, G1, KING_CASTLE).is_capture() && !Move(E1, G1, KING_CASTLE).is_promotion());
static_assert(!Move(E1, C1, QUEEN_CASTLE).is_capture() && !Move(E1, C1, QUEEN_CASTLE).is_promotion());

static_assert(Move(E4, D5, CAPTURE).is_capture() && !Move(E4, D5, CAPTURE).is_promotion());
static_assert(Move(E5, D6, EN_PASSANT).is_capture() && !Move(E5, D6, EN_PASSANT).is_promotion());

static_assert(!Move(E7, E8, QUEEN_PROMO).is_capture() && Move(E7, E8, QUEEN_PROMO).is_promotion());
static_assert(!Move(E7, E8, KNIGHT_PROMO).is_capture() && Move(E7, E8, KNIGHT_PROMO).is_promotion());
static_assert(Move(E7, D8, KNIGHT_PROMO_CAPTURE).is_capture() && Move(E7, D8, KNIGHT_PROMO_CAPTURE).is_promotion());
static_assert(Move(E7, D8, QUEEN_PROMO_CAPTURE).is_capture() && Move(E7, D8, QUEEN_PROMO_CAPTURE).is_promotion());

// Promotion piece.
static_assert(Move(A7, A8, KNIGHT_PROMO).promotion_type() == KNIGHT);
static_assert(Move(A7, A8, BISHOP_PROMO).promotion_type() == BISHOP);
static_assert(Move(A7, A8, ROOK_PROMO).promotion_type() == ROOK);
static_assert(Move(A7, A8, QUEEN_PROMO).promotion_type() == QUEEN);
static_assert(Move(A7, B8, KNIGHT_PROMO_CAPTURE).promotion_type() == KNIGHT);
static_assert(Move(A7, B8, BISHOP_PROMO_CAPTURE).promotion_type() == BISHOP);
static_assert(Move(A7, B8, ROOK_PROMO_CAPTURE).promotion_type() == ROOK);
static_assert(Move(A7, B8, QUEEN_PROMO_CAPTURE).promotion_type() == QUEEN);

// Equality.
static_assert(Move(E2, E4, DOUBLE_PUSH) == Move(E2, E4, DOUBLE_PUSH));
static_assert(!(Move(E2, E4, DOUBLE_PUSH) == Move(E2, E4, QUIET))); // same squares, different flag
static_assert(!(Move(E2, E4) == Move(E4, E2)));                     // squares swapped