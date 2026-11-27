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

static bool is_castling(Move m) { return m.flags() == KING_CASTLE || m.flags() == QUEEN_CASTLE; }

// The generated moves selected by `keep` must be exactly the expected ones:
// same number of moves, and every expected move present (with the right flag).
template <typename Filter>
static void check_moves_if(const char* fen, Filter keep, std::initializer_list<Move> expected) {
	Position pos;
	std::string error;
	CHECK(pos.set_fen(fen, &error), std::string(fen) + " -> " + error);

	MoveList list;
	generate_moves(pos, list);

	int count = 0;
	for (Move m : list)
		if (keep(pos, m))
			++count;
	CHECK(count == static_cast<int>(expected.size()),
	      std::string(fen) + " -> " + std::to_string(count) + " moves, expected " + std::to_string(expected.size()));
	for (Move m : expected)
		CHECK(contains(list, m), std::string(fen) + " -> missing " + move_name(m));
}

// Moves of the pieces of a given type (castling counts as a king move).
static void check_moves(PieceType pt, const char* fen, std::initializer_list<Move> expected) {
	check_moves_if(
	    fen, [pt](const Position& pos, Move m) { return piece_type(pos.piece_on(m.from())) == pt; }, expected);
}

// Castling moves only.
static void check_castling(const char* fen, std::initializer_list<Move> expected) {
	check_moves_if(fen, [](const Position&, Move m) { return is_castling(m); }, expected);
}

// Total number of generated moves.
static void check_move_count(const char* fen, int expected) {
	Position pos;
	std::string error;
	CHECK(pos.set_fen(fen, &error), std::string(fen) + " -> " + error);
	MoveList list;
	generate_moves(pos, list);
	CHECK(list.size == expected,
	      std::string(fen) + " -> " + std::to_string(list.size) + " moves, expected " + std::to_string(expected));
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
	check_moves(PAWN, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	            {Move(A2, A3), Move(B2, B3), Move(C2, C3), Move(D2, D3), Move(E2, E3), Move(F2, F3), Move(G2, G3),
	             Move(H2, H3), Move(A2, A4, DOUBLE_PUSH), Move(B2, B4, DOUBLE_PUSH), Move(C2, C4, DOUBLE_PUSH),
	             Move(D2, D4, DOUBLE_PUSH), Move(E2, E4, DOUBLE_PUSH), Move(F2, F4, DOUBLE_PUSH),
	             Move(G2, G4, DOUBLE_PUSH), Move(H2, H4, DOUBLE_PUSH)});
	check_moves(PAWN, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
	            {Move(A7, A6), Move(B7, B6), Move(C7, C6), Move(D7, D6), Move(E7, E6), Move(F7, F6), Move(G7, G6),
	             Move(H7, H6), Move(A7, A5, DOUBLE_PUSH), Move(B7, B5, DOUBLE_PUSH), Move(C7, C5, DOUBLE_PUSH),
	             Move(D7, D5, DOUBLE_PUSH), Move(E7, E5, DOUBLE_PUSH), Move(F7, F5, DOUBLE_PUSH),
	             Move(G7, G5, DOUBLE_PUSH), Move(H7, H5, DOUBLE_PUSH)});

	// Blocked by an enemy or an own piece: no move.
	check_moves(PAWN, "4k3/8/8/8/8/4n3/4P3/4K3 w - - 0 1", {});
	check_moves(PAWN, "4k3/8/8/8/8/4N3/4P3/4K3 w - - 0 1", {});
	check_moves(PAWN, "4k3/4p3/4N3/8/8/8/8/4K3 b - - 0 1", {});

	// Double push blocked on the second square: single push only.
	check_moves(PAWN, "4k3/8/8/8/4n3/8/4P3/4K3 w - - 0 1", {Move(E2, E3)});
	check_moves(PAWN, "4k3/4p3/8/4N3/8/8/8/4K3 b - - 0 1", {Move(E7, E6)});

	// No double push away from the starting rank.
	check_moves(PAWN, "4k3/8/8/8/8/4P3/8/4K3 w - - 0 1", {Move(E3, E4)});
	check_moves(PAWN, "4k3/8/4p3/8/8/8/8/4K3 b - - 0 1", {Move(E6, E5)});
}

