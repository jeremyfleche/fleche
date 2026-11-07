#include "attacks.h"

std::array<Bitboard, 102400> ROOK_TABLE;
std::array<Bitboard, 5248> BISHOP_TABLE;

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