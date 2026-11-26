#include "attacks.h"
#include "movegen.h"
#include "position.h"
#include "test.h"
#include <initializer_list>
#include <string>

// "e2e4", "e7e8q": readable form of a move for failure messages.
static std::string move_name(Move m) {
	std::string s;
	s += char('a' + m.from() % 8);
	s += char('1' + m.from() / 8);
	s += char('a' + m.to() % 8);
	s += char('1' + m.to() / 8);
	if (m.is_promotion())
		s += "nbrq"[m.promotion_type() - KNIGHT];
	return s + " (flag " + std::to_string(m.flags()) + ")";
}

static bool contains(const MoveList& list, Move m) {
	for (Move x : list)
		if (x == m)
			return true;
	return false;
}

// Number of generated moves made by a pawn (the moves of the kings are ignored).
static int pawn_count(const Position& pos, const MoveList& list) {
	int count = 0;
	for (Move m : list)
		if (piece_type(pos.piece_on(m.from())) == PAWN)
			++count;
	return count;
}

// The pawn moves of the position must be exactly the expected ones:
// same number of moves, and every expected move present (with the right flag).
static void check_pawn_moves(const char* fen, std::initializer_list<Move> expected) {
	Position pos;
	std::string error;
	CHECK(pos.set_fen(fen, &error), std::string(fen) + " -> " + error);

	MoveList list;
	generate_moves(pos, list);

	CHECK(pawn_count(pos, list) == static_cast<int>(expected.size()),
	      std::string(fen) + " -> " + std::to_string(pawn_count(pos, list)) + " pawn moves, expected " +
	          std::to_string(expected.size()));
	for (Move m : expected)
		CHECK(contains(list, m), std::string(fen) + " -> missing " + move_name(m));
}

// A given move must NOT be generated.
static void check_no_move(const char* fen, Move m) {
	Position pos;
	CHECK(pos.set_fen(fen), fen);
	MoveList list;
	generate_moves(pos, list);
	CHECK(!contains(list, m), std::string(fen) + " -> unexpected " + move_name(m));
}

static void test_pawn_pushes() {
	// Start position: every pawn moves one or two squares.
	check_pawn_moves("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                 {Move(A2, A3), Move(B2, B3), Move(C2, C3), Move(D2, D3), Move(E2, E3), Move(F2, F3), Move(G2, G3),
	                  Move(H2, H3), Move(A2, A4, DOUBLE_PUSH), Move(B2, B4, DOUBLE_PUSH), Move(C2, C4, DOUBLE_PUSH),
	                  Move(D2, D4, DOUBLE_PUSH), Move(E2, E4, DOUBLE_PUSH), Move(F2, F4, DOUBLE_PUSH),
	                  Move(G2, G4, DOUBLE_PUSH), Move(H2, H4, DOUBLE_PUSH)});
	check_pawn_moves("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
	                 {Move(A7, A6), Move(B7, B6), Move(C7, C6), Move(D7, D6), Move(E7, E6), Move(F7, F6), Move(G7, G6),
	                  Move(H7, H6), Move(A7, A5, DOUBLE_PUSH), Move(B7, B5, DOUBLE_PUSH), Move(C7, C5, DOUBLE_PUSH),
	                  Move(D7, D5, DOUBLE_PUSH), Move(E7, E5, DOUBLE_PUSH), Move(F7, F5, DOUBLE_PUSH),
	                  Move(G7, G5, DOUBLE_PUSH), Move(H7, H5, DOUBLE_PUSH)});

	// Blocked by an enemy or an own piece: no move.
	check_pawn_moves("4k3/8/8/8/8/4n3/4P3/4K3 w - - 0 1", {});
	check_pawn_moves("4k3/8/8/8/8/4N3/4P3/4K3 w - - 0 1", {});
	check_pawn_moves("4k3/4p3/4N3/8/8/8/8/4K3 b - - 0 1", {});

	// Double push blocked on the second square: single push only.
	check_pawn_moves("4k3/8/8/8/4n3/8/4P3/4K3 w - - 0 1", {Move(E2, E3)});
	check_pawn_moves("4k3/4p3/8/4N3/8/8/8/4K3 b - - 0 1", {Move(E7, E6)});

	// No double push away from the starting rank.
	check_pawn_moves("4k3/8/8/8/8/4P3/8/4K3 w - - 0 1", {Move(E3, E4)});
	check_pawn_moves("4k3/8/4p3/8/8/8/8/4K3 b - - 0 1", {Move(E6, E5)});
}

