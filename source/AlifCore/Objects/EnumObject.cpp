#include "alif.h"
#include "AlifCore_Call.h"
#include "AlifCore_Long.h"
#include "AlifCore_ModSupport.h"
#include "AlifCore_Object.h"












/* ------------------ Reversed Object ------------------ */


class ReversedObject {
public:
	ALIFOBJECT_HEAD{};
	AlifSizeT index{};
	AlifObject* seq{};
};


static AlifObject* reversed_newImpl(AlifTypeObject* _type, AlifObject* _seq) {
	AlifSizeT n{};
	AlifObject* reversedMeth{};
	ReversedObject* ro{};

	reversedMeth = alifObject_lookupSpecial(_seq, &ALIF_STR(__reversed__));
	if (reversedMeth == ALIF_NONE) {
		ALIF_DECREF(reversedMeth);
		alifErr_format(_alifExcTypeError_,
			"'%.200s' الكائن غير قابل للعكس",
			ALIF_TYPE(_seq)->name);
		return nullptr;
	}
	if (reversedMeth != nullptr) {
		AlifObject *res = _alifObject_callNoArgs(reversedMeth);
		ALIF_DECREF(reversedMeth);
		return res;
	}
	else if (alifErr_occurred())
		return nullptr;

	if (!alifSequence_check(_seq)) {
		alifErr_format(_alifExcTypeError_,
			"'%.200s' الكائن غير قابل للعكس",
			ALIF_TYPE(_seq)->name);
		return nullptr;
	}

	n = alifSequence_size(_seq);
	if (n == -1)
		return nullptr;

	ro = (ReversedObject*)_type->alloc(_type, 0);
	if (ro == nullptr)
		return nullptr;

	ro->index = n-1;
	ro->seq = ALIF_NEWREF(_seq);
	return (AlifObject*)ro;
}


static AlifObject* reversed_vectorCall(AlifObject* _type, AlifObject* const* _args,
	AlifUSizeT _nargsf, AlifObject* _kwNames) {
	if (!_ALIFARG_NOKWNAMES("معكوس", _kwNames)) {
		return nullptr;
	}

	AlifSizeT nargs = ALIFVECTORCALL_NARGS(_nargsf);
	if (!_ALIFARG_CHECKPOSITIONAL("معكوس", nargs, 1, 1)) {
		return nullptr;
	}

	return reversed_newImpl(ALIFTYPE_CAST(_type), _args[0]);
}






static void reversed_dealloc(ReversedObject* _ro) {
	alifObject_gcUnTrack(_ro);
	ALIF_XDECREF(_ro->seq);
	ALIF_TYPE(_ro)->free(_ro);
}




static AlifObject* reversed_next(ReversedObject* _ro) {
	AlifObject* item{};
	AlifSizeT index = _ro->index;

	if (index >= 0) {
		item = alifSequence_getItem(_ro->seq, index);
		if (item != nullptr) {
			_ro->index--;
			return item;
		}
		if (alifErr_exceptionMatches(_alifExcIndexError_) or
			alifErr_exceptionMatches(_alifExcStopIteration_))
			alifErr_clear();
	}
	_ro->index = -1;
	ALIF_CLEAR(_ro->seq);
	return NULL;
}






AlifTypeObject _alifReversedType_ = {
	.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0),
	.name = "معكوس",
	.basicSize = sizeof(ReversedObject),
	/* methods */
	.dealloc = (Destructor)reversed_dealloc,
	.getAttro = alifObject_genericGetAttr,
	.flags = ALIF_TPFLAGS_DEFAULT | ALIF_TPFLAGS_HAVE_GC |
	ALIF_TPFLAGS_BASETYPE,
	//.traverse = (TraverseProc)reversed_traverse,
	.iter = alifObject_selfIter,
	.iterNext = (IterNextFunc)reversed_next,
	//.methods = _reversedIterMethods_,
	.alloc = alifType_genericAlloc,
	//.new_ = reversed_new,
	.free = alifObject_gcDel,
	.vectorCall = (VectorCallFunc)reversed_vectorCall,
};
