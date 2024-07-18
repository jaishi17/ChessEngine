#include "board.hpp"
#include "defs.hpp"

#include <vector>
#include <bitset>
#include <iostream>
#include <chrono>

const int n_inf = -1000000;
const int piece_value[6] = {100, 300, 300, 500, 900, 10000};
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
/*
TODO:
hook up to uci 

search:
    move order (capture, promotion, etc)
    quiesnce search
    transposition tables
create a engine/game class 

*/

int Board::evaluate(){
    int mid_eval = 0, end_eval = 0;
    int phase = 0; 

    for (int i = 0; i < 64; ++i){
        if (sc_board[i].first == WHITE){
            mid_eval += (piece_value[sc_board[i].second] + piece_pst_mid[sc_board[i].second][i]);
            end_eval += (piece_value[sc_board[i].second] + piece_pst_end[sc_board[i].second][i]);
            phase += piece_phase[sc_board[i].second];
        }
        else if (sc_board[i].first == BLACK){
            int sq = i ^ 56;
            mid_eval -= (piece_value[sc_board[i].second] + piece_pst_mid[sc_board[i].second][sq]);
            end_eval -= (piece_value[sc_board[i].second] + piece_pst_end[sc_board[i].second][sq]); 
            phase += piece_phase[sc_board[i].second];
       }
    }

    return perspective[color_turn] * (mid_eval * phase + end_eval * (24 - phase))/24;
}

int Board::capture_search(int alpha, int beta){
    int eval = evaluate();
    if (eval >= beta){
            return beta;
        }
    alpha = std::max(alpha, eval);

    std::vector<Move> moves = generate_moves();

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
    if (depth == 0){
        // return capture_search(alpha, beta);
        return evaluate();
    }

    std::vector<Move> moves = generate_moves();
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

    for (Move &move : moves){
        make_move(move);
        int eval = -1 * search(depth - 1, -beta, -alpha,  false);
        unmake_move(move);

        if (eval > best_eval){
            if (compute_move){
                temp_best_move = move;
            }
            best_eval = eval;
        }

        // if (depth == 4 && compute_move){
        //     std::cout << "bestmove " << squares_RF_str[move.get_pre_sq()] << squares_RF_str[move.get_post_sq()]  << ": " << eval << std::endl;
        // }
     

        if (eval >= beta){
            return beta;
        }
        alpha = std::max(alpha, eval);

        if (std::chrono::system_clock::now() > end_search_time){
            return 0;
        } 
    }

    // if completed the search, can assign best move 
    if (std::chrono::system_clock::now() > end_search_time){
        return 0;
    } 
    else{
        if (compute_move){
            best_move = temp_best_move;
        }
        return best_eval;
    }
}

int Board::compute_time(){
    if (time_control_type == 0){
        return time[engine_color] / 20;
    }
    else{
        return time[engine_color] / (movestogo+2);
    }
}


void Board::engine_move(int uci_depth, int uci_time){
    std::chrono::steady_clock::time_point begin_time = std::chrono::steady_clock::now();
    
    // set_engine_color(color_turn);

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
            if (eval == n_inf){ //has checkmate
                break;
            }

        }

    }

    if (get_game_done()){
        print_result();
        return;
    }


    make_move(best_move);



    if (best_move.check_promoted()){
        std::cout << "bestmove " << squares_RF_str[best_move.get_pre_sq()] << squares_RF_str[best_move.get_post_sq()] << uci_promo[best_move.get_promoted()] << std::endl;
    }
    else{
        std::cout << "bestmove " << squares_RF_str[best_move.get_pre_sq()] << squares_RF_str[best_move.get_post_sq()] << std::endl;
    }
    std::cout << "info eval: " << evaluate() * perspective[color_turn] << std::endl;

    std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
    time[engine_color] -= std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count();
    // std::cout << "time left for engine: " << time[engine_color] << "[ms]" << std::endl;

}