#include "board.hpp"
#include "defs.hpp"
#include "magic.hpp"

#include <iostream>
#include <string>
#include <vector>

void Board::generate_pawn_moves(std::vector<Move> &moves){
    piece_color pc = (piece_color)get_color();
    u64 pawns = piece_bbs[pc][PAWN];

    while (pawns){
        int pre_sq = lsb(pawns);
        pawns = bit_set_to(pawns, pre_sq, 0);
        u64 pushes = pawn_push[(int)pc][pre_sq] & ~(f_bb);
        //make sure double push valid
        if (pre_sq >> 3 == 1 && pc == WHITE && !(~f_bb & ((u64)1 << (pre_sq + 8)))){
            pushes = 0;
        }
        else if (pre_sq >> 3 == 6 && pc == BLACK && !(~f_bb & ((u64)1 << (pre_sq - 8)))){
            pushes = 0;
        }
        while(pushes){
            int post_sq = lsb(pushes);
            pushes = bit_set_to(pushes, post_sq, 0);
            //promotion
            if ((pc == WHITE && pre_sq >> 3 == 6)|| (pc == BLACK && pre_sq >> 3 == 1)){
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, QUEEN));
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, ROOK));
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, BISHOP));
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, KNIGHT));    
            }
            else{
                moves.push_back(add_move(pre_sq, post_sq, 0, false, false, false, PAWN));
            }
        }

        u64 attacks = pawn_attack[(int)pc][pre_sq] & (piece_bbs[(int)(pc ^ 1)][ALL]);

        if (en_passant_sq != 0){
            attacks |= (pawn_attack[pc][pre_sq] & ((u64)1 << en_passant_sq));
        }

        while(attacks){
            int post_sq = lsb(attacks);
            attacks = bit_set_to(attacks, post_sq, 0);
            if (post_sq == en_passant_sq && en_passant_sq != 0){
                moves.push_back(add_move(pre_sq, post_sq, 0, false, false, true, PAWN));
            }
            //promotion
            else if ((pc == WHITE && pre_sq >> 3 == 6)|| (pc == BLACK && pre_sq >> 3 == 1)){
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, QUEEN));
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, ROOK));
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, BISHOP));
                moves.push_back(add_move(pre_sq, post_sq, 0, false, true, false, KNIGHT));   
            }
            else{
                moves.push_back(add_move(pre_sq, post_sq, 0, false, false, false, PAWN));
                
            }
        }
    }


}
void Board::generate_knight_moves(std::vector<Move> &moves){
    piece_color pc = (piece_color)get_color();
    u64 knights = piece_bbs[pc][KNIGHT];
    while (knights){
        int pre_sq = lsb(knights);
        knights = bit_set_to(knights, pre_sq, 0);
        u64 attacks = knight_table[pre_sq] & ~(piece_bbs[pc][ALL]);
        while(attacks){
            int post_sq = lsb(attacks);
            attacks = bit_set_to(attacks, post_sq, 0);
            moves.push_back(add_move(pre_sq, post_sq, 0, false, false, false, PAWN));
        }
    }
}
void Board::generate_bishop_moves(std::vector<Move> &moves){
    piece_color pc = (piece_color)get_color();
    u64 bishops = piece_bbs[pc][BISHOP];


    while (bishops){
        int pre_sq = lsb(bishops);
        bishops = bit_set_to(bishops, pre_sq, 0);


        u64 attacks = bishop_moves(f_bb, pre_sq) & ~(piece_bbs[pc][ALL]);



        while(attacks){
            int post_sq = lsb(attacks);
            attacks = bit_set_to(attacks, post_sq, 0);
            moves.push_back(add_move(pre_sq, post_sq, 0, false, false, false, PAWN));
        }
    }
}
void Board::generate_rook_moves(std::vector<Move> &moves){
    piece_color pc = (piece_color)get_color();
    u64 rooks = piece_bbs[pc][ROOK];

    while (rooks){
        int pre_sq = lsb(rooks);
        rooks = bit_set_to(rooks, pre_sq, 0);
        u64 attacks = rook_moves(f_bb, pre_sq) & ~(piece_bbs[pc][ALL]);

        while(attacks){
            int post_sq = lsb(attacks);
            attacks = bit_set_to(attacks, post_sq, 0);
            int change_castle = 0;
            if (pre_sq == 7 || pre_sq == 0 || pre_sq == 63 || pre_sq == 56){
                int idx = ((pre_sq >> 3) == 7 ? 2 : 0) + ((pre_sq + 1) & 1); 
                change_castle = (castling_rights[idx] << idx) & 0xF;
                
            }
            moves.push_back(add_move(pre_sq, post_sq, change_castle, false, false, false, PAWN));
        }
    }


}
void Board::generate_queen_moves(std::vector<Move> &moves){
    piece_color pc = (piece_color)get_color();
    u64 queens = piece_bbs[pc][QUEEN];
    while (queens){
        int pre_sq = lsb(queens);
        queens = bit_set_to(queens, pre_sq, 0);
        u64 attacks = queen_moves(f_bb, pre_sq) & ~(piece_bbs[pc][ALL]);
        while(attacks){
            int post_sq = lsb(attacks);
            attacks = bit_set_to(attacks, post_sq, 0);
            moves.push_back(add_move(pre_sq, post_sq, 0, false, false, false, PAWN));
        }
    }
}
void Board::generate_king_moves(std::vector<Move> &moves){
    piece_color pc = (piece_color)get_color();



    u64 kings = piece_bbs[pc][KING];
    while (kings){
        int pre_sq = lsb(kings);
        kings = bit_set_to(kings, pre_sq, 0);
        u64 attacks = king_table[pre_sq] & ~(piece_bbs[pc][ALL]);
        while(attacks){
            int post_sq = lsb(attacks);
            attacks = bit_set_to(attacks, post_sq, 0);
            int change_castle = 0xF & ((castling_rights[pc * 2] | (castling_rights[pc * 2 + 1] << 1)) << (((int)pc) * 2));

      
            moves.push_back(add_move(pre_sq, post_sq, change_castle, false, false, false, PAWN));
        }
        //castle kingside 
        if (pc == WHITE){
            //white kingside 
            int change_castle = 3 & (castling_rights[0] | (castling_rights[1] << 1));
            if (castling_rights[0] && get_bit(piece_bbs[WHITE][ROOK], H1) && !get_bit(f_bb, F1) && !inCheck(WHITE) && !get_bit(f_bb, G1) && !is_square_attacked(BLACK, F1) && !is_square_attacked(BLACK, G1)){
                moves.push_back(add_move(pre_sq, G1, change_castle, true, false, false, PAWN));
            }
             //white queenside
            if (castling_rights[1] && get_bit(piece_bbs[WHITE][ROOK], A1) &&!get_bit(f_bb, B1) && !get_bit(f_bb, C1) && !get_bit(f_bb, D1) && 
                !inCheck(WHITE) && !is_square_attacked(BLACK, C1) && !is_square_attacked(BLACK, D1)){
                moves.push_back(add_move(pre_sq, C1, change_castle, true, false, false, PAWN));
            }
        }
        else{
            int change_castle = 12 & ((castling_rights[2] | (castling_rights[3] << 1)) << 2);
            //black kingside 
            if (castling_rights[2] && get_bit(piece_bbs[BLACK][ROOK], H8) &&!get_bit(f_bb, F8) && !inCheck(BLACK) && !get_bit(f_bb, G8) && !is_square_attacked(WHITE, F8) && !is_square_attacked(WHITE, G8)){
                moves.push_back(add_move(pre_sq, G8, change_castle , true, false, false,  PAWN));
            }
            //black queenside
            if (castling_rights[3] && get_bit(piece_bbs[BLACK][ROOK], A8) &&!get_bit(f_bb, B8) && !get_bit(f_bb, C8) && !get_bit(f_bb, D8) && 
                !inCheck(BLACK) && !is_square_attacked(WHITE, C8) && !is_square_attacked(WHITE, D8)){
                moves.push_back(add_move(pre_sq, C8, change_castle , true, false, false, PAWN));
            }

        }

        
    }
}

