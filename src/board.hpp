#ifndef board_h
#define board_h
#include "defs.hpp"

#include <cstdint>
#include <string>
#include <vector>


typedef uint64_t u64;
typedef uint16_t u16;

struct Move{

    Move(int pre_sq, int post_sq, piece_type p_type, piece_type c_type, int castling_rights, bool castle, bool capture, bool promotion, bool color_turn);

    inline int get_pre_sq(){
        return move & 0x3f;
    }

    inline int get_post_sq(){
        return (move >> 6) & 0x3f;
    }

    inline int get_promoted(){
        return (move >> 12) & 0x3;
    }

    inline int get_captured(){
        return (move >> 14) & 0x7;
    }
    inline int get_castling(){
        return (move >> 17) & 0xF;
    }
    
    inline bool check_castle(){
        return (move >> 21) & 1;
    }

    inline bool check_capture(){
        return (move >> 22) & 1;
    }

    inline bool check_promoted(){
        return (move >> 23) & 1;
    }
    
    inline bool get_color_turn(){
        return (move >> 24) & 1;
    }
    

    int move;
    // lsb to msb:
    // 6 bits from square
    // 6 bits to square  
    // 2 bits for promotion type
    // 3 bits for capture type
    // 4 bits for a change in castlign rights 
    // 3 bits for flags (castle, capture, promotion)
    // 1 bit for whose turn it is (0 for white)
};

// use 64 bit integers for board represetnation for each piece type
class Board{
    public:

        //initialize
        Board();
        Board(std::string fen);
        void init_Board();
        void setToFen(std::string fen);
        void clearBitboards();
        void clearSCBoards();

        //debugging
        void print_square(u64 n);
        void print_square(u64 n, int square);


        std::vector<std::pair<piece_color, piece_type>> const getSCBoard() const;

        //leaping pieces
        void generate_pawn_table();
        void generate_knight_table();
        void generate_king_table();

        //sliding pieces magic stuff - in magic.cpp
        void init_magics();

        u64 rook_moves(u64 blockers, int square);
        u64 bishop_moves(u64 blockers, int square);
        u64 queen_moves(u64 blockers, int square);

        int rook_hash(u64 blockers, int square);
        int bishop_hash(u64 blockers, int square);

        void generate_rook_mask();
        void generate_rook_magic();

        void generate_bishop_mask();
        void generate_bishop_magic();

        u64 rook_attack(u64 blockers, int square);
        u64 bishop_attack(u64 blockers, int square);
        

        
        // check pseudo-legality
        bool check_pawn(piece_color pc, int pre_sq, int post_sq);
        bool check_knight(piece_color pc, int pre_sq, int post_sq);
        bool check_bishop(piece_color pc, int pre_sq, int post_sq);
        bool check_rook(piece_color pc, int pre_sq, int post_sq);
        bool check_queen(piece_color pc, int pre_sq, int post_sq);
        bool check_king(piece_color pc, int pre_sq, int post_sq);

        //useful stuff
        bool is_square_attacked(piece_color pc, int square); //pc is attacking
        bool inCheck(piece_color pc); //pc is defending

        //update board
        void update_bitboard(piece_color pc, piece_type pt, int pre_sq, int post_sq);
        void move_piece(piece_color pc, piece_type pt, int pre_sq, int post_sq);
        void update(std::string move);
        void make_move(Move move); 
        void unmake_move(Move move); 

        //move generation
        // std::vector<Move> generate_legal_moves(std::vector<Move> &moves);
        // std::vector<Move> generate_pseudolegal_moves(std::vector<Move> &moves);


    private:

        //TODO fifty move rule, three fold, time, insufficient, promotion
        bool castling_rights[4] = {true, true, true, true}; //WK, WQ, BK, bQ
        bool white_turn = true;
        int move_num = 0;

        std::vector<Move> move_list;

        // use 64 bit integers for board represetnation for each piece type, white, black, full
        u64 piece_bbs[color_count][piece_count + 1], f_bb;  
        std::vector<std::pair<piece_color, piece_type>> sc_board = std::vector<std::pair<piece_color, piece_type>>(64);

        //tables for move generation/checking
        u64 knight_table[64], king_table[64], pawn_push[2][64], pawn_attack[2][64];
        u64 rook_magic[64], rook_mask[64], bishop_magic[64], bishop_mask[64];
        std::vector<u64> rook_table[64], bishop_table[64];

};

#endif