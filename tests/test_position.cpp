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

static constexpr const char* START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static void test_fen_startpos() {
	Position p{};
	CHECK(p.set_fen(START_FEN));
	CHECK(p.is_consistent());
	CHECK(p.fen() == START_FEN);
	CHECK(p.side_to_move() == WHITE);
	CHECK(p.ep_square() == NO_SQUARE);

	CHECK(p.pieces(WHITE, PAWN) == 0x000000000000FF00ULL);
	CHECK(p.pieces(BLACK, PAWN) == 0x00FF000000000000ULL);
	CHECK(p.pieces(WHITE, KNIGHT) == (sq_bb(B1) | sq_bb(G1)));
	CHECK(p.pieces(BLACK, KNIGHT) == (sq_bb(B8) | sq_bb(G8)));
	CHECK(p.pieces(WHITE, BISHOP) == (sq_bb(C1) | sq_bb(F1)));
	CHECK(p.pieces(BLACK, BISHOP) == (sq_bb(C8) | sq_bb(F8)));
	CHECK(p.pieces(WHITE, ROOK) == (sq_bb(A1) | sq_bb(H1)));
	CHECK(p.pieces(BLACK, ROOK) == (sq_bb(A8) | sq_bb(H8)));
	CHECK(p.pieces(WHITE, QUEEN) == sq_bb(D1));
	CHECK(p.pieces(BLACK, QUEEN) == sq_bb(D8));
	CHECK(p.pieces(WHITE, KING) == sq_bb(E1));
	CHECK(p.pieces(BLACK, KING) == sq_bb(E8));
	CHECK(p.colors(WHITE) == 0x000000000000FFFFULL);
	CHECK(p.colors(BLACK) == 0xFFFF000000000000ULL);

	CHECK(p.piece_on(A1) == make_piece(WHITE, ROOK));
	CHECK(p.piece_on(E1) == make_piece(WHITE, KING));
	CHECK(p.piece_on(D8) == make_piece(BLACK, QUEEN));
	CHECK(p.piece_on(H8) == make_piece(BLACK, ROOK));
	for (int sq = A3; sq <= H6; ++sq)
		CHECK(p.piece_on(sq) == EMPTY);
}

static void test_fen_roundtrip() {
	const char* fens[] = {
	    START_FEN,
	    "rnbqkbnr/ppp1pppp/8/8/3pP3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
	    "rnbqkbnr/pp1ppppp/8/2pP4/4P3/8/PPP2PPP/RNBQKBNR w KQkq c6 0 2",
	    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
	    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
	    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
	    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
	    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
	    "4k3/8/8/8/8/8/8/4K3 b - - 99 150",
	    "4k3/8/8/8/8/8/8/4K2R w K - 0 1",
	    "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",
	    "4k2r/8/8/8/8/8/8/4K3 w k - 0 1",
	    "r3k3/8/8/8/8/8/8/4K3 w q - 0 1",
	    "r3k3/8/8/8/8/8/8/4K2R w Kq - 0 1",
	    "7k/8/8/8/8/8/8/K7 w - - 255 65535",
	};
	for (const char* fen : fens) {
		Position p{};
		std::string error;
		CHECK(p.set_fen(fen, &error), std::string(fen) + " -> " + error);
		CHECK(p.is_consistent(), fen);
		CHECK(p.fen() == fen, std::string(fen) + " -> " + p.fen());
	}
}

