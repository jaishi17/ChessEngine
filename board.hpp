#ifndef board_h
#define board_h


#include <cstdint>
#include <defs.hpp>

typedef uint64_t u64;



// use 64 bit integers for board represetnation for each piece type
class Board{
    public:

        //initialize board
        Board();

        //return piece bitboards
        u64 get_piece_bb(piece_color pc, piece_type pt);

    private:
        u64 piece_bbs[2][6];    


};

#endif