#include "board.hpp"
#include <iostream>
#include <fstream>



Board::Board(){
    setToFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

Board::Board(std::string fen){
    setToFen(fen);
}

u64 Board::getPieceBB(piece_color pc, piece_type pt){
    return piece_bbs[pc][pt];
}

std::vector<square_piece> const Board::getSCBoard() const {
    return sc_board;
}




void Board::clearBitboards(){
    for (int i = 0; i < color_count; ++i){
        for (int j = 0; j < piece_count; ++j){
            piece_bbs[i][j] = 0;
        }
    }
}

void Board::clearSCBoards(){
    for (int i = 0; i < 64; ++i){
        sc_board[i] = EM;
    }
    
}

void Board::setToFen(std::string fen){


    clearBitboards();
    clearSCBoards();

    int rank = 8, fen_idx = 0;
    char file = 'a';
    int len = fen.size();

    while (fen_idx < len){


        if (fen[fen_idx] == '/'){
            rank--;
            file = 'a';
        }
        else if ('0' <= fen[fen_idx] && fen[fen_idx] <= '8'){
            file += (int)(fen[fen_idx] - '0');
        }
        else {
            int pos = (int)(8 * (rank - 1)  + file - 'a');
            file++;
            switch (fen[fen_idx]){
                case 'p':
                    piece_bbs[BLACK][PAWN] |= (1 << pos);
                    sc_board[pos] = BP;
                    break;
                case 'n':
                    piece_bbs[BLACK][KNIGHT] |= (1 << pos);
                    sc_board[pos] = BN;
                    break;
                case 'b':
                    piece_bbs[BLACK][BISHOP] |= (1 << pos);
                    sc_board[pos] = BB;
                    break;
                case 'r':
                    piece_bbs[BLACK][ROOK] |= (1 << pos);
                    sc_board[pos] = BR;
                    break;
                case 'q':
                    piece_bbs[BLACK][QUEEN] |= (1 << pos);
                    sc_board[pos] = BQ;
                    break;
                case 'k':
                    piece_bbs[BLACK][KING] |= (1 << pos);
                    sc_board[pos] = BK;
                    break;
                case 'P':
                    piece_bbs[WHITE][PAWN] |= (1 << pos);
                    sc_board[pos] = WP;
                    break;
                case 'N':
                    piece_bbs[WHITE][KNIGHT] |= (1 << pos);
                    sc_board[pos] = WN;
                    break;
                case 'B':
                    piece_bbs[WHITE][BISHOP] |= (1 << pos);
                    sc_board[pos] = WB;
                    break;
                case 'R':
                    piece_bbs[WHITE][ROOK] |= (1 << pos);
                    sc_board[pos] = WR;
                    break;
                case 'Q':
                    piece_bbs[WHITE][QUEEN] |= (1 << pos);
                    sc_board[pos] = WQ;
                    break;
                case 'K':
                    piece_bbs[WHITE][KING] |= (1 << pos);
                    sc_board[pos] = WK;
                    break;
            }
        }
        fen_idx++;
    }

}

