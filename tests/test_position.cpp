#include "position.h"
#include "test.h"

static void test_put() {
	Position p{};
	p.put_piece(WHITE, PAWN, E2);
	CHECK(p.piece_on(E2) == make_piece(WHITE, PAWN));
	CHECK(p.pieces(PAWN) == sq_bb(E2));
	CHECK(p.colors(WHITE) == sq_bb(E2));
	p.put_piece(WHITE, KING, E8);
	CHECK(p.piece_on(E8) == make_piece(WHITE, KING));
	CHECK(p.pieces(KING) == sq_bb(E8));
	CHECK(p.colors(WHITE) == (sq_bb(E2) | sq_bb(E8)));
}

static void test_remove() {
	Position p{};
	p.put_piece(BLACK, KNIGHT, G8);
	p.put_piece(WHITE, QUEEN, D2);
	p.remove_piece(BLACK, KNIGHT, G8);
	CHECK(p.piece_on(G8) == EMPTY);
	CHECK(p.pieces(KNIGHT) == 0);
	CHECK(p.colors(BLACK) == 0);
	CHECK(p.colors(WHITE) == sq_bb(D2));
	CHECK(p.pieces(QUEEN) == sq_bb(D2));
}

static void test_move() {
	Position p{};
	p.put_piece(WHITE, BISHOP, F1);
	p.move_piece(WHITE, BISHOP, F1, B5);
	CHECK(p.piece_on(F1) == EMPTY);
	CHECK(p.piece_on(B5) == make_piece(WHITE, BISHOP));
	p.move_piece(WHITE, BISHOP, B5, D7);
	CHECK(p.piece_on(D7) == make_piece(WHITE, BISHOP));
}

void test_position() {
	test_put();
	test_remove();
	test_move();
}