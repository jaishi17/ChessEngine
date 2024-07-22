#ifndef book_h
#define book_h

#include <unordered_map>

struct Entry{
    short pre_sq, post_sq;
    int weight;
};

extern std::unordered_map<u64, Entry> book_moves;

void read_file();

#endif