#include "attacks.h"
#include "debug.h"
#include "test.h"

void test_attacks_rook() {
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

void test_attacks_bishop() {
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

void test_attacks() {
	init_attacks();
	test_attacks_rook();
	test_attacks_bishop();
}