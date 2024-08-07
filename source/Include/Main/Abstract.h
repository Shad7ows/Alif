#pragma once



AlifObject* alifStack_asDict(AlifObject* const*, AlifObject* );

VectorCallFunc alifVectorCall_function(AlifObject*);

AlifObject* alifObject_vectorCall(AlifObject*, AlifObject* const*, AlifUSizeT, AlifObject*);

AlifObject* alifObject_vectorcallMethod(AlifObject* , AlifObject* const* , size_t , AlifObject*);

AlifObject* alifObject_callOneArg(AlifObject*, AlifObject*);

AlifObject* alifObject_callMethod(AlifObject* obj, const wchar_t* , const wchar_t* , ...) ;

AlifIntT alifMapping_getOptionalItem(AlifObject*, AlifObject*, AlifObject**);

int alifMapping_setItemString(AlifObject* o, const wchar_t* , AlifObject* );

AlifObject* alifMapping_keys(AlifObject* );

AlifObject* alifObject_getIter(AlifObject*);

AlifObject* alifObject_getItem(AlifObject* , AlifObject* );

int alifObject_setItem(AlifObject*, AlifObject*, AlifObject* );

int alifObject_delItem(AlifObject* , AlifObject* );

AlifObject* alifNumber_add(AlifObject*, AlifObject*);

AlifObject* alifNumber_subtract(AlifObject*, AlifObject*);

AlifObject* alifNumber_inPlaceAdd(AlifObject* , AlifObject* );

AlifObject* alifNumber_inPlaceSubtract(AlifObject* , AlifObject* );

AlifObject* alifInteger_inPlaceOr(AlifObject*, AlifObject*);

AlifObject* alifIter_next(AlifObject*);

AlifObject* alifInteger_float(AlifObject* );

AlifSizeT alifNumber_asSizeT(AlifObject*, AlifObject*);

AlifObject* alifSequence_getItem(AlifObject*, AlifSizeT);

AlifObject* alifSequence_list(AlifObject* );

int alifSequence_setItem(AlifObject*, int64_t , AlifObject*);

#define ALIF_VECTORCALL_ARGUMENTS_OFFSET ((size_t)1 << (8 * sizeof(size_t) - 1))

AlifObject* alifVectorCall_call(AlifObject* , AlifObject* , AlifObject* );
AlifObject* alifSequence_fast(AlifObject* , const wchar_t* );
int alifSequence_check(AlifObject* );

AlifIntT alifSequence_delItem(AlifObject*, AlifSizeT);

#define ALIFSEQUENCE_FAST_GETSIZE(o) (((AlifVarObject*)(o))->size_ ? ((AlifVarObject*)(o))->size_ : ((AlifVarObject*)(o))->size_)

#define ALIFSEQUENCE_FAST_ITEMS(sf) ((((AlifObject*)(sf))->type_ == &_alifListType_) ? ((AlifListObject *)(sf))->items_ : ((AlifTupleObject *)(sf))->items_)

static inline int64_t alifSubVectorcall_nArgs(size_t _n)
{
    return _n & ~ALIF_VECTORCALL_ARGUMENTS_OFFSET;
}
#define ALIFVECTORCALL_NARGS(_n) alifSubVectorcall_nArgs(_n)

static inline AlifObject* alifObject_callMethodNoArgs(AlifObject* self, AlifObject* name)
{
	size_t nargsf = 1 | ALIF_VECTORCALL_ARGUMENTS_OFFSET;
	return alifObject_vectorcallMethod(name, &self, nargsf, nullptr);
}