static void test_pawn_captures() {
	// Both diagonals, plus the push.
	check_moves(PAWN, "4k3/8/8/3n1r2/4P3/8/8/4K3 w - - 0 1",
	            {Move(E4, E5), Move(E4, D5, CAPTURE), Move(E4, F5, CAPTURE)});
	check_moves(PAWN, "4k3/8/8/4p3/3N1R2/8/8/4K3 b - - 0 1",
	            {Move(E5, E4), Move(E5, D4, CAPTURE), Move(E5, F4, CAPTURE)});

	// Own pieces on the diagonals are not captured.
	check_moves(PAWN, "4k3/8/8/3N1R2/4P3/8/8/4K3 w - - 0 1", {Move(E4, E5)});

	// Pieces behind the pawn are not captured.
	check_moves(PAWN, "4k3/8/8/8/4P3/3n1n2/8/4K3 w - - 0 1", {Move(E4, E5)});

	// No wrap-around between files A and H.
	check_moves(PAWN, "4k3/8/n7/6n1/7P/8/8/4K3 w - - 0 1", {Move(H4, H5), Move(H4, G5, CAPTURE)});
	check_no_move("4k3/8/n7/6n1/7P/8/8/4K3 w - - 0 1", Move(H4, A6, CAPTURE));
	check_moves(PAWN, "4k3/8/8/1n5n/P7/8/8/4K3 w - - 0 1", {Move(A4, A5), Move(A4, B5, CAPTURE)});
	check_no_move("4k3/8/8/1n5n/P7/8/8/4K3 w - - 0 1", Move(A4, H4, CAPTURE));
	check_moves(PAWN, "4k3/8/8/7p/N5N1/8/8/4K3 b - - 0 1", {Move(H5, H4), Move(H5, G4, CAPTURE)});
	check_no_move("4k3/8/8/7p/N5N1/8/8/4K3 b - - 0 1", Move(H5, A3, CAPTURE));

	// Several pawns at once (c4 and e4 are blocked in front).
	check_moves(
	    PAWN, "4k3/8/8/1qrbn3/P1P1P3/8/8/4K3 w - - 0 1",
	    {Move(A4, A5), Move(A4, B5, CAPTURE), Move(C4, B5, CAPTURE), Move(C4, D5, CAPTURE), Move(E4, D5, CAPTURE)});
}

