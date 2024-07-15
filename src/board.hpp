#ifndef board_h
#define board_h
#include "defs.hpp"

#include <cstdint>
#include <string>
#include <vector>


typedef uint64_t u64;
typedef uint16_t u16;

struct Move{

    Move(int pre_sq, int post_sq, piece_type p_type, piece_type c_type, int castling_rights, bool castle, bool capture, bool promotion, bool color_turn, bool en_passant, int prev_epsq);

    int get_pre_sq(){
        return move & 0x3f;
    }
    int get_post_sq(){
        return (move >> 6) & 0x3f;
    }

    inline int get_promoted(){
        return (move >> 12) & 0x7;
    }

    inline int get_captured(){
        return (move >> 15) & 0x7;
    }
    inline int get_castling(){
        return (move >> 18) & 0xF;
    }
    
    inline bool check_castle(){
        return (move >> 22) & 1;
    }

    inline bool check_capture(){
        return (move >> 23) & 1;
    }

    inline bool check_promoted(){
        return (move >> 24) & 1;
    }
    
    inline bool check_en_passant(){
        return (move >> 25) & 1;
    }

    inline bool get_color_turn(){
        return (move >> 26) & 1;
    }

    inline int get_epsq(){
        return (move >> 27) & 0x3F;
    }
    

    u64 move;
    // lsb to msb:
    // 6 bits from square
    // 6 bits to square  
    // 3 bits for promotion type
    // 3 bits for capture type
    // 4 bits for a change in castlign rights 
    // 4 bits for flags (castle, capture, promotion, en_passant)
    // 1 bit for whose turn it is (0 for white)
    // 6 bits for previous en passant square 

    // total: 32
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
        piece_color get_color();

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
        void move_piece(Move move);
        void update(std::string move);
        void make_move(Move move); 
        void unmake_move(Move move); 

        //move generation - movegen.cpp
        // std::vector<Move> generate_legal_moves(std::vector<Move> &moves);
        void generate_pawn_moves(std::vector<Move> &moves);
        void generate_knight_moves(std::vector<Move> &moves);
        void generate_bishop_moves(std::vector<Move> &moves);
        void generate_rook_moves(std::vector<Move> &moves);
        void generate_queen_moves(std::vector<Move> &moves);
        void generate_king_moves(std::vector<Move> &moves);
        Move add_move(int pre_sq, int post_sq, int castling_rights, bool castling, bool promotion, bool en_passant, piece_type p_type);
        std::vector<Move> generate_pseudolegal_moves();

        void show_state();
        u64 perft(int depth, bool divide = false);

        bool debug = false;

    private:


        bool castling_rights[4] = {true, true, true, true}; //WK, WQ, BK, bQ
        bool color_turn = false; //white = 0, black = 1
        int move_num = 0; 
        
        int en_passant_sq = 0; //square that the attacking pawn would capture in 

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