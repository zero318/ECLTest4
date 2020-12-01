#ifndef STRING_STREAM_H
#define STRING_STREAM_H

#include <stdlib.h>
#include <wchar.h>

typedef struct StringStream StringStream;
struct StringStream {
    char* String;
    char* CurrentPos;
    size_t Pos;
    size_t Size;
};

typedef struct WideStringStream WideStringStream;
struct WideStringStream {
    wchar_t* String;
    wchar_t* CurrentPos;
    size_t Pos;
    size_t Size;
};

inline char AdvanceStringStream(StringStream* StringStream);
inline char AdvanceWideStringStream(WideStringStream* StringStream);

#endif