#include "font.h"

unsigned int* Font::LETTERS = new unsigned int[256];
unsigned int* Font::SIZES = new unsigned int[256];

void addLetter(unsigned char id, unsigned short w, unsigned short h, unsigned short row_start, unsigned int* rows)    {
    unsigned int v = 0;
    for (int i = h - 1; i >= 0; i--)    {
        unsigned int row = rows[i + row_start];
        unsigned int flipped = 0;
        for (int j = w - 1; j >= 0; j--)    {
            flipped |= ((row >> (w - j - 1)) & 1) << j;
        }
        v |= (flipped & ((1 << w) - 1)) << (w * i);
    }
    Font::LETTERS[id] = v;
    Font::SIZES[id] = (unsigned int) ((w & 0xF) | ((h & 0xF) << 4) | ((row_start & 0xF) << 8));
}

void Font::init() {
    //fill letters with empty values
    for (int i = 255; i >= 0; i--)  {
        Font::LETTERS[i] = 0;
        Font::SIZES[i] = 0;
    }

    unsigned int* rows = new unsigned int[16]; //cache for value things

    rows[0] = 0b0110;
    rows[1] = 0b1001;
    rows[2] = 0b1001;
    rows[3] = 0b1001;
    rows[4] = 0b0110;
    addLetter('0', 4, 5, 0, rows);

    rows[0] = 0b010;
    rows[1] = 0b110;
    rows[2] = 0b010;
    rows[3] = 0b010;
    rows[4] = 0b111;
    addLetter('1', 3, 5, 0, rows);

    rows[0] = 0b0110;
    rows[1] = 0b1001;
    rows[2] = 0b0010;
    rows[3] = 0b0100;
    rows[4] = 0b1111;
    addLetter('2', 4, 5, 0, rows);

    rows[0] = 0b0110;
    rows[1] = 0b1001;
    rows[2] = 0b0010;
    rows[3] = 0b1001;
    rows[4] = 0b0110;
    addLetter('3', 4, 5, 0, rows);

    delete rows;
}
