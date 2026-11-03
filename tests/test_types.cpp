#include "test.h"
#include "types.h"

void test_types() {
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
}