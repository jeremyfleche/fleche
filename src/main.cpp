#include "attacks.h"
#include "debug.h"
#include <cstdio>

int main() {
	init_attacks();
	display_bitboard(queen_attacks(D4, bb(C3, D3, E3, E4, E5, D5, C5, C4)),
	                 "Queen attacks from D4 with blockers on D5 and E4");
	return 0;
}
