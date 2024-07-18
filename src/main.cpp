#include "board.hpp"
#include "uci.hpp"
#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <cstring> 


bool bottom_color = 0;

int loadPosition(Board& board, std::vector<sf::Sprite> &sprite_pieces){
    int idx = 0;
    std::vector<std::pair<piece_color, piece_type>> sc_board = board.getSCBoard();

    for (int i = 0; i < 64; ++i){
        if (sc_board[i].first == NONE){
            continue;
        }
        else{
            int x = sc_board[i].second % 6;
            int y = sc_board[i].first == WHITE ? 0 : 1;
            int rank = i >> 3, file = i % 8;
            int map_to_png[6] = {5, 3, 2, 4, 1, 0};
            x = map_to_png[x];
            sprite_pieces[idx].setTextureRect(sf::IntRect(piece_size * x, piece_size * y, piece_size, piece_size));
            if (bottom_color){
                sprite_pieces[idx].setPosition(square_size * (7 - file), square_size * (rank));
            }
            else{
                sprite_pieces[idx].setPosition(square_size * file, square_size * (7 - rank));

            }
            idx++;
        }
    }
    return idx;
}

int main() {
    uci_loop();
    return 0;
}

int main2(int argc, char * argv[]){



    std::string start_position = "8/8/8/8/2k5/8/r3p1r1/2K2R2 b - - 0 1";
    bool start_pos = false;
    Board board = Board(start_position);


    sf::RenderWindow window(sf::VideoMode(960, 960), "chess engine");
    sf::Texture piece_texture, board_texture;
    piece_texture.loadFromFile("../images/pieces2.png");
    board_texture.loadFromFile("../images/board.png");


    sf::Sprite sprite_board(board_texture);
    sprite_board.setScale(2.f, 2.f);

    std::vector<sf::Sprite> sprite_pieces(32);
    for(int i = 0; i < 32; ++i){
        sprite_pieces[i].setTexture(piece_texture);
        sprite_pieces[i].setScale(0.60f, 0.60f);
    }

    // first is what color i play, second is waht color is at bottom
    bool engine_color = WHITE;

    board.set_uci(false);


    if (argc >= 2){
        if (!std::strcmp(argv[1], "b")){
            engine_color = BLACK;
        }        
    }
    if (argc == 3){
        if (!std::strcmp(argv[2], "b")){
            bottom_color = BLACK;
        }
    }

    board.set_engine_color(engine_color);
    if (engine_color && start_pos){
        board.engine_move(-1, -1);
    }
    
    int num_pieces_remaining = loadPosition(board, sprite_pieces);

    std::string move = "";
    bool isMove = false;
    int moving_piece = 0;

    while (window.isOpen()){
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);

        sf::Event event;
        if(window.pollEvent(event)){
            switch (event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseButtonPressed:
                    for (int i = 0; i < num_pieces_remaining; ++i){
                        if (sprite_pieces[i].getGlobalBounds().contains(localPosition.x, localPosition.y)){
                            isMove = true;
                            moving_piece = i;
                        }
                    }
                    if (bottom_color){
                        move += (char)((7 - localPosition.x/square_size) + 'a');
                        move += (char)((localPosition.y/square_size) + '1');       
                    }
                    else{
                        move += (char)((localPosition.x/square_size) + 'a');
                        move += (char)((7 - localPosition.y/square_size) + '1');                    
                    }

                    break;
                case sf::Event::MouseButtonReleased:
                    isMove = false;
                   if (bottom_color){
                        move += (char)((7 - localPosition.x/square_size) + 'a');
                        move += (char)((localPosition.y/square_size) + '1');       
                    }
                    else{
                        move += (char)((localPosition.x/square_size) + 'a');
                        move += (char)((7 - localPosition.y/square_size) + '1');                    
                    }
           
                    break;
                default:
                    break;
            }
            if (move.size() == 4){
                std::cout << move << std::endl;
                board.update(move);
                move = "";
            }
        }





        num_pieces_remaining = loadPosition(board, sprite_pieces);

        if (isMove){
            sprite_pieces[moving_piece].setPosition(localPosition.x - square_size/2, localPosition.y - square_size/2);
        }

        window.clear();
        window.draw(sprite_board);
        for (int i = 0; i < num_pieces_remaining; ++i){
            window.draw(sprite_pieces[i]);
        }
        window.display();
    }

    return 0;

}