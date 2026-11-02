#include "types.h"
#include <cassert>

constexpr Bitboard sq_bb(int sq) { return Bitboard(1) << sq; }

constexpr int piece_color(int pc) { return pc >> 3; }
constexpr int piece_type(int pc) { return pc & 7; }

static inline void put_piece(Position* p, int pc, int sq) {
	assert(p->board[sq] == EMPTY);
	p->pieces[piece_type(pc)] |= sq_bb(sq);
	p->colors[piece_color(pc)] |= sq_bb(sq);
	p->board[sq] = pc;
}

static inline void remove_piece(Position* p, int pc, int sq) {
	assert(p->board[sq] == pc);
	p->pieces[piece_type(pc)] ^= sq_bb(sq);
	p->colors[piece_color(pc)] ^= sq_bb(sq);
	p->board[sq] = EMPTY;
}

static inline void move_piece(Position* p, int pc, int from, int to) {
	assert(p->board[from] == pc && p->board[to] == EMPTY);
	Bitboard fromto = sq_bb(from) | sq_bb(to);
	p->pieces[piece_type(pc)] ^= fromto;
	p->colors[piece_color(pc)] ^= fromto;
	p->board[from] = EMPTY;
	p->board[to] = pc;
}
