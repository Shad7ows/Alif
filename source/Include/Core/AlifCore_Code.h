#pragma once

#include "AlifCore_StackRef.h"
#include "AlifCore_Lock.h"
#include "AlifCore_Backoff.h"


union AlifCodeUnit {
	uint16_t cache{};
	class {
	public:
		uint8_t code{};
		uint8_t arg{};
	}op;
	AlifBackoffCounter counter;
};

class AlifSuperAttrCache{
public :
	AlifBackoffCounter counter;
} ;

// this MACROS most deal with wchar_t rather than char so maybe we should use 0x0000
#define CO_FAST_HIDDEN  0x10 // example: 0x0010 or 0x1000
#define CO_FAST_LOCAL   0x20
#define CO_FAST_CELL    0x40
#define CO_FAST_FREE    0x80



static inline wchar_t alifLocals_getKind(AlifObject* kinds, AlifIntT i) {

    wchar_t* ptr = ALIFWBYTES_AS_STRING(kinds);
    return (wchar_t)(ptr[i]);
}





class AlifCodeConstructor {
public:

    /* data */
    AlifObject* fileName{};
    AlifObject* name{};
    AlifObject* qualName{};
    int flags;

    /* the code */
    AlifObject* code{};
    AlifIntT firstlineno{};
    AlifObject* lineTable{};

    /* used by the code */
    AlifObject* consts{};
    AlifObject* names{};

    /* mapping frame offsets to information */
    AlifObject* localsPlusNames{};  // Tuple of strings
    AlifObject* localsPlusKinds{};  // Bytes object, one byte per variable

    /* args (within varnames) */
    AlifIntT argCount;
    AlifIntT posOnlyArgCount;
    AlifIntT kwOnlyArgCount;

    /* needed to create the frame */
    AlifIntT stacksize;

    /* used by the eval loop */
    AlifObject* exceptionTable;
};


int alifCode_validate(class AlifCodeConstructor* );

extern AlifCodeObject* alifCode_new(AlifCodeConstructor*);





#define ENABLE_SPECIALIZATION 1


void alifSpecialize_loadSuperAttr(AlifStackRef , AlifStackRef , AlifCodeUnit* , int );




static inline uint16_t read_u16(uint16_t* p)
{
	return *p;
}




extern AlifIntT alif_getBaseOpCode(AlifCodeObject*, AlifIntT);

#define ADAPTIVE_COOLDOWN_VALUE 52
#define ADAPTIVE_COOLDOWN_BACKOFF 0

static inline AlifBackoffCounter
adaptive_counter_bits(uint16_t value, uint16_t backoff) {
	return make_backoffCounter(value, backoff);
}

static inline AlifBackoffCounter
adaptive_counter_cooldown(void) {
	return adaptive_counter_bits(ADAPTIVE_COOLDOWN_VALUE,
		ADAPTIVE_COOLDOWN_BACKOFF);
}

static inline AlifBackoffCounter adaptive_counter_backoff(AlifBackoffCounter counter) {
	return restart_backoff_counter(counter);
}