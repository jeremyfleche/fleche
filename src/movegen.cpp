#include "movegen.h"
#include "attacks.h"
#include "types.h"

template <int D> constexpr Bitboard shift(Bitboard b) { return D > 0 ? b << D : b >> -D; }

template <Color Us> static void generate_pawn_moves(const Position& pos, MoveList& list) {
	constexpr Color Them = Color(Us ^ 1);
	constexpr int Up = (Us == WHITE) ? 8 : -8;

	// Left and right relative to the board
	constexpr int UpLeft = Up - 1;
	constexpr int UpRight = Up + 1;

	const Bitboard pawns = pos.pieces(Us, PAWN);
	const Bitboard empty = ~pos.occupied();
	const Bitboard ennemies = pos.colors(Them);
	const Bitboard file_A = 0x0101010101010101;
	const Bitboard file_H = 0x8080808080808080;

	Bitboard start_rank, second_rank, can_promote_rank, can_capture_en_passant_rank;
	if constexpr (Us == WHITE) {
		start_rank = 0x000000000000FF00;
		second_rank = 0x0000000000FF0000;
		can_promote_rank = 0x00FF000000000000;
		can_capture_en_passant_rank = 0x000000FF00000000;
	} else {
		start_rank = 0x00FF000000000000;
		second_rank = 0x0000FF0000000000;
		can_promote_rank = 0x000000000000FF00;
		can_capture_en_passant_rank = 0x00000000FF000000;
	}

	const Bitboard can_promote_pawns = pawns & can_promote_rank;
	const Bitboard other_pawns = pawns & ~can_promote_rank;

	// Single pushes
	Bitboard single_push = shift<Up>(other_pawns) & empty;

	// Double pushes (equivalent to single pushes on start rank already single pushed)
	Bitboard double_push = shift<Up>(single_push & second_rank) & empty;

	while (single_push) {
		const Square to = pop_lsb(single_push);
		const Square from = static_cast<Square>(to - Up);
		list.add(Move(from, to));
	}

	while (double_push) {
		const Square to = pop_lsb(double_push);
		const Square from = static_cast<Square>(to - 2 * Up);
		list.add(Move(from, to, DOUBLE_PUSH));
	}

	// left captures (file A side)
	Bitboard left_captures = shift<UpLeft>(other_pawns & ~file_A) & ennemies;
	while (left_captures) {
		const Square to = pop_lsb(left_captures);
		const Square from = static_cast<Square>(to - UpLeft);
		list.add(Move(from, to, CAPTURE));
	}

	// right captures (file H side)
	Bitboard right_captures = shift<UpRight>(other_pawns & ~file_H) & ennemies;
	while (right_captures) {
		const Square to = pop_lsb(right_captures);
		const Square from = static_cast<Square>(to - UpRight);
		list.add(Move(from, to, CAPTURE));
	}

	// push promotions
	Bitboard push_promotions = shift<Up>(can_promote_pawns) & empty;
	while (push_promotions) {
		const Square to = pop_lsb(push_promotions);
		const Square from = static_cast<Square>(to - Up);
		list.add(Move(from, to, QUEEN_PROMO));
		list.add(Move(from, to, ROOK_PROMO));
		list.add(Move(from, to, KNIGHT_PROMO));
		list.add(Move(from, to, BISHOP_PROMO));
	}

	// promotions with left catpure (file A side)
	Bitboard left_capture_promotions = shift<UpLeft>(can_promote_pawns & ~file_A) & ennemies;
	while (left_capture_promotions) {
		const Square to = pop_lsb(left_capture_promotions);
		const Square from = static_cast<Square>(to - UpLeft);
		list.add(Move(from, to, QUEEN_PROMO_CAPTURE));
		list.add(Move(from, to, ROOK_PROMO_CAPTURE));
		list.add(Move(from, to, KNIGHT_PROMO_CAPTURE));
		list.add(Move(from, to, BISHOP_PROMO_CAPTURE));
	}

	// promotions with right capture (file H side)
	Bitboard right_capture_promotions = shift<UpRight>(can_promote_pawns & ~file_H) & ennemies;
	while (right_capture_promotions) {
		const Square to = pop_lsb(right_capture_promotions);
		const Square from = static_cast<Square>(to - UpRight);
		list.add(Move(from, to, QUEEN_PROMO_CAPTURE));
		list.add(Move(from, to, ROOK_PROMO_CAPTURE));
		list.add(Move(from, to, KNIGHT_PROMO_CAPTURE));
		list.add(Move(from, to, BISHOP_PROMO_CAPTURE));
	}

	// en passant
	const Square ep = pos.ep_square();
	if (ep != NO_SQUARE) {
		Bitboard takers = pawn_attacks(Them, ep) & pawns;
		while (takers)
			list.add(Move(pop_lsb(takers), ep, EN_PASSANT));
	}
}

