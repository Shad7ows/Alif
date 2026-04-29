#include "alif.h"

#include "AlifCore_FreeList.h"

AlifTypeObject _alifEllipsisType_ = { // 66
	.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0),
	.name = "إضمار",
	.basicSize = 0,
	.itemSize = 0,

	.getAttro = alifObject_genericGetAttr,

	.flags = ALIF_TPFLAGS_DEFAULT,
};

AlifObject _alifEllipsisObject_ = ALIFOBJECT_HEAD_INIT(&_alifEllipsisType_); // 107










static AlifSliceObject* _alifBuildSlice_consume2(AlifObject* _start,
	AlifObject* _stop, AlifObject* _step) { // 117
	AlifSliceObject* obj_ = ALIF_FREELIST_POP(AlifSliceObject, slices);
	if (obj_ == nullptr) {
		obj_ = ALIFOBJECT_GC_NEW(AlifSliceObject, &_alifSliceType_);
		if (obj_ == nullptr) {
			goto error;
		}
	}

	obj_->start = _start;
	obj_->stop = _stop;
	obj_->step = ALIF_NEWREF(_step);

	alifObject_gcTrack(obj_);
	return obj_;
error:
	ALIF_DECREF(_start);
	ALIF_DECREF(_stop);
	return nullptr;
}

AlifObject* alifSlice_new(AlifObject* _start, AlifObject* _stop, AlifObject* _step) { // 141
	if (_step == nullptr) {
		_step = ALIF_NONE;
	}
	if (_start == nullptr) {
		_start = ALIF_NONE;
	}
	if (_stop == nullptr) {
		_stop = ALIF_NONE;
	}
	return (AlifObject*)_alifBuildSlice_consume2(ALIF_NEWREF(_start),
		ALIF_NEWREF(_stop), _step);
}






AlifObject* _alifBuildSlice_consumeRefs(AlifObject* _start, AlifObject* _stop) { // 156
	return (AlifObject*)_alifBuildSlice_consume2(_start, _stop, ALIF_NONE);
}












AlifIntT alifSlice_unpack(AlifObject* _r,
	AlifSizeT* start, AlifSizeT* stop, AlifSizeT* step) { // 214
	AlifSliceObject *r = (AlifSliceObject*)_r;
	/* this is harder to get right than you might think */

	if (r->step == ALIF_NONE) {
		*step = 1;
	}
	else {
		if (!_alifEval_sliceIndex(r->step, step)) return -1;
		if (*step == 0) {
			alifErr_setString(_alifExcValueError_,
				"خطوة القطعة لا يمكن أن تكون صفر");
			return -1;
		}
		/* Here *step might be -PY_SSIZE_T_MAX-1; in this case we replace it
		* with -PY_SSIZE_T_MAX.  This doesn't affect the semantics, and it
		* guards against later undefined behaviour resulting from code that
		* does "step = -step" as part of a slice reversal.
		*/
		if (*step < -ALIF_SIZET_MAX)
			*step = -ALIF_SIZET_MAX;
	}

	if (r->start == ALIF_NONE) {
		*start = *step < 0 ? ALIF_SIZET_MAX : 0;
	}
	else {
		if (!_alifEval_sliceIndex(r->start, start)) return -1;
	}

	if (r->stop == ALIF_NONE) {
		*stop = *step < 0 ? ALIF_SIZET_MIN : ALIF_SIZET_MAX;
	}
	else {
		if (!_alifEval_sliceIndex(r->stop, stop)) return -1;
	}

	return 0;
}




AlifSizeT alifSlice_adjustIndices(AlifSizeT length,
	AlifSizeT *start, AlifSizeT *stop, AlifSizeT step) { // 260
	/* this is harder to get right than you might think */

	if (*start < 0) {
		*start += length;
		if (*start < 0) {
			*start = (step < 0) ? -1 : 0;
		}
	}
	else if (*start >= length) {
		*start = (step < 0) ? length - 1 : length;
	}

	if (*stop < 0) {
		*stop += length;
		if (*stop < 0) {
			*stop = (step < 0) ? -1 : 0;
		}
	}
	else if (*stop >= length) {
		*stop = (step < 0) ? length - 1 : length;
	}

	if (step < 0) {
		if (*stop < *start) {
			return (*start - *stop - 1) / (-step) + 1;
		}
	}
	else {
		if (*start < *stop) {
			return (*stop - *start - 1) / step + 1;
		}
	}
	return 0;
}
























AlifTypeObject _alifSliceType_ = { // 658
	.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0),
	.name = "قطع",
	.basicSize = sizeof(AlifSliceObject),
	//(Destructor)slice_dealloc,
	//(ReprFunc)slice_repr,
	//(HashFunc)slicehash,
	.getAttro = alifObject_genericGetAttr,
	.flags = ALIF_TPFLAGS_DEFAULT | ALIF_TPFLAGS_HAVE_GC,
	//(TraverseProc)slice_traverse,
	//slice_richcompare,
	//slice_methods,
	//slice_members,
	//slice_new,
};
