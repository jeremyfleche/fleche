#pragma once

#include "types.h"
#include <cassert>

inline void put_piece(Position* p, int piece, int square) {
	assert(p->board[square] == EMPTY);
	p->pieces[piece_type(piece)] |= sq_bb(square);
	p->colors[piece_color(piece)] |= sq_bb(square);
	p->board[square] = piece;
}

inline void remove_piece(Position* p, int piece, int square) {
	assert(p->board[square] == piece);
	p->pieces[piece_type(piece)] ^= sq_bb(square);
	p->colors[piece_color(piece)] ^= sq_bb(square);
	p->board[square] = EMPTY;
}

inline void move_piece(Position* p, int piece, int from, int to) {
	assert(p->board[from] == piece && p->board[to] == EMPTY);
	Bitboard fromto = sq_bb(from) | sq_bb(to);
	p->pieces[piece_type(piece)] ^= fromto;
	p->colors[piece_color(piece)] ^= fromto;
	p->board[from] = EMPTY;
	p->board[to] = piece;
}


