#include "attacks.h"

std::array<Bitboard, 102400> ROOK_TABLE;
std::array<Bitboard, 5248> BISHOP_TABLE;

constexpr int total_bits(const std::array<Bitboard, 64>& masks) {
	int total = 0;
	for (Bitboard mask : masks)
		total += std::popcount(mask);
	return total;
}

static_assert(total_bits(ROOK_MASK) == 672);
static_assert(total_bits(BISHOP_MASK) == 364);

static_assert(ROOK_MASK[A1] == bb(A2, A3, A4, A5, A6, A7, B1, C1, D1, E1, F1, G1));
static_assert(ROOK_MASK[H8] == bb(H2, H3, H4, H5, H6, H7, B8, C8, D8, E8, F8, G8));
static_assert(ROOK_MASK[D4] == bb(D2, D3, B4, C4, E4, F4, G4, D5, D6, D7));

static_assert(BISHOP_MASK[A1] == bb(B2, C3, D4, E5, F6, G7));
static_assert(BISHOP_MASK[D7] == bb(G4, B5, F5, C6, E6));
static_assert(BISHOP_MASK[F3] == bb(E2, G2, E4, G4, D5, C6, B7));

static_assert(slow_rook_attacks(H1, 0) == bb(A1, B1, C1, D1, E1, F1, G1, H2, H3, H4, H5, H6, H7, H8));
static_assert(slow_rook_attacks(H1, bb(H4)) == bb(A1, B1, C1, D1, E1, F1, G1, H2, H3, H4));
static_assert(slow_rook_attacks(H1, bb(H4, B1)) == bb(B1, C1, D1, E1, F1, G1, H2, H3, H4));
static_assert(slow_rook_attacks(H1, bb(H4, H5, B1)) == slow_rook_attacks(H1, bb(H4, B1)));

static_assert(slow_bishop_attacks(D4, 0) == bb(A1, G1, B2, F2, C3, E3, C5, E5, B6, F6, A7, G7, H8));
static_assert(slow_bishop_attacks(D4, bb(F6)) == bb(A1, G1, B2, F2, C3, E3, C5, E5, B6, F6, A7));
static_assert(slow_bishop_attacks(D4, bb(F6, C3)) == bb(G1, F2, C3, E3, C5, E5, B6, F6, A7));
static_assert(slow_bishop_attacks(D4, bb(F6, C3, G7)) == slow_bishop_attacks(D4, bb(F6, C3)));

static_assert(pawn_attacks(WHITE, A2) == bb(B3));
static_assert(pawn_attacks(BLACK, H7) == bb(G6));
static_assert(pawn_attacks(WHITE, F3) == bb(E4, G4));
static_assert(pawn_attacks(BLACK, E6) == bb(D5, F5));

static_assert(king_attacks(E1) == (bb(D1, D2, E2, F2, F1)));
static_assert(king_attacks(E8) == (bb(D8, D7, E7, F7, F8)));
static_assert(king_attacks(A1) == (bb(A2, B2, B1)));
static_assert(king_attacks(H8) == (bb(G8, G7, H7)));
static_assert(king_attacks(D4) == (bb(C3, C4, C5, D3, D5, E3, E4, E5)));

static_assert(knight_attacks(A1) == bb(B3, C2));
static_assert(knight_attacks(H8) == (bb(F7, G6)));
static_assert(knight_attacks(D4) == (bb(C2, E2, F3, F5, E6, C6, B5, B3)));

static_assert(ROOK_MAGICS[H8].offset + (1 << (64 - ROOK_MAGICS[H8].shift)) == 102400, "Rook table size mismatch");
static_assert(BISHOP_MAGICS[H8].offset + (1 << (64 - BISHOP_MAGICS[H8].shift)) == 5248, "Bishop table size mismatch");

void init_rook_attacks() {
	for (int sq = 0; sq < 64; ++sq) {
		const Magic& m = ROOK_MAGICS[sq];
		Bitboard occ = 0;
		do {
			ROOK_TABLE[m.offset + ((occ * m.magic) >> m.shift)] = slow_rook_attacks(sq, occ);
			occ = (occ - m.mask) & m.mask; // generate next subset of mask
		} while (occ != 0);
	}
}

void init_bishop_attacks() {
	for (int sq = 0; sq < 64; ++sq) {
		const Magic& m = BISHOP_MAGICS[sq];
		Bitboard occ = 0;
		do {
			BISHOP_TABLE[m.offset + ((occ * m.magic) >> m.shift)] = slow_bishop_attacks(sq, occ);
			occ = (occ - m.mask) & m.mask; // generate next subset of mask
		} while (occ != 0);
	}
}

void init_attacks() {
	init_rook_attacks();
	init_bishop_attacks();
}