static void test_pawn_promotions() {
	// Promotion by push: 4 moves.
	check_moves(
	    PAWN, "8/4P3/8/8/8/8/8/k3K3 w - - 0 1",
	    {Move(E7, E8, KNIGHT_PROMO), Move(E7, E8, BISHOP_PROMO), Move(E7, E8, ROOK_PROMO), Move(E7, E8, QUEEN_PROMO)});
	check_moves(
	    PAWN, "k3K3/8/8/8/8/8/4p3/8 b - - 0 1",
	    {Move(E2, E1, KNIGHT_PROMO), Move(E2, E1, BISHOP_PROMO), Move(E2, E1, ROOK_PROMO), Move(E2, E1, QUEEN_PROMO)});

	// Promotion by push and by capture: 8 moves.
	check_moves(PAWN, "3r4/4P3/8/8/8/8/8/k3K3 w - - 0 1",
	            {Move(E7, E8, KNIGHT_PROMO), Move(E7, E8, BISHOP_PROMO), Move(E7, E8, ROOK_PROMO),
	             Move(E7, E8, QUEEN_PROMO), Move(E7, D8, KNIGHT_PROMO_CAPTURE), Move(E7, D8, BISHOP_PROMO_CAPTURE),
	             Move(E7, D8, ROOK_PROMO_CAPTURE), Move(E7, D8, QUEEN_PROMO_CAPTURE)});

	// Blocked promotion, captures on both sides.
	check_moves(PAWN, "4nbr1/5P2/8/8/8/8/8/k3K3 w - - 0 1",
	            {Move(F7, E8, KNIGHT_PROMO_CAPTURE), Move(F7, E8, BISHOP_PROMO_CAPTURE),
	             Move(F7, E8, ROOK_PROMO_CAPTURE), Move(F7, E8, QUEEN_PROMO_CAPTURE),
	             Move(F7, G8, KNIGHT_PROMO_CAPTURE), Move(F7, G8, BISHOP_PROMO_CAPTURE),
	             Move(F7, G8, ROOK_PROMO_CAPTURE), Move(F7, G8, QUEEN_PROMO_CAPTURE)});

	// Promotion on file A, no wrap-around to H8.
	check_moves(PAWN, "1r5r/P7/8/8/8/8/8/k3K3 w - - 0 1",
	            {Move(A7, A8, KNIGHT_PROMO), Move(A7, A8, BISHOP_PROMO), Move(A7, A8, ROOK_PROMO),
	             Move(A7, A8, QUEEN_PROMO), Move(A7, B8, KNIGHT_PROMO_CAPTURE), Move(A7, B8, BISHOP_PROMO_CAPTURE),
	             Move(A7, B8, ROOK_PROMO_CAPTURE), Move(A7, B8, QUEEN_PROMO_CAPTURE)});

	// A pawn on the 6th rank does not promote.
	check_moves(PAWN, "4k3/8/4P3/8/8/8/8/4K3 w - - 0 1", {Move(E6, E7)});
}

static void test_pawn_en_passant() {
	check_moves(PAWN, "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1", {Move(E5, E6), Move(E5, D6, EN_PASSANT)});

	// Two pawns can take on the same square.
	check_moves(PAWN, "4k3/8/8/2PpP3/8/8/8/4K3 w - d6 0 1",
	            {Move(C5, C6), Move(C5, D6, EN_PASSANT), Move(E5, E6), Move(E5, D6, EN_PASSANT)});

	check_moves(PAWN, "4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1", {Move(D4, D3), Move(D4, E3, EN_PASSANT)});

	// Edge files.
	check_moves(PAWN, "4k3/8/8/6pP/8/8/8/4K3 w - g6 0 1", {Move(H5, H6), Move(H5, G6, EN_PASSANT)});
	check_moves(PAWN, "4k3/8/8/8/pP6/8/8/4K3 b - b3 0 1", {Move(A4, A3), Move(A4, B3, EN_PASSANT)});

	// Without en passant square, no en passant move.
	check_moves(PAWN, "4k3/8/8/3pP3/8/8/8/4K3 w - - 0 1", {Move(E5, E6)});
}

static void test_knight_moves() {
	// Center: 8 moves.
	check_moves(KNIGHT, "4k3/8/8/8/3N4/8/8/4K3 w - - 0 1",
	            {Move(D4, B3), Move(D4, B5), Move(D4, C2), Move(D4, C6), Move(D4, E2), Move(D4, E6), Move(D4, F3),
	             Move(D4, F5)});

	// Corner and edge, no wrap-around between files A and H.
	check_moves(KNIGHT, "4k3/8/8/8/8/8/8/N3K3 w - - 0 1", {Move(A1, B3), Move(A1, C2)});
	check_moves(KNIGHT, "4k3/8/8/8/7N/8/8/4K3 w - - 0 1", {Move(H4, F3), Move(H4, F5), Move(H4, G2), Move(H4, G6)});

	// Captures on enemies (b3, f5), own pieces blocked (c2, e6).
	check_moves(KNIGHT, "4k3/8/4P3/5p2/3N4/1n6/2P5/4K3 w - - 0 1",
	            {Move(D4, B3, CAPTURE), Move(D4, F5, CAPTURE), Move(D4, B5), Move(D4, C6), Move(D4, E2), Move(D4, F3)});

	// Black to move: only the black knight moves.
	check_moves(KNIGHT, "4k3/8/8/3N4/3n4/8/8/4K3 b - - 0 1",
	            {Move(D4, B3), Move(D4, B5), Move(D4, C2), Move(D4, C6), Move(D4, E2), Move(D4, E6), Move(D4, F3),
	             Move(D4, F5)});
}

