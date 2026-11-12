#include "attacks.h"
#include "debug.h"
#include "position.h"
#include <cstdio>

int main() {
	init_attacks();
	Position pos;
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	pos.set_fen(fen);
	std::string generated_fen = pos.fen();
	pos.display();
	if (pos.is_consistent())
		std::printf("Position is consistent\n");
	else
		std::printf("Position is inconsistent\n");
	return 0;
}
