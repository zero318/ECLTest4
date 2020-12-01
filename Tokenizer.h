#ifndef ECL_TOKENIZER_H
#define ECL_TOKENIZER_H

typedef enum ParseMode {
	Compile, Tokenize
} ParseMode;

typedef enum TokenEncoding {
    TokenDefault = 0,   //TokenEncodingSizes[0]
    TokenANSI = 1,      //TokenEncodingSizes[1]
    TokenUTF8 = 2,      //TokenEncodingSizes[2]
    TokenUTF16 = 3,     //TokenEncodingSizes[3]
    TokenUTF32 = 4,     //TokenEncodingSizes[4]
    TokenUCS2 = 5,      //TokenEncodingSizes[5]
    TokenUCS4 = 6,      //TokenEncodingSizes[6]
    TokenShiftJIS = 7   //TokenEncodingSizes[7]
} TokenEncoding;

void tokenize(char* Filename, ParseMode Mode);
#endif