static void test_pawn_captures() {
	// Both diagonals, plus the push.
	check_pawn_moves("4k3/8/8/3n1r2/4P3/8/8/4K3 w - - 0 1",
	                 {Move(E4, E5), Move(E4, D5, CAPTURE), Move(E4, F5, CAPTURE)});
	check_pawn_moves("4k3/8/8/4p3/3N1R2/8/8/4K3 b - - 0 1",
	                 {Move(E5, E4), Move(E5, D4, CAPTURE), Move(E5, F4, CAPTURE)});

	// Own pieces on the diagonals are not captured.
	check_pawn_moves("4k3/8/8/3N1R2/4P3/8/8/4K3 w - - 0 1", {Move(E4, E5)});

	// Pieces behind the pawn are not captured.
	check_pawn_moves("4k3/8/8/8/4P3/3n1n2/8/4K3 w - - 0 1", {Move(E4, E5)});

	// No wrap-around between files A and H.
	check_pawn_moves("4k3/8/n7/6n1/7P/8/8/4K3 w - - 0 1", {Move(H4, H5), Move(H4, G5, CAPTURE)});
	check_no_move("4k3/8/n7/6n1/7P/8/8/4K3 w - - 0 1", Move(H4, A6, CAPTURE));
	check_pawn_moves("4k3/8/8/1n5n/P7/8/8/4K3 w - - 0 1", {Move(A4, A5), Move(A4, B5, CAPTURE)});
	check_no_move("4k3/8/8/1n5n/P7/8/8/4K3 w - - 0 1", Move(A4, H4, CAPTURE));
	check_pawn_moves("4k3/8/8/7p/N5N1/8/8/4K3 b - - 0 1", {Move(H5, H4), Move(H5, G4, CAPTURE)});
	check_no_move("4k3/8/8/7p/N5N1/8/8/4K3 b - - 0 1", Move(H5, A3, CAPTURE));

	// Several pawns at once (c4 and e4 are blocked in front).
	check_pawn_moves(
	    "4k3/8/8/1qrbn3/P1P1P3/8/8/4K3 w - - 0 1",
	    {Move(A4, A5), Move(A4, B5, CAPTURE), Move(C4, B5, CAPTURE), Move(C4, D5, CAPTURE), Move(E4, D5, CAPTURE)});
}

static void test_pawn_promotions() {
	// Promotion by push: 4 moves.
	check_pawn_moves("8/4P3/8/8/8/8/8/k3K3 w - - 0 1", {Move(E7, E8, KNIGHT_PROMO), Move(E7, E8, BISHOP_PROMO),
	                                                    Move(E7, E8, ROOK_PROMO), Move(E7, E8, QUEEN_PROMO)});
	check_pawn_moves("k3K3/8/8/8/8/8/4p3/8 b - - 0 1", {Move(E2, E1, KNIGHT_PROMO), Move(E2, E1, BISHOP_PROMO),
	                                                    Move(E2, E1, ROOK_PROMO), Move(E2, E1, QUEEN_PROMO)});

	// Promotion by push and by capture: 8 moves.
	check_pawn_moves("3r4/4P3/8/8/8/8/8/k3K3 w - - 0 1",
	                 {Move(E7, E8, KNIGHT_PROMO), Move(E7, E8, BISHOP_PROMO), Move(E7, E8, ROOK_PROMO),
	                  Move(E7, E8, QUEEN_PROMO), Move(E7, D8, KNIGHT_PROMO_CAPTURE), Move(E7, D8, BISHOP_PROMO_CAPTURE),
	                  Move(E7, D8, ROOK_PROMO_CAPTURE), Move(E7, D8, QUEEN_PROMO_CAPTURE)});

	// Blocked promotion, captures on both sides.
	check_pawn_moves("4nbr1/5P2/8/8/8/8/8/k3K3 w - - 0 1",
	                 {Move(F7, E8, KNIGHT_PROMO_CAPTURE), Move(F7, E8, BISHOP_PROMO_CAPTURE),
	                  Move(F7, E8, ROOK_PROMO_CAPTURE), Move(F7, E8, QUEEN_PROMO_CAPTURE),
	                  Move(F7, G8, KNIGHT_PROMO_CAPTURE), Move(F7, G8, BISHOP_PROMO_CAPTURE),
	                  Move(F7, G8, ROOK_PROMO_CAPTURE), Move(F7, G8, QUEEN_PROMO_CAPTURE)});

	// Promotion on file A, no wrap-around to H8.
	check_pawn_moves("1r5r/P7/8/8/8/8/8/k3K3 w - - 0 1",
	                 {Move(A7, A8, KNIGHT_PROMO), Move(A7, A8, BISHOP_PROMO), Move(A7, A8, ROOK_PROMO),
	                  Move(A7, A8, QUEEN_PROMO), Move(A7, B8, KNIGHT_PROMO_CAPTURE), Move(A7, B8, BISHOP_PROMO_CAPTURE),
	                  Move(A7, B8, ROOK_PROMO_CAPTURE), Move(A7, B8, QUEEN_PROMO_CAPTURE)});

	// A pawn on the 6th rank does not promote.
	check_pawn_moves("4k3/8/4P3/8/8/8/8/4K3 w - - 0 1", {Move(E6, E7)});
}

static void test_pawn_en_passant() {
	check_pawn_moves("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1", {Move(E5, E6), Move(E5, D6, EN_PASSANT)});

	// Two pawns can take on the same square.
	check_pawn_moves("4k3/8/8/2PpP3/8/8/8/4K3 w - d6 0 1",
	                 {Move(C5, C6), Move(C5, D6, EN_PASSANT), Move(E5, E6), Move(E5, D6, EN_PASSANT)});

	check_pawn_moves("4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1", {Move(D4, D3), Move(D4, E3, EN_PASSANT)});

	// Edge files.
	check_pawn_moves("4k3/8/8/6pP/8/8/8/4K3 w - g6 0 1", {Move(H5, H6), Move(H5, G6, EN_PASSANT)});
	check_pawn_moves("4k3/8/8/8/pP6/8/8/4K3 b - b3 0 1", {Move(A4, A3), Move(A4, B3, EN_PASSANT)});

	// Without en passant square, no en passant move.
	check_pawn_moves("4k3/8/8/3pP3/8/8/8/4K3 w - - 0 1", {Move(E5, E6)});
}

void test_movegen() {
	init_attacks();
	test_pawn_pushes();
	test_pawn_captures();
	test_pawn_promotions();
	test_pawn_en_passant();
}