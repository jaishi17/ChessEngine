#include "board.hpp"
#include <SFML/Graphics.hpp>

#include <vector>
#include <iostream>
#include <string>


int loadPosition(const Board& board, std::vector<sf::Sprite> &sprite_pieces){
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
            sprite_pieces[idx].setPosition(square_size * file, square_size * (7 - rank));
            idx++;
        }
    }
    return idx;
}


int main(){

    std::string start_position = "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR";
    Board board = Board();

    // std::vector<std::string> piece_string = {"WK", "WQ", "WB", "WN", "WR", "WP", "BK", "BQ", "BB", "BN", "BR", "BP", "EM"};
    // std::vector<std::pair<piece_color, piece_type>> sc_board = board.getSCBoard();
    // for (int i = 7; i >= 0; --i){
    //     for (int j = 0; j < 8; ++j){
    //         std::cout << piece_string[(int)(sc_board[8 * i + j])] << " ";
    //     }
    //     std::cout << std::endl;
    // }



    sf::RenderWindow window(sf::VideoMode(960, 960), "chess engine");

    sf::Texture piece_texture, board_texture;
    piece_texture.loadFromFile("images/pieces2.png");
    board_texture.loadFromFile("images/board.png");


    sf::Sprite sprite_board(board_texture);
    sprite_board.setScale(2.f, 2.f);

    std::vector<sf::Sprite> sprite_pieces(32);
    for(int i = 0; i < 32; ++i){
        sprite_pieces[i].setTexture(piece_texture);
        sprite_pieces[i].setScale(0.60f, 0.60f);
    }


    int num_pieces_remaining = loadPosition(board, sprite_pieces);

    std::string move = "";
    bool isMove = false;
    int moving_piece = 0;
    float dx = 0, dy = 0;

    while (window.isOpen()){
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);

        sf::Event event;
        if(window.pollEvent(event)){
            switch (event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::TextEntered:
                    if (event.text.unicode != 8 && event.text.unicode < 128){
                        char c = static_cast<char>(event.text.unicode);
                        if (('1' <= c && c <= '8' && move.size() % 2 == 1) || ('a' <= c && c <= 'h' && move.size() % 2 == 0)){
                            move += c;
                            std::cout << move << std::endl;
                        }
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    if (event.mouseButton.button == sf::Mouse::Left){
                        move += (char)(localPosition.x/square_size + 'a');
                        move += (char)((8 - localPosition.y/square_size) + '0');
                        std::cout << move << std::endl;

                        for (int i = 0; i < num_pieces_remaining; ++i){
                            if (sprite_pieces[i].getGlobalBounds().contains(localPosition.x, localPosition.y)){
                                isMove = true;
                                moving_piece = i;
                                dx = localPosition.x - sprite_pieces[i].getPosition().x;
                                dy = localPosition.y - sprite_pieces[i].getPosition().y;
                            }
                        }

                    }
                    break;
                case sf::Event::MouseButtonReleased:
                    if (event.mouseButton.button == sf::Mouse::Left){
                        isMove = false;
                        move += (char)(localPosition.x/square_size + 'a');
                        move += (char)((8 - localPosition.y/square_size) + '0');
                        std::cout << move << std::endl;
                    }
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Backspace && !move.empty()){
                        move.pop_back();
                    }
                    break;
                default:
                    break;
            }
            if (move.size() == 4){
                board.update(move);
                move = "";
            }
        }


        num_pieces_remaining = loadPosition(board, sprite_pieces);

        if (isMove){
            sprite_pieces[moving_piece].setPosition(localPosition.x - dx, localPosition.y - dy);
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