static void test_fen_fields() {
	Position p{};
	std::string error;

	CHECK(p.set_fen("4k3/8/8/8/8/8/8/4K3 b - - 0 1", &error), error);
	CHECK(p.side_to_move() == BLACK);

	// En passant square is cleared when no pawn can capture
	CHECK(p.set_fen("4k3/8/8/8/4P3/8/8/4K3 b - e3 0 1", &error), error);
	CHECK(p.ep_square() == NO_SQUARE);
	CHECK(p.set_fen("4k3/8/8/3p4/8/8/8/4K3 w - d6 0 1", &error), error);
	CHECK(p.ep_square() == NO_SQUARE);
	CHECK(p.set_fen("4k3/8/8/8/P7/8/8/4K3 b - a3 0 1", &error), error);
	CHECK(p.ep_square() == NO_SQUARE);
	CHECK(p.set_fen("4k3/8/8/7p/8/8/8/4K3 w - h6 0 1", &error), error);
	CHECK(p.ep_square() == NO_SQUARE);

	// En passant square is set when a pawn can capture
	CHECK(p.set_fen("4k3/8/8/8/3pPp2/8/8/4K3 b - e3 0 1", &error), error);
	CHECK(p.ep_square() == E3);
	CHECK(p.set_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1", &error), error);
	CHECK(p.ep_square() == D6);
	CHECK(p.set_fen("4k3/8/8/8/Pp6/8/8/4K3 b - a3 0 1", &error), error);
	CHECK(p.ep_square() == A3);
	CHECK(p.set_fen("4k3/8/8/6Pp/8/8/8/4K3 w - h6 0 1", &error), error);
	CHECK(p.ep_square() == H6);

	// Pieces on edge squares
	CHECK(p.set_fen("qk5r/8/8/8/8/8/3K4/B6N w - - 0 1", &error), error);
	CHECK(p.piece_on(A8) == make_piece(BLACK, QUEEN));
	CHECK(p.piece_on(H8) == make_piece(BLACK, ROOK));
	CHECK(p.piece_on(A1) == make_piece(WHITE, BISHOP));
	CHECK(p.piece_on(H1) == make_piece(WHITE, KNIGHT));
	CHECK(p.colors(WHITE) == bb(A1, H1, D2));
	CHECK(p.colors(BLACK) == bb(A8, H8, B8));
}

static void test_fen_move_counters() {
	Position p{};
	std::string error;

	// Counters are optional and default to "0 1"
	CHECK(p.set_fen("4k3/8/8/8/8/8/8/4K3 w - -", &error), error);
	CHECK(p.fen() == "4k3/8/8/8/8/8/8/4K3 w - - 0 1");

	// Only the halfmove clock
	CHECK(p.set_fen("4k3/8/8/8/8/8/8/4K3 w - - 12", &error), error);
	CHECK(p.fen() == "4k3/8/8/8/8/8/8/4K3 w - - 12 1");

	// Fullmove 0 is clamped to 1
	CHECK(p.set_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 0", &error), error);
	CHECK(p.fen() == "4k3/8/8/8/8/8/8/4K3 w - - 0 1");

	// Extra whitespace between fields is tolerated
	CHECK(p.set_fen("  4k3/8/8/8/8/8/8/4K3   b  -  -  3  42  ", &error), error);
	CHECK(p.fen() == "4k3/8/8/8/8/8/8/4K3 b - - 3 42");

	// Halfmove clock out of range
	CHECK(!p.set_fen("4k3/8/8/8/8/8/8/4K3 w - - -1 1", &error), error);
	CHECK(!p.set_fen("4k3/8/8/8/8/8/8/4K3 w - - 256 1", &error), error);
}

// set_fen() must fail with an error starting with expected_error, and leave the position untouched.
static void check_invalid_fen(const char* fen, const char* expected_error) {
	Position p{};
	std::string error;
	CHECK(p.set_fen(START_FEN, &error), START_FEN);
	CHECK(!p.set_fen(fen, &error), std::string(fen) + " -> " + error);
	CHECK(error.starts_with(expected_error), std::string(fen) + " -> " + error);
	CHECK(p.fen() == START_FEN, fen);
	CHECK(p.is_consistent(), fen);
}

static void test_fen_invalid() {
	// Missing fields
	check_invalid_fen("", "Invalid FEN string: not enough fields");
	check_invalid_fen("   ", "Invalid FEN string: not enough fields");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", "Invalid FEN string: not enough fields");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w", "Invalid FEN string: not enough fields");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq", "Invalid FEN string: not enough fields");

	// Placement: wrong number of ranks
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of ranks");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of ranks");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of ranks");

	// Placement: wrong number of files (an empty rank has 0 files)
	check_invalid_fen("/rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of files");
	check_invalid_fen("rnbqkbnr//pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of files");
	check_invalid_fen("rnbqkbn/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of files");
	check_invalid_fen("rnbqkbnrr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Invalid piece placement: line");
	check_invalid_fen("rnbqkbnr/pppppppp/7/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of files");
	check_invalid_fen("rnbqkbnr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Invalid piece character");
	check_invalid_fen("rnbqkbnr/pppppppp/44p/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	                  "Invalid piece placement: consecutive digits");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBN w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of files");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR1 w KQkq - 0 1",
	                  "Invalid piece placement: wrong number of files");

	// Placement: invalid characters
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNX w KQkq - 0 1", "Invalid piece character");
	check_invalid_fen("rnbqkbnr/pppppppp/0/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "Invalid piece character");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR- w KQkq - 0 1", "Invalid piece character");

	// Side to move
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR W KQkq - 0 1", "Invalid side to move");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1", "Invalid side to move");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR wb KQkq - 0 1", "Invalid side to move");

	// Castling
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkx - 0 1", "Invalid castling rights");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ-kq - 0 1", "Invalid castling rights");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1", "Invalid castling rights");
	check_invalid_fen("r3k2r/8/8/8/8/8/8/R3K2R w qkQK - 0 1", "Invalid castling rights");
	check_invalid_fen("7k/8/8/8/8/8/8/K7 w K - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // no king on e1, no rook on h1
	check_invalid_fen("7k/8/8/8/8/8/8/K7 w Q - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // no king on e1, no rook on a1
	check_invalid_fen("7k/8/8/8/8/8/8/K7 w k - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // no king on e8, no rook on h8
	check_invalid_fen("7k/8/8/8/8/8/8/K7 w q - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // no king on e8, no rook on a8
	check_invalid_fen(
	    "7k/8/8/8/8/8/8/K7 w Kq - 0 1",
	    "Rook or king not in the correct square for castling rights"); // no king on e1/e8, no rook on h1/a8
	check_invalid_fen("4k3/8/8/8/8/8/8/4K3 w K - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // king on e1, no rook on h1
	check_invalid_fen("4k3/8/8/8/8/8/8/5K1R w K - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // rook on h1, king not on e1
	check_invalid_fen("4k3/8/8/8/8/8/8/R3K3 w K - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // rook on a1 instead of h1
	check_invalid_fen("4k3/8/8/8/8/8/8/4K2r w K - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // black rook on h1
	check_invalid_fen("4k3/8/8/8/8/8/8/4K2N w K - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // knight on h1
	check_invalid_fen("r3k3/8/8/8/8/8/8/4K3 w k - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // rook on a8 instead of h8
	check_invalid_fen("r4k2/8/8/8/8/8/8/4K3 w q - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // rook on a8, king not on e8
	check_invalid_fen("r3k2r/8/8/8/8/8/8/4K3 w KQkq - 0 1",
	                  "Rook or king not in the correct square for castling rights"); // black can castle, white cannot

	// En passant
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e 0 1", "Invalid en passant square");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e33 0 1", "Invalid en passant square");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq i3 0 1", "Invalid en passant square");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e9 0 1",
	                  "En passant square must be on rank 6 for white to move");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq E3 0 1", "Invalid en passant square");
	check_invalid_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 3e 0 1", "Invalid en passant square");

	check_invalid_fen("4k3/8/8/8/3pP3/8/8/4K3 w - e3 0 1",
	                  "En passant square must be on rank 6 for white to move"); // wrong side to move
	check_invalid_fen("4k3/8/8/8/3pP3/8/8/4K3 b - e4 0 1",
	                  "En passant square must be on rank 3 for black to move");                // wrong rank
	check_invalid_fen("4k3/8/8/8/3p4/8/8/4K3 b - e3 0 1", "No pawn that did the double move"); // no pawn on e4
	check_invalid_fen("4k3/8/8/8/3pp3/8/8/4K3 b - e3 0 1",
	                  "No pawn that did the double move"); // pawn of the wrong color on e4
	check_invalid_fen("4k3/8/8/8/3pP3/4N3/8/4K3 b - e3 0 1",
	                  "En passant square is occupied"); // en passant square occupied
	check_invalid_fen("4k3/8/8/8/3pP3/8/4N3/4K3 b - e3 0 1",
	                  "Starting square of the pawn that did the double move is occupied"); // origin square occupied
	check_invalid_fen("8/8/8/8/8/8/8/8 w - - 0 1", "Invalid number of kings");             // no kings
}

static void test_fen_overwrite() {
	// A successful set_fen() replaces everything from the previous position.
	Position p{};
	CHECK(p.set_fen("r3k2r/8/8/3pP3/8/8/8/R3K2R w KQkq d6 7 30"));
	CHECK(p.set_fen("4k3/8/8/8/8/8/8/4K3 b - - 0 1"));
	CHECK(p.fen() == "4k3/8/8/8/8/8/8/4K3 b - - 0 1");
	CHECK(p.is_consistent());
	CHECK(p.pieces(ROOK) == 0);
	CHECK(p.pieces(PAWN) == 0);
	CHECK(p.ep_square() == NO_SQUARE);
	CHECK(p.piece_on(A1) == EMPTY);
	CHECK(p.colors(WHITE) == sq_bb(E1));
	CHECK(p.colors(BLACK) == sq_bb(E8));
}

static void test_fen() {
	test_fen_startpos();
	test_fen_roundtrip();
	test_fen_fields();
	test_fen_move_counters();
	test_fen_invalid();
	test_fen_overwrite();
}

void test_position() {
	test_constructors();
	test_put();
	test_remove();
	test_move();
	test_fen();
}