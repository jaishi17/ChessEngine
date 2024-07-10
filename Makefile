CC = g++
CFLAGS = -Wall -g
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

chess_engine: main.o
	$(CC) $(CFLAGS) -o chess_engine main.o board.o magic.o $(LIBS)

main.o: main.cpp board.cpp board.o defs.hpp magic.cpp magic.o
	$(CC) $(CFLAGS) -c main.cpp 

board.o: board.cpp board.hpp magic.o
	$(CC) $(CFLAGS) -c board.cpp

magic.o: magic.cpp board.hpp	
	$(CC) $(CFLAGS) -c magic.cpp

