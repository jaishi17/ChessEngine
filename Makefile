CC = g++
CFLAGS = -Wall -Wextra -Werror -O2 -g
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

chess_engine: main.o
	$(CC) $(CFLAGS) -o chess_engine main.o board.o magic.o $(LIBS)

tests: tests/test.o
	$(CC) $(CFLAGS) -o tests tests/test.o board.o magic.o $(LIBS)

tests/test.o: tests/test.cpp board.cpp board.o defs.hpp magic.cpp magic.o
	$(CC) $(CFLAGS) -c tests/test.cpp -o tests/test.o

main.o: main.cpp board.cpp board.o defs.hpp magic.cpp magic.o
	$(CC) $(CFLAGS) -c main.cpp 

board.o: board.cpp board.hpp magic.o
	$(CC) $(CFLAGS) -c board.cpp

magic.o: magic.cpp board.hpp	
	$(CC) $(CFLAGS) -c magic.cpp