template <PieceType Pt> static void generate_piece_moves(const Position& pos, MoveList& list, Color Us) {
	const Bitboard occ = pos.occupied();
	const Bitboard empty = ~occ;
	const Bitboard enemies = pos.colors(Color(Us ^ 1));

	Bitboard pieces = pos.pieces(Us, Pt);

	while (pieces) {
		const Square from = pop_lsb(pieces);
		Bitboard attacks;
		if constexpr (Pt == KNIGHT)
			attacks = knight_attacks(from);
		else if constexpr (Pt == BISHOP)
			attacks = bishop_attacks(from, occ);
		else if constexpr (Pt == ROOK)
			attacks = rook_attacks(from, occ);
		else if constexpr (Pt == QUEEN)
			attacks = queen_attacks(from, occ);
		else
			attacks = king_attacks(from);

		Bitboard empty_squares = attacks & empty;
		while (empty_squares)
			list.add(Move(from, pop_lsb(empty_squares)));

		Bitboard enemy = attacks & enemies;
		while (enemy)
			list.add(Move(from, pop_lsb(enemy), CAPTURE));
	}
}

template <Color Us> static void generate_castling_moves(const Position& pos, MoveList& list) {
	constexpr int OO = (Us == WHITE) ? WHITE_OO : BLACK_OO;
	constexpr int OOO = (Us == WHITE) ? WHITE_OOO : BLACK_OOO;
	constexpr Bitboard OO_EMPTY = (Us == WHITE) ? bb(F1, G1) : bb(F8, G8);
	constexpr Bitboard OOO_EMPTY = (Us == WHITE) ? bb(B1, C1, D1) : bb(B8, C8, D8);
	constexpr Square from = (Us == WHITE) ? E1 : E8;
	constexpr Square OO_to = (Us == WHITE) ? G1 : G8;
	constexpr Square OOO_to = (Us == WHITE) ? C1 : C8;
	constexpr Color Them = Color(Us ^ 1);
	constexpr Square OO_through = (Us == WHITE) ? F1 : F8;
	constexpr Square OOO_through = (Us == WHITE) ? D1 : D8;

	if ((pos.castling() & OO) && !(pos.occupied() & OO_EMPTY) && !pos.is_square_attacked(from, Them) &&
	    !pos.is_square_attacked(OO_through, Them) && !pos.is_square_attacked(OO_to, Them))
		list.add(Move(from, OO_to, KING_CASTLE));
	if ((pos.castling() & OOO) && !(pos.occupied() & OOO_EMPTY) && !pos.is_square_attacked(from, Them) &&
	    !pos.is_square_attacked(OOO_through, Them) && !pos.is_square_attacked(OOO_to, Them))
		list.add(Move(from, OOO_to, QUEEN_CASTLE));
}

template <Color Us> void generate_all(const Position& pos, MoveList& list) {
	generate_pawn_moves<Us>(pos, list);
	generate_piece_moves<KNIGHT>(pos, list, Us);
	generate_piece_moves<BISHOP>(pos, list, Us);
	generate_piece_moves<ROOK>(pos, list, Us);
	generate_piece_moves<QUEEN>(pos, list, Us);
	generate_piece_moves<KING>(pos, list, Us);
	generate_castling_moves<Us>(pos, list);
}

void generate_moves(const Position& pos, MoveList& list) {
	if (pos.side_to_move() == WHITE)
		generate_all<WHITE>(pos, list);
	else
		generate_all<BLACK>(pos, list);
}
