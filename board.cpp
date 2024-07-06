#include "board.hpp"


Board::Board(){
    setToFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR")
}

Board::Board(std::string fen){
    setToFen(fen);
}

void Board::clearBitboards(){
    for (int i = 0; i < color_count; ++i){
        for (int j = 0; j < piece_count; ++j){
            piece_bbs[i][j] = 0;
        }
    }
}

void Board::setToFen(std::string fen){

    clearBitboards();

    int rank = 8, fen_idx = 0;
        char file = 'a';

        while (rank != 0){
            if (fen[fen_idx] == '/'){
                rank--;
                file = 'a';
            }
            else if ('0' <= fen[fen_idx] && fen[fen_idx] <= '8'){
                file += (int)(fen[fen_idx] - '0');
            }
            else {
                int pos = (int)(rank - 1  + 'h' - file);
                switch (fen[fen_idx]){
                    case 'p':
                        piece_bbs[BLACK][PAWN] |= (1 << pos);
                        break;
                    case 'n':
                        piece_bbs[BLACK][KNIGHT] |= (1 << pos);
                        break;
                    case 'b':
                        piece_bbs[BLACK][BISHOP] |= (1 << pos);
                        break;
                    case 'r':
                        piece_bbs[BLACK][ROOK] |= (1 << pos);
                        break;
                    case 'q':
                        piece_bbs[BLACK][QUEEN] |= (1 << pos);
                        break;
                    case 'k':
                        piece_bbs[BLACK][KING] |= (1 << pos);
                        break;
                    case 'P':
                        piece_bbs[WHITE][PAWN] |= (1 << pos);
                        break;
                    case 'N':
                        piece_bbs[WHITE][KNIGHT] |= (1 << pos);
                        break;
                    case 'B':
                        piece_bbs[WHITE][BISHOP] |= (1 << pos);
                        break;
                    case 'R':
                        piece_bbs[WHITE][ROOK] |= (1 << pos);
                        break;
                    case 'Q':
                        piece_bbs[WHITE][QUEEN] |= (1 << pos);
                        break;
                    case 'K':
                        piece_bbs[WHITE][KING] |= (1 << pos);
                        break;
                }
            }
            fen_idx++;
        }
}

u64 Board::getPieceBB(piece_color pc, piece_type pt){
    return piece_bbs[pc][pt];
};