Move Board::add_move(int pre_sq, int post_sq, int castling_rights, bool castling, bool promotion, bool en_passant, piece_type p_type){
    piece_color pc = sc_board[pre_sq].first;
    // piece_type pt = sc_board[pre_sq].second; 

    bool captured = false; 
    piece_type  c_type = ALL;

    
    //capture
    if (get_bit(piece_bbs[pc ^ 1][ALL], post_sq)){
        captured = true;
        c_type = sc_board[post_sq].second;
    }
    if (en_passant){
        captured = true;
        c_type = PAWN;  
    }

   

    return Move(pre_sq, post_sq, p_type, c_type, castling_rights, castling, captured, promotion, en_passant, en_passant_sq);
}

void Board::show_state(){


    for (int i = 7; i >= 0; i--){
        for (int j = 0; j < 8; ++j){
            if (sc_board[8 * i + j].first == NONE){
                std::cout << "   ";
            }
            else{
                // std::cout << sc_board[8 * i + j].first << sc_board[8 * i + j].second << " ";
                std::cout << pc_str[sc_board[8 * i + j].first] << pt_str[sc_board[8 * i + j].second] << " ";
            }
        }
        std::cout << std::endl;
    }
    

}

std::vector<Move> Board::generate_moves(bool capture){


    std::vector<Move> pseudolegal_moves;
    pseudolegal_moves.reserve(35); //average

    // std::vector<int> num_moves(6);
    // int idx = 0, pt = 0;
    // std::vector<std::string> pts = {"pawn", "knight", "bishop", "rook", "queen", "king"};


    generate_pawn_moves(pseudolegal_moves); // good



    generate_knight_moves(pseudolegal_moves);
    generate_bishop_moves(pseudolegal_moves);
    generate_rook_moves(pseudolegal_moves);
    generate_queen_moves(pseudolegal_moves);
    generate_king_moves(pseudolegal_moves);


   
    std::vector<Move> legal_moves;
    for (Move &move : pseudolegal_moves){


        make_move(move);    


        if (!inCheck((piece_color)(color_turn ^ 1))){
            if (!capture || move.check_capture()){
                legal_moves.push_back(move);
            }
        }

   
        unmake_move(move);

 
    }


    return legal_moves;
}

u64 Board::perft(int depth, bool divide){

    std::vector<Move> moves;
    int move_reserve_size = 256;
    moves.reserve(move_reserve_size);

    u64 nodes = 0;


    moves = generate_moves();



    if (depth == 1){
        return moves.size();
    }

    for (Move &move : moves){

        make_move(move); //fix  x  
        u64 new_nodes = perft(depth - 1); 

        // if (divide){
        //     std::cout << "depth: " << depth << " " << squares_RF_str[move.get_pre_sq()] << " " << squares_RF_str[move.get_post_sq()] << ": " << new_nodes << std::endl;
        // }

        nodes += new_nodes;
        unmake_move(move);


    }

   

    return nodes; 
}