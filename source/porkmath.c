#include <porkmath.h>

char* hex = "0123456789abcdef";

int decodeHex(char* in) {
    unsigned int val = 0;
    for (int i = 0; i < 8; i++) {
        char c = in[i];
        for (int j = 0; j < 16; j++) {
            if (hex[j] == c) {
                val |= j << (i << 2);
                break;
            }
        }
    }
    return val;
}

int decodeHexOffset(char* in, int offset) {
    unsigned int val = 0;
    for (int i = 0; i < 8; i++) {
        char c = in[i + offset];
        for (int j = 0; j < 16; j++) {
            if (hex[j] == c) {
                val |= j << (i << 2);
                break;
            }
        }
    }
    return val;
}

char* encodeHex(int val) {
    char* letters = (char*) malloc(8 * sizeof(char));
    for (int i = 0; i < 8; i++) {
        letters[i] = hex[(val >> (i << 2)) & 0xF];
    }
    return letters;
}

char* encodeHexTerm(int val) {
    char* letters = (char*) malloc(9 * sizeof(char));
    for (int i = 0; i < 8; i++) {
        letters[i] = hex[(val >> (i << 2)) & 0xF];
    }
    letters[8] = 0;
    return letters;
}

void encodeHexTo(int val, char* to, int at) {
    for (int i = 0; i < 8; i++) {
        to[i + at] = hex[(val >> (i << 2)) & 0xF];
    }
}

int roundUp(int numToRound, int multiple) {
    if (multiple == 0) {
        return numToRound;
    }
    int remainder = numToRound % multiple;
    if (remainder == 0) {
        return numToRound;
    }
    return numToRound + multiple - remainder;
}

int min(int a, int b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int clamp(int min, int max, int val) {
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    } else {
        return val;
    }
}
