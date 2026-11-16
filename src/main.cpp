#include "attacks.h"
#include "position.h"

int main() {
	init_attacks();
	Position pos;
	std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	pos.set_fen(fen);
	pos.display();
	return 0;
}
