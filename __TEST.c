#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>
#include <math.h>
#include <stdnoreturn.h>
//#include <stdatomic.h>


//#include "_RegisterWatch.h"

#define __building_module(e) 1
#ifdef __STDC_HOSTED__
#undef __STDC_HOSTED__
#endif
#include <intrin.h>
#ifndef __STDC_HOSTED__
#define __STDC_HOSTED__ 1
#endif
#include <x86intrin.h>
#include <immintrin.h>
#include <cpuid.h>

#define iabs(i) _Generic((i), \
	signed char : abs,\
	short       : abs,\
	int         : abs,\
	long        : labs,\
	long long   : llabs\
)

#if __GNUC__
#define FastCallWrapper(ReturnType, FuncDef) __attribute__((fastcall)) ReturnType FuncDef; __attribute__((fastcall)) ReturnType FuncDef
#else
#define FastCallWrapper(ReturnType, FuncDef) ReturnType __fastcall FuncDef
#endif

typedef union IntBytes {
	uint32_t DWord;
	struct {
		uint32_t Word : 16;
		uint32_t SwapWord : 16;
	};
	struct {
		uint32_t LowByte : 8;
		uint32_t HighByte : 8;
		uint32_t : 16;
		//uint32_t SwapLowByte  : 8;
		//uint32_t SwapHighByte : 8;
	};
	struct {
		uint32_t _0 : 8;
		uint32_t _1 : 8;
		uint32_t _2 : 8;
		uint32_t _3 : 8;
	} Bytes;
	char Chars[4];
} IntBytes;

inline char* readFile(const char* filename) {
	FILE* cur_file = fopen(filename, "rb");
	long length;
	char* buffer = 0;
	if (cur_file) {
		fseek(cur_file, 0, SEEK_END);
		length = ftell(cur_file);
		fseek(cur_file, 0, SEEK_SET);
		buffer = (char*)malloc(length + 1);
		if (buffer) {
			fread(buffer, 1, length, cur_file);
		}
		fclose(cur_file);
		buffer[length] = '\0';
	};
	return buffer;
};

FastCallWrapper(char*, StringCStrip3(char* String, char c)) {
	int building_position = 0;
	int len = strlen(String);
	int i;
	for (i = 0; i < len; i++) {
		char current_character = String[i];
		if (current_character != c) {
			String[building_position] = current_character;
			building_position++;
		}
	}
	String[building_position] = '\0';
	return String;
}

FastCallWrapper(void, StringCStrip3BOld(register char* String, register char c)) {
	register char C = c;
	char *const OriginalString = String;
	char* ScratchString = OriginalString;
	//if (ScratchString != NULL && *ScratchString != '\0') {
		//while (*(++ScratchString) != '\0');
		//char* EndString = ScratchString + strlen(ScratchString) + 1;
		char* EndString = strchr(ScratchString, '\0') + 1;
		//char* EndString = ++ScratchString;
		//ScratchString = OriginalString;
		//while (*(ScratchString) != C) if (++ScratchString == EndString) return OriginalString;
		char* BuildString = ScratchString;
		register char temp;
		do if ((temp = *(ScratchString++)) != C) *(BuildString++) = temp;
		while (ScratchString != EndString);
		//*BuildString = '\0';
	//}
	return;
}

