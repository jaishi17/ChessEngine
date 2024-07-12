#include "board.hpp"
#include <SFML/Graphics.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>
#include <map>


Move::Move(int pre_sq, int post_sq, piece_type p_type, piece_type c_type, int castling_rights, bool castle, bool capture, bool promotion, bool color_turn){
    move = 0;
    move = pre_sq & (post_sq << 6) & ((int)p_type << 12) & ((int)c_type << 14) & (castling_rights << 17) & ((int)castle << 21) & ((int)capture << 22) & ((int)promotion << 23) & ((int)color_turn << 24);
}

Board::Board(){
    init_Board();
    setToFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

Board::Board(std::string fen){
    init_Board();
    setToFen(fen);
}

void Board::init_Board(){
    generate_pawn_table();
    generate_knight_table();
    generate_king_table();
    init_magics();
}



std::vector<std::pair<piece_color, piece_type>> const Board::getSCBoard() const {
    return sc_board;
}


void Board::clearBitboards(){
    for (int i = 0; i < color_count; ++i){
        for (int j = 0; j < piece_count + 1; ++j){
            piece_bbs[i][j] = 0;
        }
    }
}

void Board::clearSCBoards(){
    for (int i = 0; i < 64; ++i){
        sc_board[i].first = NONE;
    }
    
}

void Board::setToFen(std::string fen){


    clearBitboards();
    clearSCBoards();

    int rank = 8, fen_idx = 0;
    char file = 'a';
    int len = fen.size();

    std::map<char, std::pair<piece_color, piece_type>> fen_map = {{'p', {BLACK, PAWN}}, {'n', {BLACK, KNIGHT}}, {'b', {BLACK, BISHOP}}, 
                                                              {'r', {BLACK, ROOK}}, {'q', {BLACK, QUEEN}},{'k', {BLACK, KING}}, 
                                                              {'P', {WHITE, PAWN}}, {'N', {WHITE, KNIGHT}}, {'B', {WHITE, BISHOP}}, 
                                                              {'R', {WHITE, ROOK}}, {'Q', {WHITE, QUEEN}}, {'K', {WHITE, KING}}};
    while (fen_idx < len){
        if (fen[fen_idx] == '/'){
            rank--;
            file = 'a';
        }
        else if ('0' <= fen[fen_idx] && fen[fen_idx] <= '8'){
            file += (int)(fen[fen_idx] - '0');
        }
        else {
            int pos = (int)(8 * (rank - 1)  + file - 'a');
            file++;

            piece_color pc = fen_map[fen[fen_idx]].first;
            piece_type pt = fen_map[fen[fen_idx]].second;

            piece_bbs[pc][pt] = bit_set_to(piece_bbs[pc][pt], pos, 1);
            piece_bbs[pc][ALL] = bit_set_to(piece_bbs[pc][ALL], pos, 1);
            f_bb = bit_set_to(f_bb, pos, 1);
            sc_board[pos] = {pc, pt};

        }
        fen_idx++;
    }

}

void Board::print_square(u64 n){
    for (int i = 7; i >= 0; i--){
        for (int j = 0; j < 8; ++j){
            std::cout << (((n >> (8 * i + j)) & 1) ? 'X' : '.');
        }
        std::cout << std::endl;
    }
}

void Board::print_square(u64 n, int square){
    for (int i = 7; i >= 0; i--){
        for (int j = 0; j < 8; ++j){
            if (8 * i + j == square){
                std::cout << 'O';
            }
            else{
                std::cout << (((n >> (8 * i + j)) & 1) ? 'X' : '.');
            }
        }
        std::cout << std::endl;
    }
}

void Board::generate_pawn_table(){

    for (int i = 0; i < 64; ++i){
        pawn_push[0][i] = 0;
        pawn_attack[0][i] = 0;
        //white 
        if (i < 56){
            pawn_push[0][i] = bit_set_to(pawn_push[0][i], i + 8, 1);
            if ((int)i/8 == 1){
                pawn_push[0][i] = bit_set_to(pawn_push[0][i], i + 16, 1);
            }
            if (i % 8 != 0){
                pawn_attack[0][i] = bit_set_to(pawn_attack[0][i], i + 7, 1);
            }
            if (i % 8 != 7){
                pawn_attack[0][i] = bit_set_to(pawn_attack[0][i], i + 9, 1);
            }
        }
        
        pawn_push[1][i] = 0;
        pawn_attack[1][i] = 0;
        //black
        if (i > 7){
            pawn_push[1][i] = bit_set_to(pawn_push[1][i], i - 8, 1);
            if ((int)i/8 == 6){
                pawn_push[1][i] = bit_set_to(pawn_push[1][i], i - 16, 1);
            }
            if (i % 8 != 0){
                pawn_attack[1][i] = bit_set_to(pawn_attack[1][i], i - 9, 1);
            }
            if (i % 8 != 7){
                pawn_attack[1][i] = bit_set_to(pawn_attack[1][i], i - 7, 1);
            }
        }
    }

}

void Board::generate_knight_table(){
    std::vector<int> moves = {10, 17, 15, 6, -10, -17, -15, -6};
    std::vector<u64> banned_files = {G_FILE | H_FILE, H_FILE, A_FILE, A_FILE | B_FILE, A_FILE | B_FILE, A_FILE, H_FILE, G_FILE | H_FILE};

    for (int i = 0; i < 64; ++i){
        knight_table[i] = 0;
        for (int k = 0; k < 8; ++k){
            if (0 <= i + moves[k] && i + moves[k] <= 63 && !(1 & (banned_files[k] >> i))){
                knight_table[i] = bit_set_to(knight_table[i], i + moves[k], 1);
            }
        }
    }
}

void Board::generate_king_table(){
    std::vector<int> moves = {1, 9, 8, 7, -1, -9, -8, -7};
    std::vector<u64> banned_files = {H_FILE, H_FILE, (u64)0, A_FILE, A_FILE, A_FILE, (u64)0, H_FILE};

    for (int i = 0; i < 64; ++i){
        king_table[i] = 0;
        for (int k = 0; k < 8; ++k){
            if (0 <= i + moves[k] && i + moves[k] <= 63 && !(1 & (banned_files[k] >> i))){
                king_table[i] = bit_set_to(king_table[i], i + moves[k], 1);
            }
        }
    }
}

bool Board::check_pawn(piece_color pc, int pre_sq, int post_sq){
    u64 push = pawn_push[pc][pre_sq] & ~(f_bb);
    
    //make sure double push doesn't jump over smth
    if (pre_sq >> 3 == 2 && pc == WHITE && !(push & ((u64)1 << (pre_sq + 8)))){
        push = 0;
    }
    else if (pre_sq >> 3 == 6 && pc == BLACK && !(push & ((u64)1 << (pre_sq - 8)))){
        push = 0;
    }
    u64 attack = pawn_attack[pc][pre_sq] & piece_bbs[pc ^ 1][ALL];

    u64 pawn_possible_moves = push | attack;

    if (!(pawn_possible_moves & ((u64)1 << post_sq))){
        std::cout << "not a legal move" << std::endl;
        return false;
    }
    else{
        return true;
    }
    
}


bool Board::check_knight(piece_color pc, int pre_sq, int post_sq){
    u64 knight_possible_moves = knight_table[pre_sq] & ~(piece_bbs[pc][ALL]);
    if (!(knight_possible_moves & ((u64)1 << post_sq))){
        std::cout << "not a legal move" << std::endl;
        return false;
    }
    else{
        return true;
    }
}

bool Board::check_bishop(piece_color pc, int pre_sq, int post_sq){
    u64 bishop_possible_moves = bishop_moves(f_bb, pre_sq) & ~(piece_bbs[pc][ALL]);

    if (!(bishop_possible_moves & ((u64)1 << post_sq))){
        std::cout << "not a legal move" << std::endl;
        return false;
    }
    else{
        return true;
    }


}

bool Board::check_rook(piece_color pc, int pre_sq, int post_sq){
    u64 rook_possible_moves = rook_moves(f_bb, pre_sq) & ~(piece_bbs[pc][ALL]);
    if (!(rook_possible_moves & ((u64)1 << post_sq))){
        std::cout << "not a legal move" << std::endl;
        return false;
    }
    else{
        return true;
    }
}

bool Board::check_queen(piece_color pc, int pre_sq, int post_sq){
    u64 queen_possible_moves = queen_moves(f_bb, pre_sq) & ~(piece_bbs[pc][ALL]);
    if (!(queen_possible_moves & ((u64)1 << post_sq))){
        std::cout << "not a legal move" << std::endl;
        return false;
    }
    else{
        return true;
    }
}

bool Board::check_king(piece_color pc, int pre_sq, int post_sq){

    bool can_castle = false;

    if (castling_rights[0] && pc == WHITE && pre_sq + 2 == post_sq){ //white kingside 
        if (get_bit(piece_bbs[WHITE][ROOK], H1) && !get_bit(f_bb, F1) && !get_bit(f_bb, G1) && !is_square_attacked(BLACK, F1) && !is_square_attacked(BLACK, G1)){
            can_castle = true;
        }
    }
    else if (castling_rights[1] && pc == WHITE && pre_sq - 2 == post_sq){ //white queenside
        if (get_bit(piece_bbs[WHITE][ROOK], A1) &&!get_bit(f_bb, B1) && !get_bit(f_bb, C1) && !get_bit(f_bb, D1) && 
            !is_square_attacked(BLACK, C1) && !is_square_attacked(BLACK, D1)){
            can_castle = true;
        }
    } 
    else if (castling_rights[2] && pc == BLACK && pre_sq + 2 == post_sq){ //black kingside
        if (get_bit(piece_bbs[BLACK][ROOK], H8) &&!get_bit(f_bb, F8) && !get_bit(f_bb, G8) && !is_square_attacked(WHITE, F8) && !is_square_attacked(WHITE, G8)){
            can_castle = true;
        }
    }
    else if (castling_rights[3] && pc == BLACK && pre_sq - 2 == post_sq){ //black queenside 
        if (get_bit(piece_bbs[BLACK][ROOK], A8) &&!get_bit(f_bb, B8) && !get_bit(f_bb, C8) && !get_bit(f_bb, D8) && 
            !is_square_attacked(WHITE, C8) && !is_square_attacked(WHITE, D8)){
            can_castle = true;
        }
    }


    u64 king_possible_moves = king_table[pre_sq] & ~(piece_bbs[pc][ALL]);
    if (!(king_possible_moves & ((u64)1 << post_sq)) && !can_castle){
        std::cout << "not a legal move" << std::endl;
        return false;
    }
    else{
        return true;
    }
}

bool Board::is_square_attacked(piece_color pc, int square){
    u64 pawns, knights, kings, bishops, rooks, queens;
    pawns = (pawn_attack[pc ^ 1][square] & piece_bbs[pc][PAWN]);
    knights = knight_table[square] & piece_bbs[pc][KNIGHT];
    kings = king_table[square] & piece_bbs[pc][KING];
    rooks = rook_moves(f_bb, square) & piece_bbs[pc][ROOK];
    bishops = bishop_moves(f_bb, square) & piece_bbs[pc][BISHOP];
    queens = queen_moves(f_bb, square) & piece_bbs[pc][QUEEN];

    u64 attacks = (pawns | knights | kings | rooks | bishops | queens);

    std::cout << (squares_RF)(square) << std::endl;
    print_square(attacks);

    return attacks != (u64)0;

}

void Board::update_bitboard(piece_color pc, piece_type pt, int pre_sq, int post_sq){
    piece_bbs[pc][pt] = bit_set_to(piece_bbs[pc][pt], pre_sq, 0);
    piece_bbs[pc][pt] = bit_set_to(piece_bbs[pc][pt], post_sq, 1);
    piece_bbs[pc][ALL] = bit_set_to(piece_bbs[pc][ALL], pre_sq, 0);
    piece_bbs[pc][ALL] = bit_set_to(piece_bbs[pc][ALL], post_sq, 1);
    f_bb= bit_set_to(f_bb, pre_sq, 0);
    f_bb = bit_set_to(f_bb, post_sq, 1);
}

void Board::move_piece(piece_color pc, piece_type pt, int pre_sq, int post_sq){

    update_bitboard(pc, pt, pre_sq, post_sq);

    // to create the move thing 
    piece_type capture_type, promoted_type = ALL;
    bool castle, captured, promoted;
    int change_castling_rights;

    //capturing a piece
    if (piece_bbs[pc ^ 1][ALL] & ((u64)1 << post_sq)){
        piece_bbs[pc ^ 1][ALL] = bit_set_to(piece_bbs[pc ^ 1][ALL], post_sq, 0);
        std::pair<piece_color, piece_type> piece = sc_board[post_sq];
        piece_bbs[piece.first][piece.second] = bit_set_to(piece_bbs[piece.first][piece.second], post_sq, 0);
        captured = true;
        capture_type = piece.second;
    }
    //castle
    if (pt == KING && (pre_sq + 2 == post_sq || pre_sq - 2 == post_sq)){
        int rook_prev = 0, rook_post = 0;
        if (pc == WHITE && pre_sq + 2 == post_sq){
            rook_prev = 7, rook_post = 5;
        }
        else if (pc == WHITE && pre_sq - 2 == post_sq){
            rook_prev = 0, rook_post = 3;
        } 
        else if (pc == BLACK && pre_sq + 2 == post_sq){
            rook_prev = 63, rook_post = 61;
        } 
        else if (pc == BLACK && pre_sq - 2 == post_sq){
            rook_prev = 56, rook_post = 59;
        }
        update_bitboard(pc, ROOK, rook_prev, rook_post);
        sc_board[rook_prev].first = NONE;
        sc_board[rook_post] = {pc, ROOK};
        castle = true;
    }
    //castling rights
    if (pt == KING){
        castling_rights[pc * 2] = false;
        castling_rights[pc * 2 + 1] = false;
        change_castling_rights = 3 << (pc * 2);
    }
    else if (pt == ROOK){
        int file = pre_sq % 8;
        if (file == 0){
            castling_rights[pc * 2 + 1] = false;
            change_castling_rights = 2 << (pc * 2); 
        }
        else if (file == 7){
            castling_rights[pc * 2] = false;
            change_castling_rights = 1 << (pc * 2);
        }
    }
    //promotion - auto promote to queen for now
    if (pt == PAWN && pc == WHITE && (pre_sq >> 3) == 6){
        piece_bbs[WHITE][PAWN] = bit_set_to(piece_bbs[WHITE][PAWN], post_sq, 0);
        piece_bbs[WHITE][QUEEN] = bit_set_to(piece_bbs[WHITE][QUEEN], post_sq, 1);
        pt = QUEEN;
        promoted_type = QUEEN;
        promoted = true;
    }
    else if (pt == PAWN && pc == BLACK && (pre_sq >> 3) == 1){
        piece_bbs[BLACK][PAWN] = bit_set_to(piece_bbs[BLACK][PAWN], post_sq, 0);
        piece_bbs[BLACK][QUEEN] = bit_set_to(piece_bbs[BLACK][QUEEN], post_sq, 1);
        pt = QUEEN;
        promoted_type = QUEEN;
        promoted = true;
    }

    sc_board[pre_sq].first = NONE;
    sc_board[post_sq] = {pc, pt};
    white_turn = !white_turn;
    move_list.push_back(Move(pre_sq, post_sq, promoted_type, capture_type, change_castling_rights, castle, captured, promoted, pc));
    
}

void Board::update(std::string move){
    int pre_sq = 8 * ((int)(move[1] - '1')) + (move[0] - 'a'), post_sq = 8 * ((int)(move[3] - '1')) + (move[2] - 'a');
    std::pair<piece_color, piece_type> piece = sc_board[pre_sq];

    if (piece.first == NONE){
        std::cout << "no piece selected" << std::endl;
        return;
    }
    else if ((piece.first == WHITE) == !white_turn){
        std::cout << "it is " << ( white_turn ? "white" : "black" ) << "'s move" << std::endl;
        return;
    }

    std::vector<std::string> map_color = {"WHITE", "BLACK"};
    std::vector<std::string> map_pieces = {"PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING"};
    std::cout << map_color[piece.first] << " " << map_pieces[piece.second] << " " << move << " " << pre_sq << " " << post_sq << std::endl;


    switch (piece.second){
        case PAWN:
            if (!check_pawn(piece.first, pre_sq, post_sq)){
                break;
            }
            move_piece(piece.first, piece.second, pre_sq, post_sq);
            break;
        case KNIGHT:
            if (!check_knight(piece.first, pre_sq, post_sq)){
                break;
            }
            move_piece(piece.first, piece.second, pre_sq, post_sq);
            break;
        case BISHOP:
            if (!check_bishop(piece.first, pre_sq, post_sq)){
                break;
            }
            move_piece(piece.first, piece.second, pre_sq, post_sq);
            break;
        case ROOK:
            if (!check_rook(piece.first, pre_sq, post_sq)){
                break;
            }
            move_piece(piece.first, piece.second, pre_sq, post_sq);
            break;
        case QUEEN:
            if (!check_queen(piece.first, pre_sq, post_sq)){
                break;
            }
            move_piece(piece.first, piece.second, pre_sq, post_sq);
            break;
        case KING:
            if (!check_king(piece.first, pre_sq, post_sq)){
                break;
            }
            move_piece(piece.first, piece.second, pre_sq, post_sq);
            break;
        default:
            break;
    }
}

void Board::make_move(Move move){
    move_piece(sc_board[move.get_pre_sq()].first, sc_board[move.get_pre_sq()].second, move.get_pre_sq(), move.get_post_sq());
}

void Board::unmake_move(Move move){
    int pre_sq = move.get_pre_sq(), post_sq = move.get_post_sq();
    piece_color pc = sc_board[post_sq].first;
    piece_type pt = sc_board[post_sq].second; 

    bool castle = move.check_castle(), capture = move.check_capture(), promotion = move.check_promoted();
    piece_type p_type = (piece_type)move.get_promoted(), c_type = (piece_type)move.get_captured();
    int new_castling_rights = move.get_castling();

    update_bitboard(pc, pt, post_sq, pre_sq);


    //captured a piece
    if (capture){
        piece_bbs[pc ^ 1][ALL] = bit_set_to(piece_bbs[pc ^ 1][ALL], post_sq, 1);
        piece_bbs[pc ^ 1][c_type] = bit_set_to(piece_bbs[pc][c_type], post_sq, 1);
        sc_board[post_sq] = {(piece_color)(pc ^ 1), c_type};
    }
    //castle
    if (castle){
        if (pc == WHITE && pre_sq + 2 == post_sq){
            update_bitboard(pc, ROOK, 5, 7);
        }
        else if (pc == WHITE && pre_sq - 2 == post_sq){
            update_bitboard(pc, ROOK, 3, 0);
        } 
        else if (pc == BLACK && pre_sq + 2 == post_sq){
            update_bitboard(pc, ROOK, 61, 63);
        } 
        else if (pc == BLACK && pre_sq - 2 == post_sq){
            update_bitboard(pc, ROOK, 59, 56);
        }
        castle = true;
    }
    //castling rights 
    for (int i = 0; i < 3; ++i){
        if ((1 << i) & new_castling_rights){
            castling_rights[i] = true;
        }
    }
    //promotion
    if (promotion){
        piece_bbs[pc][PAWN] = bit_set_to(piece_bbs[pc][PAWN], pre_sq, 1);
        piece_bbs[pc][p_type] = bit_set_to(piece_bbs[pc][p_type], pre_sq, 1);
        pt = PAWN;
    }

    sc_board[pre_sq] = {pc, pt};
    white_turn = !white_turn;
    move_list.pop_back();
}

// std::vector<Move> Board::generate_legal_moves(std::vector<Move> &moves){

//     /*
//     logic:
//     king moves (get rid of king and then and all the attack tables)

//     if double check (or more), only king moves

//     if single check 
//         move king out of check (in king moves)
//         make capture mask + add psuh mask if sliding
//             use mask to filter valid moves 
    
//     just hard code en passant and castling 

//     */

// }


// std::vector<Move> Board::generate_pseudolegal_moves(std::vector<Move> &moves){

//     std::vector<Move> pseudolegal_moves;
    
// }