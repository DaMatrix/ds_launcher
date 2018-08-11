#ifndef PORKMATH_H
#define PORKMATH_H

int decodeHex(char* in);
int decodeHexOffset(char* in, int offset);
char* encodeHex(int val);
void encodeHexTo(int val, char* to, int at);
char* encodeHexTerm(int val);

int roundUp(int numToRound, int multiple);

int min(int a, int b);
int max(int a, int b);
int clamp(int min, int max, int val);

#endif /* PORKMATH_H */
