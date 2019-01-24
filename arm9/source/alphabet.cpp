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
    Font::SIZES[id] = (unsigned int) ((w & 0xF) | ((h & 0xF) << 4) | ((row_start & 0xF) << 8) | (1 << 31));
}

void Font::init() {
    //fill letters with empty values
    for (int i = 255; i >= 0; i--)  {
        Font::LETTERS[i] = 0;
        Font::SIZES[i] = 0;
    }

    unsigned int* rows = new unsigned int[16]; //cache for value things

    rows[0] = 0b01110;
    rows[1] = 0b10011;
    rows[2] = 0b10101;
    rows[3] = 0b11001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('0', 5, 6, 0, rows);

    rows[0] = 0b010;
    rows[1] = 0b110;
    rows[2] = 0b010;
    rows[3] = 0b010;
    rows[4] = 0b010;
    rows[5] = 0b111;
    addLetter('1', 3, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b00010;
    rows[3] = 0b00100;
    rows[4] = 0b01000;
    rows[5] = 0b11111;
    addLetter('2', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b00111;
    rows[3] = 0b00001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('3', 5, 6, 0, rows);

    rows[0] = 0b10010;
    rows[1] = 0b10010;
    rows[2] = 0b10010;
    rows[3] = 0b11111;
    rows[4] = 0b00010;
    rows[5] = 0b00010;
    addLetter('4', 5, 6, 0, rows);

    rows[0] = 0b11111;
    rows[1] = 0b10000;
    rows[2] = 0b01110;
    rows[3] = 0b00001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('5', 5, 6, 0, rows);

    rows[0] = 0b01111;
    rows[1] = 0b10000;
    rows[2] = 0b11110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('6', 5, 6, 0, rows);

    rows[0] = 0b11111;
    rows[1] = 0b00001;
    rows[2] = 0b00010;
    rows[3] = 0b00100;
    rows[4] = 0b01000;
    rows[5] = 0b10000;
    addLetter('7', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b01110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('8', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b01111;
    rows[4] = 0b00001;
    rows[5] = 0b11110;
    addLetter('9', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b11111;
    rows[4] = 0b10001;
    rows[5] = 0b10001;
    addLetter('A', 5, 6, 0, rows);

    rows[0] = 0b11110;
    rows[1] = 0b10001;
    rows[2] = 0b11110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b11110;
    addLetter('B', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b10000;
    rows[3] = 0b10000;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('C', 5, 6, 0, rows);

    rows[0] = 0b11110;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b11110;
    addLetter('D', 5, 6, 0, rows);

    rows[0] = 0b11111;
    rows[1] = 0b10000;
    rows[2] = 0b11110;
    rows[3] = 0b10000;
    rows[4] = 0b10000;
    rows[5] = 0b11111;
    addLetter('E', 5, 6, 0, rows);

    rows[0] = 0b11111;
    rows[1] = 0b10000;
    rows[2] = 0b11110;
    rows[3] = 0b10000;
    rows[4] = 0b10000;
    rows[5] = 0b10000;
    addLetter('F', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b10000;
    rows[3] = 0b10111;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('G', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b10001;
    rows[2] = 0b11111;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b10001;
    addLetter('H', 5, 6, 0, rows);

    rows[0] = 0b111;
    rows[1] = 0b010;
    rows[2] = 0b010;
    rows[3] = 0b010;
    rows[4] = 0b010;
    rows[5] = 0b111;
    addLetter('I', 3, 6, 0, rows);

    rows[0] = 0b11111;
    rows[1] = 0b00100;
    rows[2] = 0b00100;
    rows[3] = 0b00100;
    rows[4] = 0b10100;
    rows[5] = 0b01000;
    addLetter('J', 5, 6, 0, rows);

    rows[0] = 0b1001;
    rows[1] = 0b1010;
    rows[2] = 0b1100;
    rows[3] = 0b1100;
    rows[4] = 0b1010;
    rows[5] = 0b1001;
    addLetter('K', 4, 6, 0, rows);

    rows[0] = 0b10000;
    rows[1] = 0b10000;
    rows[2] = 0b10000;
    rows[3] = 0b10000;
    rows[4] = 0b10000;
    rows[5] = 0b11111;
    addLetter('L', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b11011;
    rows[2] = 0b10101;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b10001;
    addLetter('M', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b11001;
    rows[2] = 0b10101;
    rows[3] = 0b10011;
    rows[4] = 0b10001;
    rows[5] = 0b10001;
    addLetter('N', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('O', 5, 6, 0, rows);

    rows[0] = 0b11110;
    rows[1] = 0b10001;
    rows[2] = 0b11110;
    rows[3] = 0b10000;
    rows[4] = 0b10000;
    rows[5] = 0b10000;
    addLetter('P', 5, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b10101;
    rows[4] = 0b10011;
    rows[5] = 0b01111;
    addLetter('Q', 5, 6, 0, rows);

    rows[0] = 0b11110;
    rows[1] = 0b10001;
    rows[2] = 0b11110;
    rows[3] = 0b10100;
    rows[4] = 0b10010;
    rows[5] = 0b10001;
    addLetter('R', 5, 6, 0, rows);

    rows[0] = 0b01111;
    rows[1] = 0b10000;
    rows[2] = 0b01110;
    rows[3] = 0b00001;
    rows[4] = 0b00001;
    rows[5] = 0b11110;
    addLetter('S', 5, 6, 0, rows);

    rows[0] = 0b11111;
    rows[1] = 0b00100;
    rows[2] = 0b00100;
    rows[3] = 0b00100;
    rows[4] = 0b00100;
    rows[5] = 0b00100;
    addLetter('T', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('U', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b01010;
    rows[4] = 0b01010;
    rows[5] = 0b00100;
    addLetter('V', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b10001;
    rows[2] = 0b10001;
    rows[3] = 0b10101;
    rows[4] = 0b10101;
    rows[5] = 0b01010;
    addLetter('W', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b01010;
    rows[2] = 0b00100;
    rows[3] = 0b01010;
    rows[4] = 0b10001;
    rows[5] = 0b10001;
    addLetter('X', 5, 6, 0, rows);

    rows[0] = 0b10001;
    rows[1] = 0b10001;
    rows[2] = 0b01010;
    rows[3] = 0b00100;
    rows[4] = 0b00100;
    rows[5] = 0b00100;
    addLetter('Y', 5, 6, 0, rows);

    rows[0] = 0b11111;
    rows[1] = 0b00001;
    rows[2] = 0b00010;
    rows[3] = 0b00100;
    rows[4] = 0b01000;
    rows[5] = 0b11111;
    addLetter('Z', 5, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b00000;
    rows[3] = 0b00000;
    rows[4] = 0b00000;
    rows[5] = 0b00000;
    addLetter(' ', 4, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b01111;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01111;
    addLetter('a', 5, 4, 2, rows);

    rows[0] = 0b10000;
    rows[1] = 0b10000;
    rows[2] = 0b11110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b11110;
    addLetter('b', 5, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b01111;
    rows[3] = 0b10000;
    rows[4] = 0b10000;
    rows[5] = 0b01111;
    addLetter('c', 5, 4, 2, rows);

    rows[0] = 0b00001;
    rows[1] = 0b00001;
    rows[2] = 0b01111;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01111;
    addLetter('d', 5, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b01110;
    rows[3] = 0b11111;
    rows[4] = 0b10000;
    rows[5] = 0b01111;
    addLetter('e', 5, 6, 0, rows);

    rows[0] = 0b011;
    rows[1] = 0b100;
    rows[2] = 0b111;
    rows[3] = 0b100;
    rows[4] = 0b100;
    rows[5] = 0b100;
    addLetter('f', 3, 6, 0, rows);

    rows[2] = 0b01111;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01111;
    rows[6] = 0b00001;
    rows[7] = 0b11110;
    addLetter('g', 5, 7, 2, rows);

    rows[0] = 0b10000;
    rows[1] = 0b10000;
    rows[2] = 0b11110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b10001;
    addLetter('h', 5, 6, 0, rows);

    rows[0] = 0b1;
    rows[1] = 0b0;
    rows[2] = 0b1;
    rows[3] = 0b1;
    rows[4] = 0b1;
    rows[5] = 0b1;
    addLetter('i', 1, 6, 0, rows);

    rows[0] = 0b01;
    rows[1] = 0b00;
    rows[2] = 0b01;
    rows[3] = 0b01;
    rows[4] = 0b01;
    rows[5] = 0b01;
    rows[6] = 0b01;
    rows[7] = 0b10;
    addLetter('j', 2, 8, 0, rows);

    rows[0] = 0b100;
    rows[1] = 0b100;
    rows[2] = 0b101;
    rows[3] = 0b110;
    rows[4] = 0b101;
    rows[5] = 0b101;
    addLetter('k', 3, 6, 0, rows);

    rows[0] = 0b10;
    rows[1] = 0b10;
    rows[2] = 0b10;
    rows[3] = 0b10;
    rows[4] = 0b10;
    rows[5] = 0b01;
    addLetter('l', 2, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b11010;
    rows[3] = 0b10101;
    rows[4] = 0b10101;
    rows[5] = 0b10101;
    addLetter('m', 5, 4, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b11110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b10001;
    addLetter('n', 5, 4, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b01110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01110;
    addLetter('o', 5, 4, 2, rows);

    rows[2] = 0b11110;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b11110;
    rows[6] = 0b10000;
    rows[7] = 0b10000;
    addLetter('p', 5, 6, 2, rows);

    rows[2] = 0b01111;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01111;
    rows[6] = 0b00001;
    rows[7] = 0b00001;
    addLetter('q', 5, 6, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b11110;
    rows[3] = 0b10001;
    rows[4] = 0b10000;
    rows[5] = 0b10000;
    addLetter('r', 5, 4, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b01111;
    rows[2] = 0b10000;
    rows[3] = 0b01110;
    rows[4] = 0b00001;
    rows[5] = 0b11110;
    addLetter('s', 5, 5, 1, rows);

    rows[0] = 0b010;
    rows[1] = 0b111;
    rows[2] = 0b010;
    rows[3] = 0b010;
    rows[4] = 0b010;
    rows[5] = 0b010;
    addLetter('t', 3, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b10001;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01111;
    addLetter('u', 5, 4, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b10001;
    rows[3] = 0b10001;
    rows[4] = 0b01010;
    rows[5] = 0b00100;
    addLetter('v', 5, 4, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b10001;
    rows[3] = 0b10001;
    rows[4] = 0b10101;
    rows[5] = 0b01010;
    addLetter('w', 5, 4, 2, rows);

    rows[0] = 0b0000;
    rows[1] = 0b0000;
    rows[2] = 0b1001;
    rows[3] = 0b0110;
    rows[4] = 0b0110;
    rows[5] = 0b1001;
    addLetter('x', 4, 4, 2, rows);

    rows[2] = 0b10001;
    rows[3] = 0b10001;
    rows[4] = 0b10001;
    rows[5] = 0b01111;
    rows[6] = 0b00001;
    rows[7] = 0b11110;
    addLetter('y', 5, 6, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b11111;
    rows[3] = 0b00011;
    rows[4] = 0b01100;
    rows[5] = 0b11111;
    addLetter('z', 5, 4, 2, rows);

    rows[0] = 0b1;
    rows[1] = 0b1;
    rows[2] = 0b1;
    rows[3] = 0b1;
    rows[4] = 0b0;
    rows[5] = 0b1;
    addLetter('!', 1, 6, 0, rows);

    rows[0] = 0b01110;
    rows[1] = 0b10001;
    rows[2] = 0b00010;
    rows[3] = 0b00100;
    rows[4] = 0b00000;
    rows[5] = 0b00100;
    addLetter('?', 5, 6, 0, rows);

    rows[0] = 0b0;
    rows[1] = 0b0;
    rows[2] = 0b0;
    rows[3] = 0b0;
    rows[4] = 0b0;
    rows[5] = 0b1;
    addLetter('.', 1, 1, 5, rows);

    rows[0] = 0b00;
    rows[1] = 0b00;
    rows[2] = 0b00;
    rows[3] = 0b00;
    rows[5] = 0b01;
    rows[6] = 0b10;
    addLetter(',', 2, 2, 5, rows);

    rows[0] = 0b0;
    rows[1] = 0b1;
    rows[2] = 0b0;
    rows[3] = 0b0;
    rows[4] = 0b1;
    rows[5] = 0b0;
    addLetter(':', 1, 4, 1, rows);

    rows[1] = 0b01;
    rows[2] = 0b00;
    rows[3] = 0b00;
    rows[4] = 0b00;
    rows[5] = 0b01;
    rows[6] = 0b10;
    addLetter(';', 2, 6, 1, rows);

    rows[0] = 0b000;
    rows[1] = 0b000;
    rows[2] = 0b111;
    rows[3] = 0b000;
    rows[4] = 0b000;
    rows[5] = 0b000;
    addLetter('-', 3, 1, 2, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b00000;
    rows[3] = 0b00000;
    rows[4] = 0b00000;
    rows[5] = 0b11111;
    addLetter('_', 5, 1, 5, rows);

    rows[0] = 0b01;
    rows[1] = 0b10;
    rows[2] = 0b10;
    rows[3] = 0b10;
    rows[4] = 0b10;
    rows[5] = 0b01;
    addLetter('(', 2, 6, 0, rows);

    rows[0] = 0b10;
    rows[1] = 0b01;
    rows[2] = 0b01;
    rows[3] = 0b01;
    rows[4] = 0b01;
    rows[5] = 0b10;
    addLetter(')', 2, 6, 0, rows);

    rows[0] = 0b11;
    rows[1] = 0b10;
    rows[2] = 0b10;
    rows[3] = 0b10;
    rows[4] = 0b10;
    rows[5] = 0b11;
    addLetter('[', 2, 6, 0, rows);

    rows[0] = 0b11;
    rows[1] = 0b01;
    rows[2] = 0b01;
    rows[3] = 0b01;
    rows[4] = 0b01;
    rows[5] = 0b11;
    addLetter(']', 2, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00100;
    rows[2] = 0b00100;
    rows[3] = 0b11111;
    rows[4] = 0b00100;
    rows[5] = 0b00100;
    addLetter('+', 5, 5, 1, rows);

    rows[0] = 0b101;
    rows[1] = 0b101;
    rows[2] = 0b000;
    rows[3] = 0b000;
    rows[4] = 0b000;
    rows[5] = 0b000;
    addLetter('"', 3, 2, 0, rows);

    rows[0] = 0b010;
    rows[1] = 0b111;
    rows[2] = 0b010;
    rows[3] = 0b000;
    rows[4] = 0b000;
    rows[5] = 0b000;
    addLetter('*', 3, 3, 0, rows);

    rows[0] = 0b01010;
    rows[1] = 0b11111;
    rows[2] = 0b01010;
    rows[3] = 0b01010;
    rows[4] = 0b11111;
    rows[5] = 0b01010;
    addLetter('#', 5, 6, 0, rows);

    rows[1] = 0b10001;
    rows[2] = 0b00010;
    rows[3] = 0b00100;
    rows[4] = 0b01000;
    rows[5] = 0b10001;
    addLetter('%', 5, 5, 1, rows);

    rows[0] = 0b01000;
    rows[1] = 0b10100;
    rows[2] = 0b01000;
    rows[3] = 0b10101;
    rows[4] = 0b10010;
    rows[5] = 0b01101;
    addLetter('&', 5, 6, 0, rows);

    rows[0] = 0b01111;
    rows[1] = 0b10100;
    rows[2] = 0b01110;
    rows[3] = 0b00101;
    rows[4] = 0b00101;
    rows[5] = 0b11110;
    addLetter('$', 5, 6, 0, rows);

    rows[0] = 0b00000;
    rows[1] = 0b00000;
    rows[2] = 0b11111;
    rows[3] = 0b00000;
    rows[4] = 0b11111;
    rows[5] = 0b00000;
    addLetter('=', 5, 3, 2, rows);

    rows[0] = 0b1;
    rows[1] = 0b1;
    rows[2] = 0b0;
    rows[3] = 0b0;
    rows[4] = 0b0;
    rows[5] = 0b0;
    addLetter('\'', 1, 2, 0, rows);

    delete rows;
}