static void test_bishop_moves() {
	// Center: 13 moves on the 4 diagonals.
	check_moves(BISHOP, "4k3/8/8/8/3B4/8/8/4K3 w - - 0 1",
	            {Move(D4, C3), Move(D4, B2), Move(D4, A1), Move(D4, E5), Move(D4, F6), Move(D4, G7), Move(D4, H8),
	             Move(D4, C5), Move(D4, B6), Move(D4, A7), Move(D4, E3), Move(D4, F2), Move(D4, G1)});

	// Corner: one diagonal, no wrap-around.
	check_moves(BISHOP, "4k3/8/8/8/8/8/8/4K2B w - - 0 1",
	            {Move(H1, G2), Move(H1, F3), Move(H1, E4), Move(H1, D5), Move(H1, C6), Move(H1, B7), Move(H1, A8)});

	// Stopped by an own pawn (b2), captures that stop the ray (b6, f6).
	check_moves(BISHOP, "4k3/8/1p3n2/8/3B4/8/1P6/4K3 w - - 0 1",
	            {Move(D4, C3), Move(D4, E5), Move(D4, F6, CAPTURE), Move(D4, C5), Move(D4, B6, CAPTURE), Move(D4, E3),
	             Move(D4, F2), Move(D4, G1)});
}

static void test_rook_moves() {
	// Center: 14 moves.
	check_moves(ROOK, "4k3/8/8/8/3R4/8/8/4K3 w - - 0 1",
	            {Move(D4, D5), Move(D4, D6), Move(D4, D7), Move(D4, D8), Move(D4, D3), Move(D4, D2), Move(D4, D1),
	             Move(D4, A4), Move(D4, B4), Move(D4, C4), Move(D4, E4), Move(D4, F4), Move(D4, G4), Move(D4, H4)});

	// Stopped by an own pawn (d6), captures that stop the ray (g4, d1).
	check_moves(ROOK, "4k3/8/3P4/8/3R2p1/8/8/3nK3 w - - 0 1",
	            {Move(D4, D5), Move(D4, E4), Move(D4, F4), Move(D4, G4, CAPTURE), Move(D4, D3), Move(D4, D2),
	             Move(D4, D1, CAPTURE), Move(D4, C4), Move(D4, B4), Move(D4, A4)});

	// Black rook in the corner.
	check_moves(ROOK, "r3k3/8/8/8/8/8/8/4K3 b - - 0 1",
	            {Move(A8, A7), Move(A8, A6), Move(A8, A5), Move(A8, A4), Move(A8, A3), Move(A8, A2), Move(A8, A1),
	             Move(A8, B8), Move(A8, C8), Move(A8, D8)});
}

