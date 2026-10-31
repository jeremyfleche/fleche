#include "types.h"
#include <assert.h>

#define SQ_BB(sq) (1ULL << (sq))

static inline int piece_color(int pc) { return pc >> 3; }
static inline int piece_type(int pc) { return pc & 7; }

static inline void put_piece(Position* p, int pc, int sq) {
	assert(p->board[sq] == EMPTY);
	p->pieces[piece_type(pc)] |= SQ_BB(sq);;
	p->colors[piece_color(pc)] |= SQ_BB(sq);;
	p->board[sq] |= pc;
}

static inline void remove_piece(Position* p, int pc, int sq) {
	assert(p->board[sq] == pc);
	p->pieces[piece_type(pc)] ^= SQ_BB(sq);;
	p->colors[piece_color(pc)] ^= SQ_BB(sq);;
	p->board[sq] = EMPTY;
}

static inline void move_piece(Position* p, int pc, int from, int to) {
	assert(p->pieces[from] == pc && p->board[to] == EMPTY);
	Bitboard fromto = SQ_BB(from) | SQ_BB(to);
	p->pieces[piece_type(pc)] ^= fromto;
	p->colors[piece_color(pc)] ^= fromto;
	p->board[from] = EMPTY;
	p->board[to] = pc;
}
