#include "position.h"
#include <iostream>
#include <sstream>

constexpr std::string_view PIECE_CHARS = "PNBRQKpnbrqk";
constexpr std::string_view CASTLING_CHARS = "KQkq";

bool Position::set_fen(const std::string& fen) {
	std::istringstream iss(fen);

	std::string placement, side_to_move, castling, en_passant;
	if (!(iss >> placement >> side_to_move >> castling >> en_passant))
		return false;

	Position new_position = Position();

	int halfmove = 0, fullmove = 1;
	if (!(iss >> halfmove))
		halfmove = 0;
	if (!(iss >> fullmove))
		fullmove = 1;

	if (halfmove < 0 || halfmove > 255)
		return false;
	if (fullmove < 1)
		fullmove = 1;

	new_position.halfmove_ = static_cast<uint8_t>(halfmove);
	new_position.fullmove_ = static_cast<uint16_t>(fullmove);

	int line = 7;
	int column = 0;
	for (char c : placement) {
		if (c == '/') {
			if (line == 0 || column != 8)
				return false;
			line--;
			column = 0;
		} else if (c >= '1' && c <= '8') {
			column += c - '0';
		} else {
			const size_t i = PIECE_CHARS.find(c);
			if (i == std::string_view::npos)
				return false;
			if (column >= 8 || line < 0)
				return false;
			int square = 8 * line + column;
			new_position.put_piece(Color(i / 6), PieceType(i % 6), square);
			column++;
		}
	}

	if (line != 0 || column != 8)
		return false;

	if (side_to_move == "w")
		new_position.side_to_move_ = WHITE;
	else if (side_to_move == "b")
		new_position.side_to_move_ = BLACK;
	else
		return false;

	if (castling == "-")
		new_position.castling_ = 0;
	else
		for (char c : castling) {
			const size_t i = CASTLING_CHARS.find(c);
			if (i == std::string_view::npos)
				return false;
			new_position.castling_ |= 1 << i;
		}

	if (en_passant == "-")
		new_position.ep_square_ = NO_SQUARE;
	else {
		if (en_passant.size() != 2)
			return false;
		char file = en_passant[0];
		char rank = en_passant[1];
		if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
			return false;
		new_position.ep_square_ = Square((rank - '1') * 8 + (file - 'a'));
	}

	*this = new_position;
	return true;
}

std::string Position::fen() const {
	std::ostringstream oss;

	for (int line = 7; line >= 0; --line) {
		int empty_count = 0;
		for (int column = 0; column < 8; ++column) {
			int square = 8 * line + column;
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
	std::cout << "    ┌─────────────────┐\n";
	for (int rank = 7; rank >= 0; --rank) {
		std::cout << "  " << (rank + 1) << " │ ";
		for (int file = 0; file < 8; ++file) {
			int square = rank * 8 + file;
			int piece = board_[square];
			int color = piece_color(piece);
			int type = piece_type(piece);
			char c = (piece == EMPTY) ? '.' : PIECE_CHARS[color * 6 + type];
			std::cout << c << ' ';
		}
		std::cout << "│\n";
	}
	std::cout << "    └─────────────────┘\n";
	std::cout << "      A B C D E F G H\n\n";

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