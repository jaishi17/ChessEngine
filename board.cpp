#include "board.hpp"


// use 64 bit integers for board represetnation for each piece type

u64 Board::get_piece_bb(piece_color pc, piece_type pt){
    return piece_bbs[pc][pt];
};