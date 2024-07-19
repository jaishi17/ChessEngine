CC = g++
CFLAGS = -Wall -Wextra -O2 -g -pg
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

chess_engine: main.o
	$(CC) $(CFLAGS) -o chess_engine main.o board.o magic.o movegen.o search.o uci.o zobrist.o $(LIBS)

test: tests/test.o
	$(CC) $(CFLAGS) -o tests/test tests/test.o board.o magic.o movegen.o   $(LIBS)

tests/test.o: tests/test.cpp board.o defs.hpp 
	$(CC) $(CFLAGS) -c tests/test.cpp -o tests/test.o 

main.o: main.cpp board.o defs.hpp 
	$(CC) $(CFLAGS) -c main.cpp 

board.o: board.cpp board.hpp magic.o movegen.o search.o uci.o zobrist.o 
	$(CC) $(CFLAGS) -c board.cpp

magic.o: magic.cpp board.hpp	
	$(CC) $(CFLAGS) -c magic.cpp

movegen.o : movegen.cpp board.hpp
	$(CC) $(CFLAGS) -c movegen.cpp

search.o : search.cpp board.hpp
	$(CC) $(CFLAGS) -c search.cpp

uci.o : uci.cpp uci.hpp board.hpp
	$(CC) $(CFLAGS) -c uci.cpp

zobrist.o : zobrist.cpp zobrist.hpp board.hpp
	$(CC) $(CFLAGS) -c zobrist.cpp