static void test_queen_moves() {
	// Center: 27 moves (rook + bishop).
	Position pos;
	CHECK(pos.set_fen("4k3/8/8/8/3Q4/8/8/4K3 w - - 0 1"));
	MoveList list;
	generate_moves(pos, list);
	int queen_moves = 0;
	for (Move m : list)
		if (piece_type(pos.piece_on(m.from())) == QUEEN)
			++queen_moves;
	CHECK(queen_moves == 27, std::to_string(queen_moves) + " queen moves");

	// Blockers and captures on files, ranks and diagonals.
	check_moves(QUEEN, "4k3/3p2n1/8/8/3Q4/8/1P6/4K3 w - - 0 1",
	            {Move(D4, D5), Move(D4, D6), Move(D4, D7, CAPTURE), Move(D4, D3),          Move(D4, D2), Move(D4, D1),
	             Move(D4, A4), Move(D4, B4), Move(D4, C4),          Move(D4, E4),          Move(D4, F4), Move(D4, G4),
	             Move(D4, H4), Move(D4, E5), Move(D4, F6),          Move(D4, G7, CAPTURE), Move(D4, C3), Move(D4, C5),
	             Move(D4, B6), Move(D4, A7), Move(D4, E3),          Move(D4, F2),          Move(D4, G1)});
}

static void test_king_moves() {
	// Center: 8 moves.
	check_moves(KING, "4k3/8/8/8/3K4/8/8/8 w - - 0 1",
	            {Move(D4, C3), Move(D4, C4), Move(D4, C5), Move(D4, D3), Move(D4, D5), Move(D4, E3), Move(D4, E4),
	             Move(D4, E5)});

	// Corner: 3 moves, no wrap-around.
	check_moves(KING, "4k3/8/8/8/8/8/8/K7 w - - 0 1", {Move(A1, A2), Move(A1, B1), Move(A1, B2)});
	check_moves(KING, "4k3/8/8/8/8/8/8/7K w - - 0 1", {Move(H1, H2), Move(H1, G1), Move(H1, G2)});

	// Capture (d2), own piece blocked (f2).
	check_moves(KING, "4k3/8/8/8/8/8/3p1P2/4K3 w - - 0 1",
	            {Move(E1, D1), Move(E1, F1), Move(E1, E2), Move(E1, D2, CAPTURE)});
}

static void test_castling() {
	// Both sides, both colors.
	check_castling("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", {Move(E1, G1, KING_CASTLE), Move(E1, C1, QUEEN_CASTLE)});
	check_castling("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", {Move(E8, G8, KING_CASTLE), Move(E8, C8, QUEEN_CASTLE)});

	// Castling rights.
	check_castling("r3k2r/8/8/8/8/8/8/R3K2R w - - 0 1", {});
	check_castling("r3k2r/8/8/8/8/8/8/R3K2R w K - 0 1", {Move(E1, G1, KING_CASTLE)});
	check_castling("r3k2r/8/8/8/8/8/8/R3K2R w Q - 0 1", {Move(E1, C1, QUEEN_CASTLE)});
	check_castling("r3k2r/8/8/8/8/8/8/R3K2R w kq - 0 1", {});

	// Squares between the king and the rook must be empty (b1 included).
	check_castling("r3k2r/8/8/8/8/8/8/R3KB1R w KQkq - 0 1", {Move(E1, C1, QUEEN_CASTLE)});
	check_castling("r3k2r/8/8/8/8/8/8/R3K1NR w KQkq - 0 1", {Move(E1, C1, QUEEN_CASTLE)});
	check_castling("r3k2r/8/8/8/8/8/8/R2QK2R w KQkq - 0 1", {Move(E1, G1, KING_CASTLE)});
	check_castling("r3k2r/8/8/8/8/8/8/R1B1K2R w KQkq - 0 1", {Move(E1, G1, KING_CASTLE)});
	check_castling("r3k2r/8/8/8/8/8/8/RN2K2R w KQkq - 0 1", {Move(E1, G1, KING_CASTLE)});
	check_castling("rn2k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", {Move(E8, G8, KING_CASTLE)});

	// No castling out of check.
	check_castling("r3k2r/8/8/8/1b6/8/8/R3K2R w KQkq - 0 1", {});
	check_castling("r3k2r/8/8/1B6/8/8/8/R3K2R b KQkq - 0 1", {});

	// No castling through or into an attacked square.
	check_castling("r3k2r/8/8/8/2b5/8/8/R3K2R w KQkq - 0 1", {Move(E1, C1, QUEEN_CASTLE)}); // f1
	check_castling("r3k2r/8/8/8/3b4/8/8/R3K2R w KQkq - 0 1", {Move(E1, C1, QUEEN_CASTLE)}); // g1
	check_castling("r3k2r/8/8/8/6b1/8/8/R3K2R w KQkq - 0 1", {Move(E1, G1, KING_CASTLE)});  // d1
	check_castling("r3k2r/8/8/8/5b2/8/8/R3K2R w KQkq - 0 1", {Move(E1, G1, KING_CASTLE)});  // c1
	check_castling("r3k2r/8/8/2B5/8/8/8/R3K2R b KQkq - 0 1", {Move(E8, C8, QUEEN_CASTLE)}); // f8
	check_castling("r3k2r/8/8/6B1/8/8/8/R3K2R b KQkq - 0 1", {Move(E8, G8, KING_CASTLE)});  // d8

	// b1/b8 and the rooks may be attacked.
	check_castling("r3k2r/8/8/8/4b3/8/8/R3K2R w KQkq - 0 1", {Move(E1, G1, KING_CASTLE), Move(E1, C1, QUEEN_CASTLE)});
	check_castling("r3k2r/8/8/4B3/8/8/8/R3K2R b KQkq - 0 1", {Move(E8, G8, KING_CASTLE), Move(E8, C8, QUEEN_CASTLE)});
}

