
#include "AlifCore_ModSupport.h"





// 16
#define SET_POP_METHODDEF    \
    {"اسحب", (AlifCPPFunction)set_pop, METHOD_NOARGS},

static AlifObject* set_popImpl(AlifSetObject*); // 19


static AlifObject* set_pop(AlifSetObject* so, AlifObject* ALIF_UNUSED(ignored)) { // 22
	AlifObject* returnValue = nullptr;

	ALIF_BEGIN_CRITICAL_SECTION(so);
	returnValue = set_popImpl(so);
	ALIF_END_CRITICAL_SECTION();

	return returnValue;
}


// 429
#define SET_ADD_METHODDEF    \
    {"اضف", (AlifCPPFunction)set_add, METHOD_O},

static AlifObject* set_addImpl(AlifSetObject*, AlifObject*);

static AlifObject* set_add(AlifSetObject* _so, AlifObject* _key) {
	AlifObject* returnValue = nullptr;

	ALIF_BEGIN_CRITICAL_SECTION(_so);
	returnValue = set_addImpl(_so, _key);
	ALIF_END_CRITICAL_SECTION();

	return returnValue;
}
