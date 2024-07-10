#include "board.hpp"
#include <SFML/Graphics.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <bitset>
#include <map>

Board::Board(){
    generate_knight_table();
    init_magics();
    setToFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");


    u64 blockers = 0xFBAC1931AD21BC91;
    print_square(blockers);
    std::cout << std::endl;
    u64 moves = rook_moves(blockers, 36);
    print_square(moves);
}

Board::Board(std::string fen){
    setToFen(fen);
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




void Board::generate_knight_table(){
    std::vector<int> moves = {10, 17, 15, 6, -10, -17, -15, -6};
    std::vector<u64> not_allowed_files = {G_FILE | H_FILE, H_FILE, A_FILE, A_FILE | B_FILE, A_FILE | B_FILE, A_FILE, H_FILE, G_FILE | H_FILE};

    for (int i = 0; i < 64; ++i){
        knight_table[i] = 0;
        for (int k = 0; k < 8; ++k){
            if (0 <= i + moves[k] && i + moves[k] <= 63 && !(1 & (not_allowed_files[k] >> i))){
                knight_table[i] = bit_set_to(knight_table[i], i + moves[k], 1);
            }
        }

    }
}


bool Board::check_knight(piece_color pc, int pre_sq, int post_sq){
    u64 knight_possible_moves = knight_table[pre_sq] & ~(piece_bbs[pc][ALL]);
    if (!(knight_possible_moves & ((u64)1 << post_sq))){
        std::cout << "Not a legal move" << std::endl;
        return false;
    }
    else{
        return true;
    }
}

void Board::move_piece(piece_color pc, piece_type pt, int pre_sq, int post_sq, std::string move){
    piece_bbs[pc][pt] = bit_set_to(piece_bbs[pc][pt], pre_sq, 0);
    piece_bbs[pc][pt] = bit_set_to(piece_bbs[pc][pt], post_sq, 1);
    piece_bbs[pc][ALL] = bit_set_to(piece_bbs[pc][ALL], pre_sq, 0);
    piece_bbs[pc][ALL] = bit_set_to(piece_bbs[pc][ALL], post_sq, 1);
    f_bb= bit_set_to(f_bb, pre_sq, 0);
    f_bb = bit_set_to(f_bb, post_sq, 1);

    sc_board[pre_sq].first = NONE;
    sc_board[post_sq] = {pc, pt};

    white_turn = !white_turn;

    move_list.push_back(move);
}

void Board::update(std::string move){
    int pre_sq = 8 * ((int)(move[1] - '1')) + (move[0] - 'a'), post_sq = 8 * ((int)(move[3] - '1')) + (move[2] - 'a');
    std::pair<piece_color, piece_type> piece = sc_board[pre_sq];

    if ((piece.first == WHITE) == !white_turn){
        std::cout << "it is " << ( white_turn ? "white" : "black" ) << "'s move" << std::endl;
        return;
    }

    // std::cout << piece.first << " " << piece.second << std::endl;

    switch (piece.second){
        case PAWN:
            break;
        case KNIGHT:
            if (!check_knight(piece.first, pre_sq, post_sq)){
                break;
            }
            move_piece(piece.first, piece.second, pre_sq, post_sq, move);
        case BISHOP:
            break;
        case ROOK:
            break;
        case QUEEN:
            break;
        case KING:
            break;
        default:
            break;
    }

    std::cout << move << " " << pre_sq << " " << post_sq << std::endl;
}