// Properties every generated move must have, whatever the piece.
static void check_move_invariants(const char* fen) {
	Position pos;
	std::string error;
	CHECK(pos.set_fen(fen, &error), std::string(fen) + " -> " + error);
	MoveList list;
	generate_moves(pos, list);

	const Color us = pos.side_to_move();
	for (int i = 0; i < list.size; ++i) {
		const Move m = list.moves[i];
		const std::string name = std::string(fen) + " -> " + move_name(m);
		const int moved = pos.piece_on(m.from());
		const int target = pos.piece_on(m.to());

		CHECK(moved != EMPTY && piece_color(moved) == us, name + ": not a piece of the side to move");
		if (m.flags() == EN_PASSANT || is_castling(m) || !m.is_capture())
			CHECK(target == EMPTY, name + ": target square is not empty");
		else
			CHECK(target != EMPTY && piece_color(target) != us && piece_type(target) != KING,
			      name + ": capture of an own piece, a king or an empty square");
		if (m.is_promotion() || m.flags() == DOUBLE_PUSH || m.flags() == EN_PASSANT)
			CHECK(piece_type(moved) == PAWN, name + ": pawn-only flag on another piece");
		if (is_castling(m))
			CHECK(piece_type(moved) == KING, name + ": castling flag on another piece");
		for (int j = 0; j < i; ++j)
			CHECK(list.moves[j] != m, name + ": generated twice");
	}
}

static void test_move_invariants() {
	const char* fens[] = {
	    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
	    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
	    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1",
	    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
	    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
	    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
	    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
	};
	for (const char* fen : fens)
		check_move_invariants(fen);
}

static void test_move_counts() {
	// Positions without pins, checks or king moves into attacked squares:
	// the pseudo-legal count equals the known legal count (perft 1).
	check_move_count("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20);
	check_move_count("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", 20);
	check_move_count("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 48);
	// 218 moves: the maximum known for a legal position.
	check_move_count("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1", 218);

	// Pseudo-legal: 14 legal moves + b5b6 (pinned pawn) + Ka5-b6 (attacked square).
	check_move_count("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 16);
}

void test_movegen() {
	init_attacks();
	test_pawn_pushes();
	test_pawn_captures();
	test_pawn_promotions();
	test_pawn_en_passant();
	test_knight_moves();
	test_bishop_moves();
	test_rook_moves();
	test_queen_moves();
	test_king_moves();
	test_castling();
	test_move_invariants();
	test_move_counts();
}