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

        //initialize
        Board();
        Board(std::string fen);
        void setToFen(std::string fen);
        void clearBitboards();
        void clearSCBoards();

        inline u64 bit_set_to(u64 number, u64 n, bool x) {
            return (number & ~((u64)1 << n)) | ((u64)x << n);
        }

        //useful for debugging
        void print_square(u64 n);

        std::vector<std::pair<piece_color, piece_type>> const getSCBoard() const;

       
        void generate_knight_table();
        bool check_knight(piece_color pc, int pre_sq, int post_sq);

        /*
        void generate_king_table();
        bool check_king(piece_color pc, int pre_sq, int post_sq);

        void generate_pawn_table();
        bool check_pawn(piece_color pc, int pre_sq, int post_sq);
        
        */


        //magic stuff - in magic.cpp
        void init_magics();
        int rook_hash(u64 blockers, int square);
        int bishop_hash(u64 blockers, int square);
        u64 rook_moves(u64 blockers, int square);
        u64 bishop_moves(u64 blockers, int square);
        void generate_rook_mask();
        void generate_bishop_mask();
        void generate_rook_magic();
        void generate_bishop_magic();
        u64 rook_attack(u64 blockers, int square);
        u64 bishop_attack(u64 blockers, int square);
        


        void move_piece(piece_color pc, piece_type pt, int pre_sq, int post_sq, std::string move);
        void update(std::string move);

    private:

        //to add: castling rights, fifty move rule, three fold, time

        bool white_turn = true;
        int move_num = 0;

        std::vector<std::string> move_list;

        // use 64 bit integers for board represetnation for each piece type, white, black, full
        u64 piece_bbs[color_count][piece_count + 1], f_bb;  
        std::vector<std::pair<piece_color, piece_type>> sc_board = std::vector<std::pair<piece_color, piece_type>>(64);

        u64 knight_table[64];
        u64 rook_magic[64], rook_mask[64], bish_map, bishop_magic[64], bishop_mask[64];
        std::vector<u64> rook_table[64], bishop_table[64];

};

#endif