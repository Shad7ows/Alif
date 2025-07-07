#include "AlifCore_CriticalSection.h"
#include "AlifCore_ModSupport.h"


#define _RANDOM_RANDOM_RANDOM_METHODDEF    \
    {"random", (AlifCPPFunction)random_RandomRandom, METHOD_NOARGS},

static AlifObject* randomRandom_randomImpl(RandomObject* ); // 18

static AlifObject* random_RandomRandom(RandomObject* _self, AlifObject* ALIF_UNUSED(ignored)) { // 21
	AlifObject* returnValue = nullptr;

	ALIF_BEGIN_CRITICAL_SECTION(_self);
	returnValue = randomRandom_randomImpl(_self);
	ALIF_END_CRITICAL_SECTION();

	return returnValue;
}
