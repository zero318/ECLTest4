#include "StringStream.h"

inline char AdvanceStringStream(StringStream* StringStream) {
	return *StringStream->CurrentPos++;
}

inline char AdvanceWideStringStream(WideStringStream* StringStream) {
	return *StringStream->CurrentPos++;
}