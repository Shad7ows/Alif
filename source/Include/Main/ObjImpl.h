#pragma once


AlifObject* alifSubObject_new(AlifTypeObject*);
AlifVarObject* alifSubObject_newVar(AlifTypeObject*, int64_t);

void alifObjectGC_track(void*);

void alifObjectGC_unTrack(void*);


#define ALIFOBJECT_NEW(_type, _typeObj) ((_type *)alifSubObject_new(_typeObj))

#define ALIFOBJECT_NEWVAR(_type, _typeObj, _n) \
                ( (_type *) alifSubObject_newVar((_typeObj), (_n)) )

#define ALIFOBJECT_NEW_VAR(_type, _typeObj, _n) ALIFOBJECT_NEWVAR(_type, (_typeObj), (_n))



static inline size_t alifSubObject_varSize(AlifTypeObject* _type, int64_t _nItems) {
    size_t size_ =  _type->basicSize;
    size_ +=  _nItems * _type->itemsSize;
    return ALIFSIZE_ROUND_UP(size_, SIZEOF_VOID_P);
}








/*
 * Garbage Collection
 * ==================
 */
AlifObject* alifObjectGC_new(AlifTypeObject*);

#define ALIFOBJECT_GC_NEW(_type, _typeObj) (_type*)(alifObjectGC_new(_typeObj))
