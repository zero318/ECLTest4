#ifndef ECL_TEST_3_UTIL_H
#define ECL_TEST_3_UTIL_H

#include <stdint.h>
typedef uint_fast8_t flag;

//LittleEndian

typedef union ByteBits {
	uint8_t byte;
	struct {
		uint8_t _0 : 1; uint8_t _1 : 1; uint8_t _2 : 1; uint8_t _3 : 1; uint8_t _4 : 1; uint8_t _5 : 1; uint8_t _6 : 1; uint8_t _7 : 1;
	} bit;
} ByteBits;

typedef union WordBits {
	uint16_t word;
	struct {
		uint16_t _0 : 1; uint16_t _1 : 1; uint16_t _2 : 1; uint16_t _3 : 1; uint16_t _4 : 1; uint16_t _5 : 1; uint16_t _6 : 1; uint16_t _7 : 1;
		uint16_t _8 : 1; uint16_t _9 : 1; uint16_t _A : 1; uint16_t _B : 1; uint16_t _C : 1; uint16_t _D : 1; uint16_t _E : 1; uint16_t _F : 1;
	} bit;
} WordBits;

typedef union DwordBits {
	uint32_t dword;
	struct {
		uint32_t _00 : 1; uint32_t _01 : 1; uint32_t _02 : 1; uint32_t _03 : 1; uint32_t _04 : 1; uint32_t _05 : 1; uint32_t _06 : 1; uint32_t _07 : 1;
		uint32_t _08 : 1; uint32_t _09 : 1; uint32_t _0A : 1; uint32_t _0B : 1; uint32_t _0C : 1; uint32_t _0D : 1; uint32_t _0E : 1; uint32_t _0F : 1;
		uint32_t _10 : 1; uint32_t _11 : 1; uint32_t _12 : 1; uint32_t _13 : 1; uint32_t _14 : 1; uint32_t _15 : 1; uint32_t _16 : 1; uint32_t _17 : 1;
		uint32_t _18 : 1; uint32_t _19 : 1; uint32_t _1A : 1; uint32_t _1B : 1; uint32_t _1C : 1; uint32_t _1D : 1; uint32_t _1E : 1; uint32_t _1F : 1;
	} bit;
} DwordBits;

typedef union QwordBits {
	uint64_t qword;
	struct {
		uint64_t _00 : 1; uint64_t _01 : 1; uint64_t _02 : 1; uint64_t _03 : 1; uint64_t _04 : 1; uint64_t _05 : 1; uint64_t _06 : 1; uint64_t _07 : 1;
		uint64_t _08 : 1; uint64_t _09 : 1; uint64_t _0A : 1; uint64_t _0B : 1; uint64_t _0C : 1; uint64_t _0D : 1; uint64_t _0E : 1; uint64_t _0F : 1;
		uint64_t _10 : 1; uint64_t _11 : 1; uint64_t _12 : 1; uint64_t _13 : 1; uint64_t _14 : 1; uint64_t _15 : 1; uint64_t _16 : 1; uint64_t _17 : 1;
		uint64_t _18 : 1; uint64_t _19 : 1; uint64_t _1A : 1; uint64_t _1B : 1; uint64_t _1C : 1; uint64_t _1D : 1; uint64_t _1E : 1; uint64_t _1F : 1;
		uint64_t _20 : 1; uint64_t _21 : 1; uint64_t _22 : 1; uint64_t _23 : 1; uint64_t _24 : 1; uint64_t _25 : 1; uint64_t _26 : 1; uint64_t _27 : 1;
		uint64_t _28 : 1; uint64_t _29 : 1; uint64_t _2A : 1; uint64_t _2B : 1; uint64_t _2C : 1; uint64_t _2D : 1; uint64_t _2E : 1; uint64_t _2F : 1;
		uint64_t _30 : 1; uint64_t _31 : 1; uint64_t _32 : 1; uint64_t _33 : 1; uint64_t _34 : 1; uint64_t _35 : 1; uint64_t _36 : 1; uint64_t _37 : 1;
		uint64_t _38 : 1; uint64_t _39 : 1; uint64_t _3A : 1; uint64_t _3B : 1; uint64_t _3C : 1; uint64_t _3D : 1; uint64_t _3E : 1; uint64_t _3F : 1;
	} bits;
} QwordBits;

#endif