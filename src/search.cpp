#include "board.hpp"
#include "defs.hpp"
#include "zobrist.hpp"
#include "magic.hpp"
#include "book.hpp"

#include <vector>
#include <bitset>
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>


const int n_inf = -1000000;

const int piece_value_mid[6] = { 82, 337, 365, 477, 1025,  0};
const int piece_value_end[6] = { 94, 281, 297, 512,  936,  0};
const int perspective[2] = {1, -1};

int pawn_pst_mid[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

int pawn_pst_end[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

int knight_pst_mid[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

int knight_pst_end[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

int bishop_pst_mid[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

int bishop_pst_end[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

int rook_pst_mid[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

int rook_pst_end[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

int queen_pst_mid[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

int queen_pst_end[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

int king_pst_mid[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

int king_pst_end[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

const int * piece_pst_mid[6] = {pawn_pst_mid, knight_pst_mid, bishop_pst_mid, rook_pst_mid, queen_pst_mid, king_pst_mid};
const int * piece_pst_end[6] = {pawn_pst_end, knight_pst_end, bishop_pst_end, rook_pst_end, queen_pst_end, king_pst_end};
const int piece_phase[6] = {0, 1, 1, 2, 4, 0};

int Board::evaluate(){
    int ret_mid = 0, ret_end = 0;
    int phase = 0; 

    u64 white_bb = piece_bbs[WHITE][ALL];
    while (white_bb > 0){
        int sq = lsb(white_bb);
        white_bb = bit_set_to(white_bb, sq, 0);
        ret_mid += (piece_value_mid[sc_board[sq].second] + piece_pst_mid[sc_board[sq].second][sq ^ 56]);
        ret_end += (piece_value_end[sc_board[sq].second] + piece_pst_end[sc_board[sq].second][sq ^ 56]);
        phase += piece_phase[sc_board[sq].second];
    }

    u64 black_bb = piece_bbs[BLACK][ALL];
    while (black_bb > 0){
        int sq = lsb(black_bb);
        black_bb = bit_set_to(black_bb, sq, 0);
        ret_mid -= (piece_value_mid[sc_board[sq].second] + piece_pst_mid[sc_board[sq].second][sq]);
        ret_end -= (piece_value_end[sc_board[sq].second] + piece_pst_end[sc_board[sq].second][sq]);
        phase += piece_phase[sc_board[sq].second];
    }


    int wking_pos = lsb(piece_bbs[WHITE][KING]), bking_pos = lsb(piece_bbs[BLACK][KING]);
    int dist = std::abs((wking_pos >> 3) - (bking_pos >> 3)) + std::abs((wking_pos % 8) - (bking_pos % 8));

    ret_mid *= perspective[color_turn];
    ret_end *= perspective[color_turn];

    if (ret_mid * phase + ret_end * (24 - phase) > 0){ //winning for the color to move, so shld bring king closer
        ret_end += (14 - dist) ;
    }
    else {
        ret_end -= (14 - dist) ;
    }

    return (ret_mid * phase + ret_end * (24 - phase))/24;
}

void Board::sort_moves(std::vector<Move> &moves){
    std::vector<std::pair<int, Move>> move_scores(moves.size());

    bool has_move = false;
    Move prev_move;
    TTentry tt_entry = ztable[(zhash_pos) >> (64 - zobrist_size)];
    if (tt_entry.z_key == zhash_pos){
        has_move = true;
        prev_move = tt_entry.best_move;
    }

    int idx = 0;
    for (Move &move : moves){

        int move_score = 0;

        piece_type pt = sc_board[move.get_pre_sq()].second;

        if (move.check_capture()){
            move_score += 10 * piece_value_mid[move.get_captured()] - piece_value_mid[pt];
        }

        // square attacked by pawn
        if (piece_bbs[color_turn ^ 1][PAWN] & pawn_attack[color_turn][move.get_post_sq()]){
            move_score -= piece_value_mid[pt];
        }
        // previosuly stored move
        if (has_move && prev_move.move == move.move){
            move_score += 10000;
        }
        // std::cout << squares_RF_str[move.get_pre_sq()] << squares_RF_str[move.get_post_sq()] << " eval: " << move_score << std::endl;
        move_scores[idx++] = std::make_pair(move_score, move);
    }
    std::sort(move_scores.begin(), move_scores.end(), std::greater<>());

    for (size_t i = 0; i < moves.size(); ++i){
        moves[i] = move_scores[i].second;
    }

}

int Board::capture_search(int alpha, int beta){

    if (std::chrono::system_clock::now() > end_search_time){
        return 0;
    } 

    nodes++;

    int eval = evaluate();
    if (eval >= beta){
        return beta;
    }
    alpha = std::max(alpha, eval);


    std::vector<Move> moves = generate_moves(true);
    sort_moves(moves); // to test


    for (Move &move : moves){
        if (move.get_captured()){
            make_move(move);
            int eval = -1 * capture_search(-beta, -alpha);
            unmake_move(move);

            if (eval >= beta){
                return beta;
            }
            alpha = std::max(alpha, eval);
            
        }
    }

    return alpha;


}

int Board::search(int depth, int alpha, int beta, bool compute_move){

    //3 fold 
    int amnt = 0;
    for (int i = ply - 1; i >= std::max(0, ply - 8); i--){
        if (zhash_moves[i] == zhash_pos){
            amnt++;
        }
        if (amnt >= 3){
            return -10;
        }
    }

    //is check, add one more 
    if (inCheck((piece_color)color_turn)){
        depth++;
    }

    //q search
    if (depth == 0){
        return capture_search(alpha, beta); // fix this 
        // return evaluate();
    }


    //check transposition table
    int original_alpha = alpha;
    u64 ztable_index = zhash_pos >> (64 - zobrist_size);
    TTentry tt_entry = ztable[ztable_index];
    if (tt_entry.z_key == zhash_pos && tt_entry.depth >= depth){
        if (compute_move){
            best_move = tt_entry.best_move;
        }
        if (tt_entry.eval_type == 0){
            return tt_entry.eval;
        }
        else if (tt_entry.eval_type == 1){
            alpha = std::max(alpha, tt_entry.eval);
        }
        else if (tt_entry.eval_type == 2){
            beta = std::min(beta, tt_entry.eval);
        }
        if (alpha >= beta){
            return tt_entry.eval;
        }

    }



    std::vector<Move> moves = generate_moves();
    sort_moves(moves); // move ordering 
    

    //checkmate / draw
    if (moves.size() == 0){
        if (compute_move){
            game_done = true;
        }
        if (inCheck((piece_color)color_turn)){
            return n_inf;
        }
        else{
            return 0;
        }
    }


    int best_eval = n_inf - 1;
    Move temp_best_move = Move();

    bool first_move = true;
    for (Move &move : moves){
        make_move(move);
        int eval;
        if (first_move){
            eval = -1 * search(depth - 1, -beta, -alpha,  false);
            first_move = false;
        }
        else{
            eval = -1 * search(depth - 1, -alpha - 1, -alpha, false);
            if (eval > alpha && beta - alpha > 1){
                eval = -1 * search(depth - 1, -beta, -alpha, false);
            }
        }
        unmake_move(move);
        if (eval > best_eval){
            temp_best_move = move;    
            best_eval = eval;
        }

        alpha = std::max(alpha, eval);
        if (alpha >= beta){
            break;
        }
        if (std::chrono::system_clock::now() > end_search_time){
            return 0;
        } 
    }

    // if completed the search, can assign best move 
    if (std::chrono::system_clock::now() > end_search_time){
        return 0;
    } 
    else{
        //depth replacement scheme
        if (tt_entry.depth <= depth){
            ztable[ztable_index].eval = best_eval;
            if (best_eval <= original_alpha){
                ztable[ztable_index].eval_type = 2;
            }
            else if (best_eval >= beta){
                ztable[ztable_index].eval_type = 1;
            }
            else{
                ztable[ztable_index].eval_type = 0;
            }
            ztable[ztable_index].depth = depth;
            ztable[ztable_index].best_move = temp_best_move;
            ztable[ztable_index].z_key = zhash_pos;
        }
        if (compute_move){
            best_move = temp_best_move;
            // std::cout << squares_RF_str[best_move.get_pre_sq()] << squares_RF_str[best_move.get_post_sq()] << "\n";
        }
        return best_eval;
    }
}

int Board::compute_time(){
    if (time_control_type == 0){
        return time[color_turn] / 20;
    }
    else if (time_control_type == 1){
        return time[color_turn] / (movestogo+2);
    }
    else{
        return (time[color_turn] + time_inc[color_turn]) / 20;
    }
}


void Board::engine_move(int uci_depth, int uci_time){
    std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
    
    // set_engine_color(color_turn);
    int pos_eval = 0;
    //opening book move
    if (book_moves.find(zhash_pos) != book_moves.end()){
        std::vector<Move> possible_moves = generate_moves();

        for (Move &move : possible_moves){
            if (move.get_pre_sq() == book_moves[zhash_pos].pre_sq && move.get_post_sq() == book_moves[zhash_pos].post_sq){
                best_move = move;
                std::cout << "info opening book used\n";
                break;   
            }
        }
        
    }
    else{
        if (uci_depth != -1){
            search(uci_depth, n_inf * 2, n_inf * -2, true);
        }
        else{
            if (uci_time == -1){
                uci_time = compute_time();
            }
            end_search_time = std::chrono::system_clock::now() + std::chrono::milliseconds(uci_time);
            int depth = 0;
            while (std::chrono::system_clock::now() < end_search_time){
                depth++;
                int eval = search(depth, n_inf * 2, n_inf * -2, true);
                // std::cout << "info depth: " << depth << " eval: " << eval << std::endl;
                if (eval == -1 * n_inf){ //has checkmate
                    // std::cout << "info has checkmate" << std::endl;
                    break;
                }
                if (std::chrono::system_clock::now() < end_search_time){
                    pos_eval = eval;
                }
            }
            
            std::cout << "info (used) depth: " << depth - 1 << " nodes: " << nodes << " nps: " << nodes/uci_time << "\n";
        }
    }

    make_move(best_move);

    if (best_move.check_promoted()){
        std::cout << "bestmove " << squares_RF_str[best_move.get_pre_sq()] << squares_RF_str[best_move.get_post_sq()] << uci_promo[best_move.get_promoted()] << "\n";
    }
    else{
        std::cout << "bestmove " << squares_RF_str[best_move.get_pre_sq()] << squares_RF_str[best_move.get_post_sq()] << "\n";
    }
    std::cout << "info eval: " << pos_eval << "\n";
    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    time[engine_color] -= std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
    // std::cout << "info time left for engine: " << time[engine_color] << "[ms]" << std::endl;


}