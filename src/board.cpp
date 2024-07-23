#include "board.hpp"
#include "zobrist.hpp"
#include "magic.hpp"
#include <SFML/Graphics.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>
#include <map>
#include <chrono>

Move::Move(){
    move = 0;
}

Move::Move(int pre_sq, int post_sq, piece_type p_type, piece_type c_type, int castling_rights, bool castle, bool capture, bool promotion, bool en_passant, int prev_epsq){
    move = 0;
    move = pre_sq | (post_sq << 6) | ((int)p_type << 12) | ((int)c_type << 15) | (castling_rights << 18) | ((int)castle << 22) | ((int)capture << 23) | ((int)promotion << 24) | ((int)en_passant << 25)  | ((prev_epsq & 0x3f) << 26);

}

Board::Board(){
    setToFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ");
}

Board::Board(std::string fen){
    setToFen(fen);

    
}



std::vector<std::pair<piece_color, piece_type>> const Board::getSCBoard() const {
    return sc_board;
}

piece_color Board::get_color(){
    return color_turn ? BLACK : WHITE;
}

bool Board::get_game_done(){
    return game_done;
}

void Board::clearBitboards(){
    for (int i = 0; i < color_count; ++i){
        for (int j = 0; j < piece_count + 1; ++j){
            piece_bbs[i][j] = 0;
        }
    }
    f_bb = 0;
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
    std::map<char, std::pair<piece_color, piece_type>> fen_map = {{'p', {BLACK, PAWN}}, {'n', {BLACK, KNIGHT}}, {'b', {BLACK, BISHOP}}, 
                                                              {'r', {BLACK, ROOK}}, {'q', {BLACK, QUEEN}},{'k', {BLACK, KING}}, 
                                                              {'P', {WHITE, PAWN}}, {'N', {WHITE, KNIGHT}}, {'B', {WHITE, BISHOP}}, 
                                                              {'R', {WHITE, ROOK}}, {'Q', {WHITE, QUEEN}}, {'K', {WHITE, KING}}};
    while (fen[fen_idx] != ' '){
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
    fen_idx++;
    color_turn = (fen[fen_idx] == 'w' ? 0 : 1); 
    castling_rights[0] = false;
    castling_rights[1] = false;
    castling_rights[2] = false;
    castling_rights[3] = false;
    en_passant_sq = 0;

    fen_idx += 2;
    while(fen[fen_idx] != ' '){

        switch (fen[fen_idx]){
            case 'K':
                castling_rights[0] = true;
                break;
            case 'Q':
                castling_rights[1] = true;
                break;
            case 'k':
                castling_rights[2] = true;
                break;
            case 'q':
                castling_rights[3] = true;
                break;
        }
        fen_idx++;
    }

    en_passant_sq = 0;
    fen_idx++;
    if ('a' <= fen[fen_idx] && fen[fen_idx] <= 'h'){
        en_passant_sq = (int)(fen[fen_idx] - 'a') + 8 * ((int)(fen[fen_idx + 1] - '1'));
    }

    for (int i = 0; i < 64; ++i){
        if (sc_board[i].first != NONE){
            zhash_pos ^= zobrist_keys[64 * ((sc_board[i].first ^ 1) + 2 * sc_board[i].second) + i];
        }
    }
    for (int i = 0; i < 4; ++i){
        if (castling_rights[i]){
            zhash_pos ^= zobrist_keys[768 + i];
        }
    }
    if (en_passant_sq != -1){
        if (en_passant_sq % 8 != 0 && ((get_bit(piece_bbs[color_turn][PAWN], en_passant_sq - 9) && color_turn == WHITE) || (get_bit(piece_bbs[color_turn][PAWN], en_passant_sq + 7) && color_turn == BLACK))){
            zhash_pos ^= zobrist_keys[772 + (en_passant_sq % 8)];
        }
        else if (en_passant_sq % 8 != 7 && ((get_bit(piece_bbs[color_turn][PAWN], en_passant_sq - 7) && color_turn == WHITE) || (get_bit(piece_bbs[color_turn][PAWN], en_passant_sq + 9) && color_turn == BLACK))){
            zhash_pos ^= zobrist_keys[772 + (en_passant_sq % 8)];
        }
    }
    if (color_turn == 0){
        zhash_pos ^= zobrist_keys[780];
    }


}

void Board::print_result(){
    if (inCheck(WHITE)){
        std::cout << "checkmate for black" << std::endl; 
    }
    else if (inCheck(BLACK)){
        std::cout << "checkmate for white" << std::endl;
    }
    else{
        std::cout << "draw" << std::endl;
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


bool Board::check_pawn(piece_color pc, int pre_sq, int post_sq){
    u64 push = pawn_push[pc][pre_sq] & ~(f_bb);
    
    //make sure double push doesn't jump over smth
    if (pre_sq >> 3 == 1 && pc == WHITE && !(~f_bb & ((u64)1 << (pre_sq + 8)))){
        push = 0;
    }
    else if (pre_sq >> 3 == 6 && pc == BLACK && !(~f_bb & ((u64)1 << (pre_sq - 8)))){
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
        if (get_bit(piece_bbs[WHITE][ROOK], H1) && !get_bit(f_bb, F1) && !inCheck(WHITE) && !get_bit(f_bb, G1) && !is_square_attacked(BLACK, F1) && !is_square_attacked(BLACK, G1)){
            can_castle = true;
        }
    }
    else if (castling_rights[1] && pc == WHITE && pre_sq - 2 == post_sq){ //white queenside
        if (get_bit(piece_bbs[WHITE][ROOK], A1) &&!get_bit(f_bb, B1) && !get_bit(f_bb, C1) && !get_bit(f_bb, D1) && 
            !inCheck(WHITE) && !is_square_attacked(BLACK, C1) && !is_square_attacked(BLACK, D1)){
            can_castle = true;
        }
    } 
    else if (castling_rights[2] && pc == BLACK && pre_sq + 2 == post_sq){ //black kingside
        if (get_bit(piece_bbs[BLACK][ROOK], H8) &&!get_bit(f_bb, F8) && !inCheck(BLACK) && !get_bit(f_bb, G8) && !is_square_attacked(WHITE, F8) && !is_square_attacked(WHITE, G8)){
            can_castle = true;
        }
    }
    else if (castling_rights[3] && pc == BLACK && pre_sq - 2 == post_sq){ //black queenside 
        if (get_bit(piece_bbs[BLACK][ROOK], A8) &&!get_bit(f_bb, B8) && !get_bit(f_bb, C8) && !get_bit(f_bb, D8) && 
            !inCheck(BLACK) && !is_square_attacked(WHITE, C8) && !is_square_attacked(WHITE, D8)){
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

//pc is the attacking color
bool Board::is_square_attacked(piece_color pc, int square){
    u64 pawns, knights, kings, bishops, rooks, queens;
    pawns = (pawn_attack[pc ^ 1][square] & piece_bbs[pc][PAWN]);
    knights = knight_table[square] & piece_bbs[pc][KNIGHT];
    kings = king_table[square] & piece_bbs[pc][KING];

    u64 rook_sqs = rook_moves(f_bb, square), bishop_sqs = bishop_moves(f_bb, square);
    u64 queen_sqs = rook_sqs | bishop_sqs;

    rooks = rook_sqs & piece_bbs[pc][ROOK];
    bishops = bishop_sqs & piece_bbs[pc][BISHOP];
    queens = queen_sqs & piece_bbs[pc][QUEEN];

    u64 attacks = (pawns | knights | kings | rooks | bishops | queens);

    return attacks != (u64)0;
}

bool Board::inCheck(piece_color pc){
    int king_loc = lsb(piece_bbs[pc][KING]);
    return is_square_attacked((piece_color)(pc ^ 1), king_loc);
}

void Board::update_bitboard(piece_color pc, piece_type pt, int pre_sq, int post_sq){

    piece_bbs[pc][pt] = bit_set_to(piece_bbs[pc][pt], pre_sq, 0);
    piece_bbs[pc][pt] = bit_set_to(piece_bbs[pc][pt], post_sq, 1);
    piece_bbs[pc][ALL] = bit_set_to(piece_bbs[pc][ALL], pre_sq, 0);
    piece_bbs[pc][ALL] = bit_set_to(piece_bbs[pc][ALL], post_sq, 1);
    f_bb= bit_set_to(f_bb, pre_sq, 0);
    f_bb = bit_set_to(f_bb, post_sq, 1);
}

const int eval_color[2] = {-1, 1};
const int eval_flip[2] = {56, 0};

void Board::make_move(Move &move){   

    int pre_sq = move.get_pre_sq(), post_sq = move.get_post_sq();
    piece_color pc = sc_board[pre_sq].first;
    piece_type pt = sc_board[pre_sq].second; 

    bool castle = move.check_castle(), capture = move.check_capture(), promotion = move.check_promoted(), en_passant = move.check_en_passant();
    piece_type p_type = (piece_type)move.get_promoted(), c_type = (piece_type)move.get_captured();
    int new_castling_rights = move.get_castling();
    
    //----------------------
    update_bitboard(pc, pt, pre_sq, post_sq);
    zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * pt) + pre_sq];

    //----------------------
    if (en_passant_sq != 0){
        zhash_pos ^= zobrist_keys[772 + en_passant_sq % 8];
    }
    if (capture){
        u64 cap_sq = post_sq;
        if (en_passant){
            if (pc == WHITE){
                cap_sq = post_sq - 8;
            }
            else { 
                cap_sq = post_sq + 8;
            }
            f_bb = bit_set_to(f_bb, cap_sq, 0);
            sc_board[cap_sq].first = NONE;
        }        
        piece_bbs[pc ^ 1][ALL] = bit_set_to(piece_bbs[pc ^ 1][ALL], cap_sq, 0);
        piece_bbs[pc ^ 1][c_type] = bit_set_to(piece_bbs[pc ^ 1][c_type], cap_sq, 0);
        zhash_pos ^= zobrist_keys[64 * (((int)pc) + 2 * c_type) + cap_sq];
    }
    if (castle){
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
        zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * ROOK) + rook_prev];
        zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * ROOK) + rook_post];
    }
    if (promotion){
        piece_bbs[pc][PAWN] = bit_set_to(piece_bbs[pc][PAWN], post_sq, 0);
        piece_bbs[pc][p_type] = bit_set_to(piece_bbs[pc][p_type], post_sq, 1);
        pt = p_type;
    }
    //--------------
    for (int i = 0; i < 4; ++i){
        if (new_castling_rights & (1 << i)){
            castling_rights[i] = false;
            zhash_pos ^= zobrist_keys[768 + i];

        }
    }
    en_passant_sq = 0;
    if (post_sq == pre_sq + 16 && pt == PAWN && pc == WHITE){
        if ((post_sq % 8 != 0 && get_bit(piece_bbs[BLACK][PAWN], post_sq - 1)) || (post_sq % 8 != 7 && (get_bit(piece_bbs[BLACK][PAWN], post_sq + 1)))){
            en_passant_sq = pre_sq + 8; 
            zhash_pos ^= zobrist_keys[772 + (en_passant_sq % 8)];
        }
    } 
    else if (post_sq == pre_sq - 16 && pt == PAWN && pc == BLACK){
        if (((post_sq % 8) != 0 && get_bit(piece_bbs[WHITE][PAWN], post_sq -1)) || ((post_sq % 8 != 7) && (get_bit(piece_bbs[WHITE][PAWN], post_sq + 1)))){
            en_passant_sq = pre_sq - 8;
            zhash_pos ^= zobrist_keys[772 + (en_passant_sq % 8)];
        }
    }

    //-------------
    zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * pt) + post_sq];
    zhash_pos ^= zobrist_keys[780];
    sc_board[pre_sq].first = NONE;
    sc_board[post_sq] = {pc, pt};
    color_turn = !color_turn;
    ply++;   
    zhash_moves[ply] = zhash_pos;

}

