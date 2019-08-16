#ifndef DS_LAUNCHER_FONT_H
#define DS_LAUNCHER_FONT_H

//
// this is a really hacky way of getting a font into the program quickly
// without having to write some sort of file parser
//
// because screw that
//

#define BITS_PER_ROW = 5;
#define ROW_MASK = (1 << BITS_PER_ROW) - 1;

#define WIDTH(size) (size & 0xF)
#define HEIGHT(size) ((size >>> 4) & 0xF)

struct Font {
    static unsigned int* LETTERS;
    static unsigned int* SIZES;
public:
    static void init();
};


#endif //DS_LAUNCHER_FONT_H
