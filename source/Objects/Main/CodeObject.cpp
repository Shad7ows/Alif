#include "alif.h"
#include "OpCode.h"

#include "AlifCore_Code.h"
#include "AlifCore_Frame.h"
#include "AlifCore_InitConfig.h"
#include "AlifCore_Interpreter.h"
#include "AlifCore_Object.h"
#include "AlifCore_OpCodeData.h"
#include "AlifCore_OpCodeUtils.h"
#include "AlifCore_AlifState.h"





















static void get_localsPlusCounts(AlifObject* _names, AlifObject* _kinds,
	AlifIntT* _pnLocals, AlifIntT* _pnCellVars, AlifIntT* _pnFreeVars) { 
	AlifIntT nlocals = 0;
	AlifIntT ncellvars = 0;
	AlifIntT nfreevars = 0;
	AlifSizeT nlocalsplus = ALIFTUPLE_GET_SIZE(_names);
	for (AlifIntT i = 0; i < nlocalsplus; i++) {
		wchar_t kind = alifLocals_getKind(_kinds, i); // <-- need review
		if (kind & CO_FAST_LOCAL) {
			nlocals += 1;
			if (kind & CO_FAST_CELL) {
				ncellvars += 1;
			}
		}
		else if (kind & CO_FAST_CELL) {
			ncellvars += 1;
		}
		else if (kind & CO_FAST_FREE) {
			nfreevars += 1;
		}
	}
	if (_pnLocals != nullptr) {
		*_pnLocals = nlocals;
	}
	if (_pnCellVars != nullptr) {
		*_pnCellVars = ncellvars;
	}
	if (_pnFreeVars != nullptr) {
		*_pnFreeVars = nfreevars;
	}
}

#define ALIF_IS_ALIGNED(p, a) (!((uintptr_t)(p) & (uintptr_t)((a) - 1)))

int alifCode_validate(class AlifCodeConstructor* con)
{
	/* Check argument types */
	if (con->argCount < con->posOnlyArgCount || con->posOnlyArgCount < 0 ||
		con->kwOnlyArgCount < 0 ||
		con->stacksize < 0 || con->flags < 0 ||
		con->code == NULL || !ALIFBYTES_CHECK(con->code) ||
		con->consts == NULL || !ALIFTUPLE_CHECK(con->consts) ||
		con->names == NULL || !ALIFTUPLE_CHECK(con->names) ||
		con->localsPlusNames == NULL || !ALIFTUPLE_CHECK(con->localsPlusNames) ||
		con->localsPlusKinds == NULL || !ALIFBYTES_CHECK(con->localsPlusKinds) ||
		ALIFTUPLE_GET_SIZE(con->localsPlusNames)
		!= ALIFWBYTES_GET_SIZE(con->localsPlusKinds) ||
		con->name == NULL || !ALIFUSTR_CHECK(con->name) ||
		con->qualName == NULL || !ALIFUSTR_CHECK(con->qualName) ||
		con->fileName == NULL || !ALIFUSTR_CHECK(con->fileName) ||
		con->lineTable == NULL || !ALIFBYTES_CHECK(con->lineTable)||
		con->exceptionTable == NULL || !ALIFBYTES_CHECK(con->exceptionTable)
		) {
		return -1;
	}

	if (ALIFWBYTES_GET_SIZE(con->code) > INT_MAX) {

		return -1;
	}
	if (ALIFWBYTES_GET_SIZE(con->code) % sizeof(AlifCodeUnit) != 0 ||
		!ALIF_IS_ALIGNED(ALIFWBYTES_AS_STRING(con->code), sizeof(AlifCodeUnit))
		) {
		return -1;
	}

	int nlocals;
	get_localsPlusCounts(con->localsPlusNames, con->localsPlusKinds,
		&nlocals, NULL, NULL);
	int nplainlocals = nlocals -
		con->argCount -
		con->kwOnlyArgCount -
		((con->flags & CO_VARARGS) != 0) -
		((con->flags & CO_VARKEYWORDS) != 0);
	//if (nplainlocals < 0) {
		//return -1;
	//}

	return 0;
}


extern void alifCode_quicken(AlifCodeObject*); 

static void init_code(AlifCodeObject* _co, AlifCodeConstructor* _cons) { 

	AlifIntT nLocalsPlus = ALIFTUPLE_GET_SIZE(_cons->localsPlusNames);
	AlifIntT nLocals{}, nCellVars{}, nFreeVars{};
	get_localsPlusCounts(_cons->localsPlusNames, _cons->localsPlusKinds, &nLocals, &nCellVars, &nFreeVars);
	if (_cons->stacksize == 0) {
		_cons->stacksize = 1;
	}

	_co->fileName = ALIF_NEWREF(_cons->fileName);
	_co->name = ALIF_NEWREF(_cons->name);
	_co->qualName = ALIF_NEWREF(_cons->qualName);

	

	_co->firstLineNo = _cons->firstlineno;
	_co->lineTable = ALIF_NEWREF(_cons->lineTable);

	_co->consts = ALIF_NEWREF(_cons->consts);
	_co->names = ALIF_NEWREF(_cons->names);

	_co->localsPlusNames = ALIF_NEWREF(_cons->localsPlusNames);
	_co->localsPlusKinds = ALIF_NEWREF(_cons->localsPlusKinds);
	
	_co->args = _cons->argCount;
	_co->posOnlyArgCount = _cons->posOnlyArgCount;
	_co->kwOnlyArgCount = _cons->kwOnlyArgCount;

	_co->stackSize = _cons->stacksize;

	_co->nLocalsPlus = nLocalsPlus;
	_co->nLocals = nLocals;
	_co->frameSize = nLocalsPlus + _cons->stacksize + FRAME_SPECIALS_SIZE;
	
	memcpy(ALIFCODE_CODE(_co), ALIFWBYTES_AS_STRING(_cons->code), ALIFWBYTES_GET_SIZE(_cons->code));
	AlifIntT entryPoint = 0;
	while (entryPoint < ALIF_SIZE(_co) 
		and 
		ALIFCODE_CODE(_co)[entryPoint].op.code != RESUME)
	{
		entryPoint++;
	}
	_co->firstTraceable = entryPoint;
	alifCode_quicken(_co);
}

static AlifIntT internCode_constants(AlifCodeConstructor* _cons) { 
	
	//if (intern_string(_cons->names) < 0) goto error;
	//if (intern_constants(_cons->consts, nullptr) < 0) goto error;
	//if (intern_string(_cons->localsPlusNames) < 0) goto error;
	
	return 0;

error:
	return -1;
}


AlifCodeObject* alifCode_new(AlifCodeConstructor* _cons) { 

	if (internCode_constants(_cons) < 0) return nullptr;

	AlifObject* replacementLocations = nullptr;

	AlifSizeT size = ALIFWBYTES_GET_SIZE(_cons->code) / sizeof(AlifCodeUnit);
	AlifCodeObject* co = ALIFOBJECT_NEW_VAR(AlifCodeObject, &_alifCodeType_, size);
	if (co == nullptr) {
		ALIF_XDECREF(replacementLocations);
		// memory error
		return nullptr;
	}
	init_code(co, _cons);

	ALIF_XDECREF(replacementLocations);
	return co;
}











AlifTypeObject _alifCodeType_ = {
	ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0)
	L"شفرة",
	offsetof(AlifCodeObject, codeAdaptive),
	sizeof(AlifCodeUnit),






};
