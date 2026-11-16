#include "position.h"
#include "attacks.h"
#include <charconv>
#include <iostream>
#include <sstream>
#include <unistd.h>

constexpr std::string_view PIECE_CHARS = "PNBRQKpnbrqk";
constexpr std::string_view CASTLING_CHARS = "KQkq";

namespace {
// Square backgrounds (Lichess-like palette)
constexpr const char* LIGHT_SQUARE = "\033[48;2;240;217;181m";
constexpr const char* DARK_SQUARE = "\033[48;2;181;136;99m";

// Piece foregrounds: 1 = bold, 22 = back to normal intensity (otherwise bold leaks onto black pieces)
constexpr const char* WHITE_PIECE = "\033[1;38;2;255;255;255m";
constexpr const char* BLACK_PIECE = "\033[22;38;2;0;0;0m";
constexpr const char* RESET_COLOR = "\033[0m";

// Filled glyphs for both colors, indexed by PieceType.
// \uFE0E forces text rendering of the pawn (some terminals draw it as a 2-column emoji).
constexpr const char* PIECE_GLYPHS[6] = {"♟\uFE0E", "♞", "♝", "♜", "♛", "♚"};
} // namespace

// Parses the whole string as a decimal integer. Fails on empty input, trailing characters or overflow.
static bool parse_int(const std::string& s, int& value) {
	const char* first = s.data();
	const char* last = s.data() + s.size();
	auto [ptr, ec] = std::from_chars(first, last, value);
	return ec == std::errc() && ptr == last;
}

