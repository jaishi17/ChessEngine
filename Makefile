CC = g++
CFLAGS = -Wall -g
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

chess_engine: main.o
	$(CC) $(CFLAGS) -o chess_engine main.o board.o $(LIBS)

main.o: main.cpp board.cpp board.o defs.hpp
	$(CC) $(CFLAGS) -c main.cpp 

board.o: board.cpp board.hpp
	$(CC) $(CFLAGS) -c board.cpp


