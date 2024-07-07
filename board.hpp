#ifndef board_h
#define board_h
#include "defs.hpp"

#include <cstdint>
#include <string>
#include <vector>


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

        std::vector<square_piece> const getSCBoard() const;

        //clear all bitboards
        void clearBitboards();

        //clear square centric boards
        void clearSCBoards();

        //set board to fen 
        void setToFen(std::string fen);

    private:
        // use 64 bit integers for board represetnation for each piece type
        u64 piece_bbs[color_count][piece_count];   

        //use this to check what piece is at what square (rank first bit)
        std::vector<square_piece> sc_board = std::vector<square_piece>(64);




};

#endif