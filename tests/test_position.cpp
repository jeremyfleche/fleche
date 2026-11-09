#include "position.h"
#include "test.h"

void test_constructors() {
	Position p{};
	for (int type = PAWN; type <= KING; ++type)
		CHECK(p.pieces(type) == 0);
	CHECK(p.side_to_move() == WHITE);
	CHECK(p.ep_square() == NO_SQUARE);
	CHECK(p.colors(WHITE) == 0);
	CHECK(p.colors(BLACK) == 0);
	for (int sq = A1; sq <= H8; ++sq)
		CHECK(p.piece_on(static_cast<Square>(sq)) == EMPTY);

	Position p1{};
	CHECK(p1.pieces(PAWN) == 0);
	CHECK(p1.colors(WHITE) == 0);
	CHECK(p1.colors(BLACK) == 0);

	Position p2{p1};
	CHECK(p2.pieces(PAWN) == 0);
	CHECK(p2.colors(WHITE) == 0);
	CHECK(p2.colors(BLACK) == 0);

	Position p3{};
	p3.put_piece(WHITE, PAWN, E2);
	p3.put_piece(BLACK, KNIGHT, G8);
	Position p4{p3};
	CHECK(p4.piece_on(E2) == make_piece(WHITE, PAWN));
	CHECK(p4.piece_on(G8) == make_piece(BLACK, KNIGHT));
}

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
	CHECK(p.pieces(BISHOP) == sq_bb(B5));
	CHECK(p.colors(WHITE) == sq_bb(B5));
	CHECK(p.pieces(WHITE, BISHOP) == sq_bb(B5));
	p.move_piece(WHITE, BISHOP, B5, D7);
	CHECK(p.piece_on(D7) == make_piece(WHITE, BISHOP));
	CHECK(p.pieces(BISHOP) == sq_bb(D7));
	CHECK(p.colors(WHITE) == sq_bb(D7));
	CHECK(p.pieces(WHITE, BISHOP) == sq_bb(D7));
	p.put_piece(BLACK, ROOK, D8);
	p.move_piece(BLACK, ROOK, D8, D1);
	CHECK(p.piece_on(D1) == make_piece(BLACK, ROOK));
	CHECK(p.pieces(ROOK) == sq_bb(D1));
	CHECK(p.colors(BLACK) == sq_bb(D1));
	CHECK(p.pieces(BLACK, ROOK) == sq_bb(D1));
	CHECK(p.piece_on(D8) == EMPTY);
	CHECK(p.piece_on(D7) == make_piece(WHITE, BISHOP));
}

void test_position() {
	test_constructors();
	test_put();
	test_remove();
	test_move();
}