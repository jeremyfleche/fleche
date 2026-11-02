#include "attacks.h"
#include "test.h"

void test_attacks() {
	static_assert(pawn_attacks(WHITE, A2) == bb(B3));
	static_assert(pawn_attacks(BLACK, H7) == bb(G6));
	static_assert(pawn_attacks(WHITE, F3) == bb(E4, G4));
	static_assert(pawn_attacks(BLACK, E6) == bb(D5, F5));

	static_assert(knight_attacks(A1) == bb(B3, C2));
	static_assert(knight_attacks(H8) == (bb(F7, G6)));
	static_assert(knight_attacks(D4) == (bb(C2, E2, F3, F5, E6, C6, B5, B3)));

	static_assert(king_attacks(E1) == (bb(D1, D2, E2, F2, F1)));
	static_assert(king_attacks(E8) == (bb(D8, D7, E7, F7, F8)));
	static_assert(king_attacks(A1) == (bb(A2, B2, B1)));
	static_assert(king_attacks(H8) == (bb(G8, G7, H7)));
	static_assert(king_attacks(D4) == (bb(C3, C4, C5, D3, D5, E3, E4, E5)));
}