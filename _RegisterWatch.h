#ifndef REGISTER_WATCH_H
#define REGISTER_WATCH_H
#include <stdint.h>
#include <stdalign.h>

#if UINTPTR_MAX == 0xffffffff

typedef uint32_t Register32;

#define EAXStyleRegister(Letter)\
typedef union {\
	uint32_t Input;\
	union {\
		int32_t E ## Letter ## X;\
		int32_t Letter ## X : 16;\
		struct {\
			int32_t Letter ## L : 8;\
			int32_t Letter ## H : 8;\
		};\
	} Signed;\
	union {\
		uint32_t E ## Letter ## X;\
		uint32_t Letter ## X : 16;\
		struct {\
			uint32_t Letter ## L : 8;\
			uint32_t Letter ## H : 8;\
		};\
	} Unsigned;\
	struct {\
		char Letter ## L_Char;\
		char Letter ## H_Char;\
	};\
	char Chars[4];\
	char* str;\
	void* ptr;\
} _E ## Letter ## X;\
_E ## Letter ## X RegE ## Letter ## X;\
static const _E ## Letter ## X GetE ## Letter ## X(const unsigned int _E ## Letter ## XT) {\
	return (const _E ## Letter ## X){.Unsigned.E ## Letter ## X = _E ## Letter ## XT };\
}
#define ESPStyleRegister(Letters)\
typedef union {\
	union {\
		int32_t E ## Letters;\
		int32_t Letters : 16;\
		int32_t Letters ## L : 8;\
	} Signed;\
	union {\
		uint32_t E ## Letters;\
		uint32_t Letters : 16;\
		uint32_t Letters ## L : 8;\
	} Unsigned;\
	char Char;\
	char Chars[4];\
	char* str;\
	void* ptr;\
} _E ## Letters;\
_E ## Letters RegE ## Letters;\
static const _E ## Letters GetE ## Letters(const unsigned int _E ## Letters ## T) {\
	return (const _E ## Letters){.Unsigned.E ## Letters = _E ## Letters ## T };\
}

typedef struct _EAXTT {
	uint32_t input;
} _EAXTT;

_EAXTT eeeeeeeeee(void) {
	return (_EAXTT) {
		.input = 0
	};
}

static _EAXTT jskdbfksdbf;

EAXStyleRegister(A);
EAXStyleRegister(C);
EAXStyleRegister(D);
EAXStyleRegister(B);
ESPStyleRegister(SP);
ESPStyleRegister(BP);
ESPStyleRegister(SI);
ESPStyleRegister(DI);

void GetAllRegisters(unsigned int _EAXT, unsigned int _ECXT, unsigned int _EDXT, unsigned int _EBXT, unsigned int _ESPT, unsigned int _EBPT, unsigned int _ESIT, unsigned int _EDIT) {
	RegEAX = GetEAX(_EAXT);
	RegECX = GetECX(_ECXT);
	RegEDX = GetEDX(_EDXT);
	RegEBX = GetEBX(_EBXT);
	RegESP = GetESP(_ESPT);
	RegEBP = GetEBP(_EBPT);
	RegESI = GetESI(_ESIT);
	RegEDI = GetEDI(_EDIT);
}

#elif UINTPTR_MAX == 0xffffffffffffffff

#endif

#endif