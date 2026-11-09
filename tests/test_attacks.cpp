#include "attacks.h"
#include "debug.h"
#include "test.h"
#include <random>

static std::mt19937_64 rng(1234); // fixed seed: failures are reproducible

// Random bitboard: about 32, 16 or 8 bits set depending on density (0, 1 or 2).
static inline Bitboard random_bits(int density) {
	Bitboard bits = rng();
	for (int i = 0; i < density; ++i)
		bits &= rng();
	return bits;
}

static constexpr int total_bits(const std::array<Bitboard, 64>& masks) {
	int total = 0;
	for (Bitboard mask : masks)
		total += __builtin_popcountll(mask);
	return total;
}

static constexpr bool is_symmetric(const std::array<Bitboard, 64>& table1, const std::array<Bitboard, 64>& table2) {
	for (int a = 0; a < 64; ++a)
		for (int b = 0; b < 64; ++b) {
			const bool a_attacks_b = table1[a] & sq_bb(b);
			const bool b_attacks_a = table2[b] & sq_bb(a);
			if (a_attacks_b != b_attacks_a)
				return false;
		}
	return true;
}

static_assert(total_bits(ROOK_MASK) == 672);
static_assert(total_bits(BISHOP_MASK) == 364);
static_assert(total_bits(PAWN_ATTACKS[WHITE]) == 98);
static_assert(total_bits(PAWN_ATTACKS[BLACK]) == 98);
static_assert(total_bits(KING_ATTACKS) == 420);
static_assert(total_bits(KNIGHT_ATTACKS) == 336);

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

static_assert(is_symmetric(KNIGHT_ATTACKS, KNIGHT_ATTACKS));
static_assert(is_symmetric(KING_ATTACKS, KING_ATTACKS));
static_assert(is_symmetric(PAWN_ATTACKS[WHITE], PAWN_ATTACKS[BLACK]));

static void test_attacks_rook() {
	for (int sq = 0; sq < 64; ++sq) {
		const Magic& m = ROOK_MAGICS[sq];
		Bitboard occ = 0;
		int i = 0;
		do {
			Bitboard expected = slow_rook_attacks(sq, occ);
			Bitboard actual = rook_attacks(sq, occ);
			CHECK(expected == actual);
			Bitboard noisy_occ = occ | (random_bits(i % 3) & ~m.mask);
			CHECK(slow_rook_attacks(sq, noisy_occ) == rook_attacks(sq, noisy_occ));
			occ = (occ - m.mask) & m.mask; // generate next subset of mask
			++i;
		} while (occ != 0);
	}
}

static void test_attacks_bishop() {
	for (int sq = 0; sq < 64; ++sq) {
		const Magic& m = BISHOP_MAGICS[sq];
		Bitboard occ = 0;
		int i = 0;
		do {
			Bitboard expected = slow_bishop_attacks(sq, occ);
			Bitboard actual = bishop_attacks(sq, occ);
			CHECK(expected == actual);
			Bitboard noisy_occ = occ | (random_bits(i % 3) & ~m.mask);
			CHECK(slow_bishop_attacks(sq, noisy_occ) == bishop_attacks(sq, noisy_occ));
			occ = (occ - m.mask) & m.mask; // generate next subset of mask
			++i;
		} while (occ != 0);
	}
}

static void test_attacks_queen() {
	CHECK(__builtin_popcountll(queen_attacks(D4, 0)) == 27);
	CHECK(__builtin_popcountll(queen_attacks(A1, 0)) == 21);
	CHECK(queen_attacks(D4, bb(C3, D3, E3, E4, E5, D5, C5, C4)) == king_attacks(D4));
	CHECK(queen_attacks(D4, bb(D5, E4)) ==
	      bb(A1, D1, G1, B2, D2, F2, C3, D3, E3, A4, B4, C4, E4, C5, D5, E5, B6, F6, A7, G7, H8));
	for (int sq = 0; sq < 64; ++sq) {
		for (int i = 0; i < 150; ++i) {
			Bitboard occ = random_bits(i % 3);
			Bitboard expected = rook_attacks(sq, occ) | bishop_attacks(sq, occ);
			Bitboard actual = queen_attacks(sq, occ);
			CHECK(expected == actual);
		}
	}
}

void test_attacks() {
	init_attacks();
	test_attacks_rook();
	test_attacks_bishop();
	test_attacks_queen();
}