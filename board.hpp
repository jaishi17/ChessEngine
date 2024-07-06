#ifndef board_h
#define board_h
#include <defs.hpp>

#include <cstdint>
#include <string>
typedef uint64_t u64;



// use 64 bit integers for board represetnation for each piece type
class Board{
    public:

        //initialize board
        Board();

        //initialize board with fen
        Board(std::string fen);

        //return piece bitboards
        u64 getPieceBB(piece_color pc, piece_type pt);

        //clear all bitboards
        void clearBitboards();

        //set board to fen 
        void setToFen(std::string fen);

    private:
        // use 64 bit integers for board represetnation for each piece type
        u64 piece_bbs[color_count][piece_count];   




};

#endif