bool Position::set_fen(const std::string& fen, std::string* error) {
	std::istringstream iss(fen);

	std::string placement, side_to_move, castling, en_passant;
	if (!(iss >> placement >> side_to_move >> castling >> en_passant)) {
		if (error)
			*error = "Invalid FEN string: not enough fields";
		return false;
	}

	Position new_position = Position();

	std::string halfmove_str, fullmove_str, extra;
	iss >> halfmove_str >> fullmove_str >> extra;
	if (!extra.empty()) {
		if (error)
			*error = "Too many fields in FEN string";
		return false;
	}
	int halfmove = 0, fullmove = 1;
	if (!halfmove_str.empty() && !parse_int(halfmove_str, halfmove)) {
		if (error)
			*error = "Invalid halfmove value: " + halfmove_str;
		return false;
	}
	if (!fullmove_str.empty() && !parse_int(fullmove_str, fullmove)) {
		if (error)
			*error = "Invalid fullmove value: " + fullmove_str;
		return false;
	}
	if (halfmove < 0 || halfmove > 255 || fullmove < 0 || fullmove > 65535) {
		if (error)
			*error =
			    "Invalid move values: halfmove=" + std::to_string(halfmove) + ", fullmove=" + std::to_string(fullmove);
		return false;
	}
	if (fullmove == 0)
		fullmove = 1;

	new_position.halfmove_ = static_cast<uint8_t>(halfmove);
	new_position.fullmove_ = static_cast<uint16_t>(fullmove);

	int line = 7;
	int column = 0;
	bool prev_was_digit = false;
	for (char c : placement) {
		if (c == '/') {
			if (column != 8) {
				if (error)
					*error = "Invalid piece placement: wrong number of files";
				return false;
			}
			if (line == 0) {
				if (error)
					*error = "Invalid piece placement: wrong number of ranks";
				return false;
			}
			line--;
			column = 0;
			prev_was_digit = false;
		} else if (c >= '1' && c <= '8') {
			if (prev_was_digit) {
				if (error)
					*error = "Invalid piece placement: consecutive digits";
				return false;
			}
			prev_was_digit = true;
			column += c - '0';
		} else {
			const size_t i = PIECE_CHARS.find(c);
			if (i == std::string_view::npos) {
				if (error)
					*error = "Invalid piece character: " + std::string(1, c);
				return false;
			}
			if (column >= 8 || line < 0) {
				if (error)
					*error =
					    "Invalid piece placement: line " + std::to_string(line) + ", column " + std::to_string(column);
				return false;
			}
			int square = 8 * line + column;
			new_position.put_piece(Color(i / 6), PieceType(i % 6), square);
			prev_was_digit = false;
			column++;
		}
	}

	if (column != 8) {
		if (error)
			*error = "Invalid piece placement: wrong number of files";
		return false;
	}
	if (line != 0) {
		if (error)
			*error = "Invalid piece placement: wrong number of ranks";
		return false;
	}

	if (__builtin_popcountll(new_position.pieces(WHITE, KING)) != 1 ||
	    __builtin_popcountll(new_position.pieces(BLACK, KING)) != 1) {
		if (error)
			*error = "Invalid number of kings: white=" +
			         std::to_string(__builtin_popcountll(new_position.pieces(WHITE, KING))) +
			         ", black=" + std::to_string(__builtin_popcountll(new_position.pieces(BLACK, KING)));
		return false;
	}

	if (new_position.pieces(PAWN) & 0xFF000000000000FFULL) {
		if (error)
			*error = "Invalid pawn placement: pawns on the first or last rank";
		return false;
	}

	if (side_to_move == "w")
		new_position.side_to_move_ = WHITE;
	else if (side_to_move == "b")
		new_position.side_to_move_ = BLACK;
	else {
		if (error)
			*error = "Invalid side to move: " + side_to_move;
		return false;
	}

	if (castling == "-")
		new_position.castling_ = 0;
	else {
		int castling_index = -1;
		for (char c : castling) {
			const size_t i = CASTLING_CHARS.find(c);
			if (i == std::string_view::npos || static_cast<int>(i) <= castling_index) {
				if (error)
					*error = "Invalid castling rights: " + castling;
				return false;
			}
			castling_index = i;
			new_position.castling_ |= 1 << i;

			Square king_square = (i < 2) ? E1 : E8;
			Square rook_square = (i == 0) ? H1 : (i == 1) ? A1 : (i == 2) ? H8 : A8;

			int king_piece = new_position.piece_on(king_square);
			int rook_piece = new_position.piece_on(rook_square);
			if (king_piece != make_piece((i < 2) ? WHITE : BLACK, KING) ||
			    rook_piece != make_piece((i < 2) ? WHITE : BLACK, ROOK)) {
				if (error)
					*error = "Rook or king not in the correct square for castling rights: " + castling;
				return false;
			}
		}
	}

	if (en_passant == "-")
		new_position.ep_square_ = NO_SQUARE;
	else {
		if (en_passant.size() != 2) {
			if (error)
				*error = "Invalid en passant square: " + en_passant;
			return false;
		}
		char rank = en_passant[1];
		char file = en_passant[0];
		Square ep_square = Square((rank - '1') * 8 + (file - 'a'));

		// Check that the en passant square is valid
		if (file < 'a' || file > 'h') {
			if (error)
				*error = "Invalid en passant square: " + en_passant;
			return false;
		}
		// Check that the en passant square is on the correct rank and that there is a pawn of the correct color on the
		// correct square
		if (new_position.side_to_move_ == WHITE && rank != '6') {
			if (error)
				*error = "En passant square must be on rank 6 for white to move: " + en_passant;
			return false;
		}
		if (new_position.side_to_move_ == BLACK && rank != '3') {
			if (error)
				*error = "En passant square must be on rank 3 for black to move: " + en_passant;
			return false;
		}

		// Check that the en passant square is not occupied and that there is a pawn of the correct color on the correct
		// square
		if (new_position.piece_on(ep_square) != EMPTY) {
			if (error)
				*error = "En passant square is occupied: " + en_passant;
			return false;
		}

		Bitboard occupied = new_position.colors(WHITE) | new_position.colors(BLACK);
		int up = (new_position.side_to_move_ == WHITE) ? 8 : -8;
		Color opponent_color = (new_position.side_to_move_ == WHITE) ? BLACK : WHITE;

		// Check that there is a pawn that did the double move to create the en passant square
		if ((new_position.pieces(opponent_color, PAWN) & sq_bb(ep_square - up)) == 0) {
			if (error)
				*error = "No pawn that did the double move: " + en_passant;
			return false;
		}

		// Check that the starting square of the pawn that did the double move is free (i.e., not occupied by any piece)
		if ((occupied & sq_bb(ep_square + up)) != 0) {
			if (error)
				*error = "Starting square of the pawn that did the double move is occupied: " + en_passant;
			return false;
		}

		Bitboard attacking_pawns = pawn_attacks(opponent_color, ep_square);

		// Check that there is a pawn of the correct color that can capture en passant
		if ((new_position.pieces(new_position.side_to_move_, PAWN) & attacking_pawns) != 0) {
			new_position.ep_square_ = ep_square;
		}
	}

	*this = new_position;
	return true;
}