FastCallWrapper(void, StringCStrip3B(register char* String, register char c)) {
	register char C = c;
	char *const OriginalString = String;
	char* ScratchString = OriginalString;
	char* EndString = strchr(ScratchString, '\0') + 1;
	char* BuildString = ScratchString;
	register char temp;
	do if ((temp = *(ScratchString++)) != C) *(BuildString++) = temp;
	while (ScratchString != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3C(register char* String, register char c)) {
	register char C = c;
	char *const OriginalString = String;
	char* ScratchString = OriginalString;
	char* EndString = ScratchString + strlen(ScratchString) + 1;
	char* BuildString = ScratchString;
	register char temp;
	do if ((temp = *(ScratchString++)) != C) *(BuildString++) = temp;
	while (ScratchString != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3I(register char* String, register char c)) {
	register char C = c;
	char *const volatile OriginalString = String;
	char* ScratchString = OriginalString;
	char* EndString = ScratchString + strlen(ScratchString) + 1;
	char* BuildString = ScratchString;
	register char temp;
	do if ((temp = *(ScratchString++)) != C) *(BuildString++) = temp;
	while (ScratchString != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3J(register char* String, register char c)) {
	char* BuildString = String;
	char *EndString = String + strlen(String) + 1;
	register char temp;
	do if ((temp = *(String++)) != c) *(BuildString++) = temp;
	while (String != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3K(register char* String, register char c)) {
	char* BuildString = String;
	char *EndString = String + strlen(String) + 1;
	//register char temp;
	do {
		if (*(String) != c) {
			*(BuildString++) = *(String);
		}
	}  while (++String != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3L(register char* String, register char c)) {
	char* BuildString = String;
	char *const EndString = String + strlen(String);
	do if (*String != c) *BuildString++ = *String;
	while (++String != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3M(register char* String, register char c)) {
	char* BuildString = String;
	char *const EndString = strchr(String,'\0');
	do if (*String != c) *BuildString++ = *String;
	while (++String != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3N(register char* String, register char c)) {
	char* BuildString = String;
	char *const EndString = strchr(String--, '\0');
EvilLabelN:
	if (*++String == c) goto EvilLabelN;
	*BuildString++ = *String;
	if (String != EndString) goto EvilLabelN;
	return;
}

FastCallWrapper(void, StringCStrip3O(register char* String, register char c)) {
	char* BuildString = String;
	char *const EndString = String + strlen(String);
	--String;
EvilLabelO:
	if (*++String == c) goto EvilLabelO;
	*BuildString++ = *String;
	if (String != EndString) goto EvilLabelO;
	return;
}

FastCallWrapper(void, StringCStrip3P(register char* String, register char c)) {
	char* BuildString = String;
	goto EvilLabelP2;
EvilLabelP1:
	++String;
EvilLabelP2:
	if (*String != '\0') goto EvilLabelP1;
	char *const EndString = String;
	String = BuildString;
	--String;
EvilLabelP3:
	if (*++String == c) goto EvilLabelP3;
	*BuildString++ = *String;
	if (String != EndString) goto EvilLabelP3;
	return;
}

FastCallWrapper(void, StringCStrip3Q(register char *__restrict String, register char c)) {
	char* BuildString = String;
EvilLabelQ:
	if (*++String == c) goto EvilLabelQ;
	*BuildString++ = *String;
	if (*String != '\0') goto EvilLabelQ;
	return;
}

FastCallWrapper(void, StringCStrip3R(register char *__restrict String, register char c)) {
	register char* BuildString = String;
	register char temp;
EvilLabelR:
	if ((temp = *++String) == c) goto EvilLabelR;
	*BuildString++ = temp;
	if (temp != '\0') goto EvilLabelR;
	return;
}

FastCallWrapper(void, StringCStrip3S(register char *__restrict String, register char c)) {
	register char* BuildString = String; register char temp;
	do {
		if ((temp = *++String) == c) continue;
		*BuildString++ = temp;
	} while (temp != '\0');
	return;
}

FastCallWrapper(void, StringCStrip3T(char* String, char c)) {
	char* BuildString = String;
EvilLabelT:
	if (*++String == c) goto EvilLabelT;
	*BuildString++ = *String;
	if (*String != '\0') goto EvilLabelT;
	return;
}

FastCallWrapper(void, StringCStrip3U(char* String, char c)) {
	char* BuildString = String;
	register char temp;
EvilLabelU:
	if ((temp = *String++) == c) goto EvilLabelU;
	*BuildString++ = temp;
	if (temp != '\0') goto EvilLabelU;
	return;
}

FastCallWrapper(void, StringCStrip3V(char* String, char c)) {
	char* BuildString = String;
	int32_t temp;
EvilLabelV:
	if ((temp = *String++) == c) goto EvilLabelV;
	*BuildString++ = temp;
	if (temp) goto EvilLabelV;
	return;
}

FastCallWrapper(void, StringCStrip3W(char* String, char c)) {
	char* BuildString = String;
	goto EvilLabelW2;
EvilLabelW1:
	if (*++String == c) goto EvilLabelW1;
	*BuildString++ = *String;
EvilLabelW2:
	if (*String != '\0') goto EvilLabelW1;
	return;
}

FastCallWrapper(void, StringCStrip3X(char* String, char c)) {
	char* BuildString = String--;
	register char temp;
EvilLabelX:
	if (*++String == c) goto EvilLabelX;
	temp = *BuildString++ = *String;
	if (temp != '\0') goto EvilLabelX;
	return;
}

FastCallWrapper(void, StringCStrip3Y(char* String, char c)) {
	char* BuildString = --String;
EvilLabelY:
	++String;
	if (*String == c) goto EvilLabelY;
	register char temp = *++BuildString = *String;
	if (temp != '\0') goto EvilLabelY;
	return;
}

FastCallWrapper(void, StringCStrip3Z(char* String, char c)) {
	char* BuildString = --String;
EvilLabelZ:;
	register char temp = *++String;
	if (temp == c) goto EvilLabelZ;
	*++BuildString = temp;
	if (temp != '\0') goto EvilLabelZ;
	return;
}

FastCallWrapper(void, StringCStrip3AA(char* String, char c)) {
	register char temp, *BuildString = --String;
EvilLabelAA:
	if ((temp = *++String) == c) goto EvilLabelAA;
	if ((*++BuildString = temp) != '\0') goto EvilLabelAA;
	return;
}

FastCallWrapper(void, StringCStrip3BB(char* String, char c)) {
	register char temp, *BuildString = --String;
EvilLabelBB:
	switch (temp = *++String) {
		default:
			if (temp == c) goto EvilLabelBB;
			*++BuildString = temp;
			goto EvilLabelBB;
		case '\0':
			*++BuildString = temp;
			return;
	}
}

FastCallWrapper(void, StringCStrip3CC(char* String, char c)) {
	register char temp, *BuildString = --String;
EvilLabelCC:
	if ((temp = *++String) == c || (*++BuildString = temp) != '\0') goto EvilLabelCC;
	return;
}

FastCallWrapper(void, StringCStrip3DD(char* String, char c)) {
	register char temp, *BuildString = --String;
EvilLabelDD:
	if ((temp = *++String) == c || (*++BuildString = temp)&-1) goto EvilLabelDD;
	return;
}

FastCallWrapper(void, StringCStrip3EE(int* String, char c)) {
	register IntBytes Scratch;
	register IntBytes Build;
	int *BuildString = --String;
EvilLabelEE1:
	Scratch.DWord = *++String;
	if (Scratch.LowByte != c) Build.LowByte = Scratch.LowByte;
	if (Scratch.LowByte == '\0') goto EvilLabelEE2;
	if (Scratch.HighByte != c) Build.HighByte = Scratch.HighByte;
	if (Scratch.HighByte == '\0') goto EvilLabelEE2;
	Scratch.DWord = (Scratch.DWord >> 16) | (Scratch.DWord << 16);
	Build.DWord = (Build.DWord >> 16) | (Build.DWord << 16);
	if (Scratch.LowByte != c) Build.LowByte = Scratch.LowByte;
	if (Scratch.LowByte == '\0') goto EvilLabelEE3;
	if (Scratch.HighByte != c) Build.HighByte = Scratch.HighByte;
	if (Scratch.HighByte == '\0') goto EvilLabelEE3;
	Build.DWord = (Build.DWord >> 16) | (Build.DWord << 16);
	*++BuildString = Build.DWord;
	goto EvilLabelEE1;
EvilLabelEE3:
	Build.DWord = (Build.DWord >> 16) | (Build.DWord << 16);
EvilLabelEE2:
	*++BuildString = Build.DWord;
	return;
}

char* StringCStrip3Original(char* String, char c) {
	char* ScratchString = String;
	if (*ScratchString != '\0') {
		while (*(++ScratchString) != '\0');
		char* EndString = ScratchString;
		ScratchString = String;
		do {
			if (*ScratchString == c) {
				char* TempString = ScratchString;
				while (*(++ScratchString) == c);
				ptrdiff_t EndDelta = EndString - ScratchString;
				do {
					*(TempString++) = *(ScratchString);
				} while (++ScratchString != EndString);
				EndString -= EndDelta - 1;
				ScratchString = TempString - EndDelta;
			}
		} while (++ScratchString != EndString);
		*(--EndString) = '\0';
	}
	return String;
}

__attribute__((naked, regparm(2))) void StringCStrip3Naked(short c, char* String);
__attribute__((naked, regparm(2))) void StringCStrip3Naked(short c, char* String) {
	__asm ( "MOV %edx, %ecx\n"
			"EvilLabelNaked: MOV (%ecx), %al\n"
			"INC %ecx\n"
			"CMP %ah, %al\n"
			"JE EvilLabelNaked\n"
			"MOV %al, (%edx)\n"
			"INC %edx\n"
			"CMP $0, %al\n"
			"JNZ EvilLabelNaked\n"
			"RET");
}

__attribute__((naked, regparm(2))) void StringCStrip3NakedB(short c, char* String);
__attribute__((naked, regparm(2))) void StringCStrip3NakedB(short c, char* String) {
	__asm ("MOV %edx, %ecx\n"
		   "NOPL %cs:0L(%eax,%eax,1); nopl 0(%eax,%eax,1); nopl 0(%eax,%eax,1)\n"
		   //"NOPW (%eax); NOPL (%eax); NOPL (%eax)\n"
		   //"XCHG %ax, %ax; nopl %cs:(%eax); nopl %cs:(%eax)\n"
		   "EvilLabelNakedB: MOV (%ecx), %al\n"
		   "INC %ecx\n"
		   "CMP %ah, %al\n"
		   "JE EvilLabelNakedB\n"
		   "MOV %al, (%edx)\n"
		   "INC %edx\n"
		   "CMP $0, %al\n"
		   "JNZ EvilLabelNakedB\n"
		   "RET");
}

FastCallWrapper(void, StringCStrip3H(register char* String, register char c)) {
	register char C = c;
	char *const OriginalString = String;
	char* ScratchString = OriginalString;
	char* EndString = strrchr(ScratchString, '\0') + 1;
	char* BuildString = ScratchString;
	register char temp;
	do if ((temp = *(ScratchString++)) != C) *(BuildString++) = temp;
	while (ScratchString != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3E(register char *__restrict String, register char c)) {
	register char C = c;
	char *const OriginalString = String;
	char* ScratchString = OriginalString;
	//if (ScratchString != NULL && *ScratchString != '\0') {
	while (*(++ScratchString) != '\0');
	//char* EndString = ScratchString + strlen(ScratchString) + 1;
	//char* EndString = strchr(ScratchString, '\0') + 1;
	char* EndString = ++ScratchString;
	ScratchString = OriginalString;
	//while (*(ScratchString) != C) if (++ScratchString == EndString) return OriginalString;
	char* BuildString = ScratchString;
	register char temp;
	do if ((temp = *(ScratchString++)) != C) *(BuildString++) = temp;
	while (ScratchString != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3F(register char *__restrict String, register char c)) {
	//register char *volatile OriginalString = String;
	char* ScratchString = String;
	while (*(ScratchString++) != '\0');
	char* EndString = ScratchString;
	ScratchString = String;
	//while (*(ScratchString) != C) if (++ScratchString == EndString) return OriginalString;
	char* BuildString = ScratchString;
	register char temp;
	do if ((temp = *(ScratchString++)) != c) *(BuildString++) = temp;
	while (ScratchString != EndString);
	return;
}

FastCallWrapper(void, StringCStrip3G(register char *__restrict String, register char c)) {
	register char *volatile OriginalString = String;
	char* ScratchString = OriginalString;
	while (*(ScratchString++) != '\0');
	char* EndString = ScratchString;
	ScratchString = OriginalString;
	//while (*(ScratchString) != C) if (++ScratchString == EndString) return OriginalString;
	char* BuildString = ScratchString;
	register char temp;
	do if ((temp = *(ScratchString++)) != c) *(BuildString++) = temp;
	while (ScratchString != EndString);
	return;
}

FastCallWrapper(char*, StringSStripReplace3B(register char* String, register const char* cset, register char d)) {
	const char *const OriginalCSet = cset;
	char *const OriginalString = String;
	if (OriginalString != NULL && *OriginalString != '\0' && OriginalCSet != NULL && *OriginalCSet != '\0') {
		bool CharLookup[UCHAR_MAX + 1];
		const char* Cset = OriginalCSet;
		register char temp;
		while ((temp = *(Cset++)) != '\0') *(CharLookup+(unsigned char)temp) = true;
		char* ScratchString = OriginalString;
		while (*(++ScratchString) != '\0');
		char* EndString = ++ScratchString;
		ScratchString = OriginalString;
		while (!*(CharLookup+((unsigned char)*ScratchString))) if (++ScratchString == EndString) return OriginalString;
		register const char D = d;
		char* BuildString = ScratchString;
		do {
			if (!*(CharLookup + (unsigned char)(temp = *(ScratchString++)))) {
				*(BuildString++) = temp;
			} else {
				*(BuildString++) = D;
				while (*(CharLookup + (unsigned char)*ScratchString)) ++ScratchString;
			}
		}  while (ScratchString != EndString);
		Cset = OriginalCSet;
		while ((temp = *(Cset++)) != '\0') *(CharLookup + (unsigned char)temp) = false;
		*BuildString = '\0';
	}
	return OriginalString;
}

FastCallWrapper(char*, StringCStrip4(char* String, char c)) {
	char* ScratchString = String;
	if (ScratchString != NULL || *ScratchString != '\0') {
		while (*(++ScratchString) != '\0');
		char* EndString = ++ScratchString;
		ScratchString = String;
		do {
			if (*ScratchString != c) {
				++ScratchString;
				continue;
			}
			char* TempString = ScratchString;
			while (*(++ScratchString) == c);
			ptrdiff_t ScratchDelta = EndString - --ScratchString;
			do *(TempString++) = *(++ScratchString);
			while (ScratchString != EndString);
			EndString -= ScratchString - TempString;
			ScratchString = TempString - ScratchDelta;
		} while (ScratchString != EndString);
		*EndString = '\0';
	}
	return String;
}

FastCallWrapper(char*, StringCStrip5(char* String, char c)) {
	char* ScratchString = String;
	if (ScratchString != NULL || *ScratchString != '\0') {
		char* BuildString = ScratchString;
		while (*(++ScratchString) != '\0');
		char* EndString = ++ScratchString;
		ScratchString = BuildString;
		while (*(ScratchString) != c) if (++ScratchString == EndString) return String;
		BuildString = ScratchString;
		do {

			if (*ScratchString != c) {
				++ScratchString;
				++BuildString;
				continue;
			}
			//char temp;
			do ++ScratchString;
			while (*ScratchString == c);
			*BuildString = *ScratchString;


			/*char* TempString = ScratchString;
			
			ptrdiff_t ScratchDelta = EndString - --ScratchString;
			do *(TempString++) = *(++ScratchString);
			while (ScratchString != EndString);
			EndString -= ScratchString - TempString;
			ScratchString = TempString - ScratchDelta;*/
		} while (ScratchString != EndString);
		*EndString = '\0';
	}
	return String;
}

//#define FULL_RANGE true

#if FULL_RANGE
#define LOOP_START 1
#define LOOP_END 256
#else
#define LOOP_START 32
#define LOOP_END 127
#endif
#define LOOP_COUNT (LOOP_END - LOOP_START)
#define NANOSECOND_MULTIPLIER 1000000000

#define TEST_COUNT 25

#define _AlexFunc(String, i) StringCStrip3CC(String, i)
#define _DougFunc(String, i) StringCStrip3EE(String, i)//StringCStrip3NakedB(i<<8, String)
//StringCStrip3X(String, i)

#define AlexName "Alex"
#define AlexFunc(String, i) _AlexFunc(String, i)
#define DougName "Doug"
#define DougFunc(String, i) _DougFunc(String, i)

#define ProfilingPrint PRIuMAX

#define ProfilingType(Type) \
typedef u##Type ProfilingTime;\
typedef Type SProfilingTime;

ProfilingType(intmax_t);

typedef enum WhoFirst {
	Alex = 0, Doug = 1
} WhoFirst;
typedef enum WhichWasFaster {
	First = 0, Second = 1
} WhichWasFaster;

inline WhichWasFaster doTestCases(const char *const __restrict OriginalString, char *const __restrict TestString, size_t StringLength, WhoFirst First, ProfilingTime* TimeDiff, double* RatioDiff) {
	
	struct timespec TimeTemp, TimeTemp2;
	
	ProfilingTime AlexAvgs[TEST_COUNT] = { 0 };
	ProfilingTime DougAvgs[TEST_COUNT] = { 0 };
	ProfilingTime AlexTotalAvg = 0;
	ProfilingTime DougTotalAvg = 0;

	ProfilingTime DougTimes[256] = { 0 };
	ProfilingTime DougTime = 0;
	ProfilingTime AlexTimes[256] = { 0 };
	ProfilingTime AlexTime = 0;

	bool SecondFunction;
	uint_fast16_t i;
	uint_fast16_t j;
  //printf("###### | ############ | ############");
	printf("Test # | Alex ns      | Doug ns\n");
	for (i = 0; i < TEST_COUNT; ++i) {
		AlexTime = 0;
		DougTime = 0;
		SecondFunction = false;
		do {
			if ((!SecondFunction && First == Alex) || (SecondFunction && First == Doug)) {
				for (j = LOOP_START; j < LOOP_END; ++j) {
					memcpy(TestString, OriginalString, StringLength);
					//strcpy(TestString, OriginalString);
					timespec_get(&TimeTemp, TIME_UTC);
					AlexFunc(TestString, (char)j);
					timespec_get(&TimeTemp2, TIME_UTC);
					AlexTimes[j] = (((ProfilingTime)TimeTemp2.tv_sec * (ProfilingTime)NANOSECOND_MULTIPLIER) + (ProfilingTime)TimeTemp2.tv_nsec) - (((ProfilingTime)TimeTemp.tv_sec * (ProfilingTime)NANOSECOND_MULTIPLIER) + (ProfilingTime)TimeTemp.tv_nsec);
				}
			} else {
				for (j = LOOP_START; j < LOOP_END; ++j) {
					memcpy(TestString, OriginalString, StringLength);
					//strcpy(TestString, OriginalString);
					//uint_fast16_t k = j << 8;
					timespec_get(&TimeTemp, TIME_UTC);
					DougFunc((int*)TestString, (char)j);
					timespec_get(&TimeTemp2, TIME_UTC);
					DougTimes[j] = (((ProfilingTime)TimeTemp2.tv_sec * (ProfilingTime)NANOSECOND_MULTIPLIER) + (ProfilingTime)TimeTemp2.tv_nsec) - (((ProfilingTime)TimeTemp.tv_sec * (ProfilingTime)NANOSECOND_MULTIPLIER) + (ProfilingTime)TimeTemp.tv_nsec);
				}
			}
		} while (SecondFunction++ == false);
		for (j = LOOP_START; j < LOOP_END; ++j) {
			AlexTime += AlexTimes[j];
			DougTime += DougTimes[j];
		}
		AlexTime /= LOOP_COUNT;
		AlexAvgs[i] = AlexTime;
		DougTime /= LOOP_COUNT;
		DougAvgs[i] = DougTime;
		printf("%-6"PRIuFAST16" | %12"ProfilingPrint" | %12"ProfilingPrint"\n", i, AlexTime, DougTime);
	}
	for (i = 0; i < TEST_COUNT; ++i) {
		AlexTotalAvg += AlexAvgs[i];
		DougTotalAvg += DougAvgs[i];
	}

	printf("\n"
		   "Result of %"PRIuFAST16" Test Sets (of %"PRIuFAST16" Calls) with %s First:\n"
		 //"###### | ############ | ##################"
		   "Name   | Average ns   | Total ns\n"
		   "%-6s | %12"ProfilingPrint" | %12"ProfilingPrint"\n"
		   "%-6s | %12"ProfilingPrint" | %12"ProfilingPrint"\n"
		   "\n"
		   "\n"
		   , TEST_COUNT, LOOP_COUNT, First == Alex ? AlexName : DougName, 
		   AlexName, AlexTotalAvg / TEST_COUNT, AlexTotalAvg,
		   DougName, DougTotalAvg / TEST_COUNT, DougTotalAvg);
	
	if (AlexTotalAvg > DougTotalAvg) {
		AlexTotalAvg /= TEST_COUNT;
		DougTotalAvg /= TEST_COUNT;
		*TimeDiff = AlexTotalAvg - DougTotalAvg;
		//*RatioDiff = 1.0 - ((double)AlexTotalAvg / (double)DougTotalAvg);
		*RatioDiff = fma((double)AlexTotalAvg, 1.0 / (double)DougTotalAvg, -1.0 );
	} else {
		AlexTotalAvg /= TEST_COUNT;
		DougTotalAvg /= TEST_COUNT;
		*TimeDiff = DougTotalAvg - AlexTotalAvg;
		//*RatioDiff = 1.0 - ((double)DougTotalAvg / (double)AlexTotalAvg);
		*RatioDiff = fma((double)DougTotalAvg, 1.0 / (double)AlexTotalAvg, -1.0);
	}
	return AlexTotalAvg > DougTotalAvg;
}

noreturn void main_thing(int argc, char* argv[]) {

	uint_fast32_t i;
	printf("loading");
	for (i = 0; i < 50000; ++i) {
		printf(".");
	}
	printf("\n");

	char* input_file = "F:\\Users\\zero318\\Source\\Repos\\ECLTest3\\document.html";
	
	char* OriginalString;
	OriginalString = readFile(input_file);
	//OriginalString = "";

	size_t TestStringLength = strlen(OriginalString) + 1;
	char *const __restrict TestString = malloc(TestStringLength + sizeof(int));
	//char *const __restrict TestString2 = malloc(TestStringLength);
	ProfilingTime FirstDiff, SecondDiff;
	double FirstRatio = 0.0;
	double SecondRatio = 0.0;

	WhichWasFaster FirstTest, SecondTest;
	FirstTest = doTestCases(OriginalString, TestString, TestStringLength, Alex, &FirstDiff, &FirstRatio);
	SecondTest = doTestCases(OriginalString, TestString, TestStringLength, Doug, &SecondDiff, &SecondRatio);
	free(TestString);
	//free(TestString2);

	FirstDiff += SecondDiff;
	FirstDiff /= 2;
	//FirstRatio += SecondRatio;
	//FirstRatio /= 2;

	if (FirstTest == SecondTest) {
		printf("The faster function was %s by ~%"PRIuMAX" ns (%lf%%)\n\n",
			   FirstTest == Alex ? AlexName : DougName, FirstDiff, FirstRatio);
	} else {
		printf("The ordering of the tests seems to have affected the results.\n"
			   "Both times, the test that ran %s was faster by ~%"PRIuMAX" ns (%lf%%)\n\n",
			   !SecondTest ? "FIRST" : "SECOND", FirstDiff, FirstRatio);
	}
	
	exit(EXIT_SUCCESS);
}