bool Board::update(std::string move){
    int pre_sq = 8 * ((int)(move[1] - '1')) + (move[0] - 'a'), post_sq = 8 * ((int)(move[3] - '1')) + (move[2] - 'a');

    std::vector<Move> possible_moves = generate_moves();
    if (possible_moves.size() == 0){
        game_done = true;
        if (inCheck((piece_color)color_turn)){
            return false;
        }
        else{
            return false;
        }
    }

    for (Move &move : possible_moves){
        if (move.get_pre_sq() == pre_sq && move.get_post_sq() == post_sq){
            make_move(move);
            if (!uci_game){
                engine_move(-1, -1);
            }
            return true;
        }
    }

    return false;

}


void Board::unmake_move(Move &move){

    int pre_sq = move.get_pre_sq(), post_sq = move.get_post_sq();
    piece_color pc = sc_board[post_sq].first;
    piece_type pt = sc_board[post_sq].second; 

    bool castle = move.check_castle(), capture = move.check_capture(), promotion = move.check_promoted(), en_passant = move.check_en_passant();
    piece_type p_type = (piece_type)move.get_promoted(), c_type = (piece_type)move.get_captured();
    int new_castling_rights = move.get_castling();

    //---------------------
    update_bitboard(pc, pt, post_sq, pre_sq);
    sc_board[post_sq].first = NONE;
    zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * pt) + post_sq];

    //---------------
    if (capture){
        u64 cap_sq = post_sq;
        if (en_passant){
            if (pc == WHITE){
                cap_sq = post_sq - 8;
            }
            else{
                cap_sq = post_sq + 8;
            }
        }
        piece_bbs[pc ^ 1][ALL] = bit_set_to(piece_bbs[pc ^ 1][ALL], cap_sq, 1);
        piece_bbs[pc ^ 1][c_type] = bit_set_to(piece_bbs[pc ^ 1][c_type], cap_sq, 1);
        sc_board[cap_sq] = {(piece_color)(pc ^ 1), c_type};
        zhash_pos ^= zobrist_keys[64 * (((int)pc) + 2 * c_type) + cap_sq];
        f_bb = bit_set_to(f_bb, cap_sq, 1);
    }
    if (castle){
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
        update_bitboard(pc, ROOK, rook_post, rook_prev);
        sc_board[rook_prev] = {pc, ROOK};
        sc_board[rook_post] = {NONE, ROOK};
        zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * ROOK) + rook_prev];
        zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * ROOK) + rook_post];
        castle = true;
    }
    //---------------
    for (int i = 0; i < 4; ++i){
        if ((1 << i) & new_castling_rights){
            castling_rights[i] = true;
            zhash_pos ^= zobrist_keys[768 + i];
        }
    }
    //promotion
    if (promotion){
        piece_bbs[pc][PAWN] = bit_set_to(piece_bbs[pc][PAWN], pre_sq, 1);
        piece_bbs[pc][p_type] = bit_set_to(piece_bbs[pc][p_type], pre_sq, 0);
        pt = PAWN;
    }

    //-----------------
    zhash_pos ^= zobrist_keys[64 * (((int)pc ^ 1) + 2 * pt) + pre_sq];
    zhash_pos ^= zobrist_keys[780];

    sc_board[pre_sq] = {pc, pt};
    color_turn = !color_turn;

    
    if (en_passant_sq != 0){
        zhash_pos ^= zobrist_keys[772 + en_passant_sq % 8];
    }
    en_passant_sq = move.get_epsq();
    if (en_passant_sq != 0){
        zhash_pos ^= zobrist_keys[772 + en_passant_sq % 8];
    }
    ply--;

}


//engine stuff
void Board::set_time(int t, int color){
    time[color] = t;
}

void Board::set_engine_color(int color){
    time[color] = 60000; //engine gets 60 seconds to think;

    engine_color = color;


}

int Board::get_engine_color(){
    return engine_color;
} 

void Board::set_uci(bool uci){
    uci_game = uci;
}

void Board::set_movestogo(int moves){
    movestogo = moves;
}

void Board::set_time_inc(int inc, int color){
    time_inc[color] = inc;
    time_control_type = 2;
}