std::string Position::fen() const {
	std::ostringstream oss;

	for (int line = 7; line >= 0; --line) {
		int empty_count = 0;
		for (int column = 0; column < 8; ++column) {
			Square square = static_cast<Square>(8 * line + column);
			int piece = piece_on(square);

			if (piece == EMPTY) {
				++empty_count;
				continue;
			}

			int color = piece_color(piece);
			int type = piece_type(piece);

			if (empty_count > 0) {
				oss << empty_count;
				empty_count = 0;
			}

			int index = color * 6 + type;
			oss << PIECE_CHARS[index];
		}
		if (empty_count > 0)
			oss << empty_count;
		if (line > 0)
			oss << '/';
	}

	oss << ' ' << (side_to_move_ == WHITE ? 'w' : 'b') << ' ';

	if (castling_ == 0)
		oss << '-';
	else {
		for (size_t i = 0; i < CASTLING_CHARS.size(); ++i) {
			if (castling_ & (1 << i))
				oss << CASTLING_CHARS[i];
		}
	}

	oss << ' ';
	if (ep_square_ == NO_SQUARE)
		oss << '-';
	else {
		int file = ep_square_ % 8;
		int rank = ep_square_ / 8;
		oss << static_cast<char>('a' + file) << static_cast<char>('1' + rank);
	}

	oss << ' ' << static_cast<int>(halfmove_) << ' ' << static_cast<int>(fullmove_);

	return oss.str();
}

void Position::display() const {
	// No escape codes when output is piped (UCI GUI, logs, tests)
	const bool pretty = isatty(STDOUT_FILENO);

	for (int rank = 7; rank >= 0; --rank) {
		std::cout << ' ' << (rank + 1) << ' ';

		for (int file = 0; file < 8; ++file) {
			int piece = board_[rank * 8 + file];

			if (!pretty) {
				char c = (piece == EMPTY) ? '.' : PIECE_CHARS[piece_color(piece) * 6 + piece_type(piece)];
				std::cout << ' ' << c << ' ';
				continue;
			}

			bool light = (rank + file) % 2 == 1; // a1 (0 + 0) is dark
			std::cout << (light ? LIGHT_SQUARE : DARK_SQUARE);

			if (piece == EMPTY)
				std::cout << "  ";
			else
				std::cout << (piece_color(piece) == WHITE ? WHITE_PIECE : BLACK_PIECE)
				          << PIECE_GLYPHS[piece_type(piece)] << ' ';
		}

		if (pretty)
			std::cout << RESET_COLOR;
		std::cout << '\n';
	}

	std::cout << "   A B C D E F G H\n\n";
	std::cout << "FEN: " << fen() << '\n';
}

bool Position::is_consistent() const {
	Bitboard all_pieces = 0;

	for (int type = 0; type < 6; ++type) {
		if (all_pieces & pieces_[type])
			return false;
		all_pieces |= pieces_[type];
	}

	if (all_pieces != (colors_[WHITE] | colors_[BLACK]))
		return false;

	if (colors_[WHITE] & colors_[BLACK])
		return false;

	for (int square = 0; square < 64; ++square) {
		int piece = board_[square];
		if (piece == EMPTY) {
			if ((all_pieces & sq_bb(square)) != 0)
				return false;
			if ((colors_[WHITE] & sq_bb(square)) != 0)
				return false;
			if ((colors_[BLACK] & sq_bb(square)) != 0)
				return false;
		} else {
			if ((pieces_[piece_type(piece)] & sq_bb(square)) == 0)
				return false;
			if ((colors_[piece_color(piece)] & sq_bb(square)) == 0)
				return false;
		}
	}
	return true;
}

Bitboard Position::attackers_to(Square square, Bitboard occ) const {

	Bitboard attackers = 0;

	attackers |= PAWN_ATTACKS[WHITE][square] & pieces(BLACK, PAWN);
	attackers |= PAWN_ATTACKS[BLACK][square] & pieces(WHITE, PAWN);
	attackers |= KNIGHT_ATTACKS[square] & pieces(KNIGHT);
	attackers |= KING_ATTACKS[square] & pieces(KING);
	attackers |= rook_attacks(square, occ) & (pieces(ROOK) | pieces(QUEEN));
	attackers |= bishop_attacks(square, occ) & (pieces(BISHOP) | pieces(QUEEN));

	return attackers;
}