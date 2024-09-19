#include "alif.h"

#include "AlifCore_BiaseRefCount.h"
#include "AlifCore_Eval.h"
#include "AlifCore_Dict.h"
#include "AlifCore_FreeList.h"
#include "AlifCore_InitConfig.h"
#include "AlifCore_Object.h"
#include "AlifCore_State.h"





void alif_decRefSharedDebug(AlifObject* _obj,
	const char* _filename, AlifIntT _lineno) { // 322
	AlifIntT shouldQueue;

	AlifSizeT newShared;
	AlifSizeT shared = alifAtomic_loadSizeRelaxed(&_obj->refShared);
	do {
		shouldQueue = (shared == 0 or shared == ALIF_REF_MAYBE_WEAKREF);

		if (shouldQueue) {
			newShared = ALIF_REF_QUEUED;
		}
		else {
			newShared = shared - (1 << ALIF_REF_SHARED_SHIFT);
		}

	} while (!alifAtomic_compareExchangeSize(&_obj->refShared,
		&shared, newShared));

	if (shouldQueue) {
		alifBRC_queueObject(_obj);
	}
	else if (newShared == ALIF_REF_MERGED) {
		// refcount is zero AND merged
		alif_dealloc(_obj);
	}
}



void alif_decRefShared(AlifObject* _o) { // 368
	alif_decRefSharedDebug(_o, nullptr, 0);
}


void alif_mergeZeroLocalRefcount(AlifObject* _op) { // 374

	AlifSizeT shared = alifAtomic_loadSizeAcquire(&_op->refShared);
	if (shared == 0) {
		alif_dealloc(_op);
		return;
	}

	alifAtomic_storeUintptrRelaxed(&_op->threadID, 0);

	AlifSizeT newShared{};
	do {
		newShared = (shared & ~ALIFREF_SHARED_FLAG_MASK) | ALIF_REF_MERGED;
	} while (!alifAtomic_compareExchangeSize(&_op->refShared,
		&shared, newShared));

	if (newShared == ALIF_REF_MERGED) {
		alif_dealloc(_op);
	}
}

AlifObject* alifObject_init(AlifObject* _op, AlifTypeObject* _tp) { // 437
	if (_op == nullptr) {
		//return alifErr_noMemory();
		return nullptr;
	}

	_alifObject_init(_op, _tp);
	return _op;
}

AlifVarObject* alifObject_initVar(AlifVarObject* _op,
	AlifTypeObject* _tp, AlifSizeT _size) { // 448
	if (_op == nullptr) {
		//return (AlifVarObject*)alifErr_noMemory();
		return nullptr;
	}

	_alifObject_initVar(_op, _tp, _size);
	return _op;
}


AlifObject* alifObject_new(AlifTypeObject* _tp) { // 459
	AlifObject* op = (AlifObject*)alifMem_objAlloc(alifObject_size(_tp));
	if (op == nullptr) {
		//return alifErr_noMemory();
		return nullptr; // temp
	}
	_alifObject_init(op, _tp);
	return op;
}




AlifIntT _alifSwappedOp_[] = { ALIF_GT, ALIF_GE, ALIF_EQ, ALIF_NE, ALIF_LT, ALIF_LE }; // 953

static const char* const _opStrings_[] = { "<", "<=", "==", "!=", ">", ">=" }; // 955

static AlifObject* do_richCompare(AlifThread* _thread,
	AlifObject* _v, AlifObject* _w, AlifIntT _op) { // 959
	RichCmpFunc f_{};
	AlifObject* res_{};
	AlifIntT checkedReverseOp = 0;

	if (!ALIF_IS_TYPE(_v, ALIF_TYPE(_w)) and
		alifType_isSubType(ALIF_TYPE(_w), ALIF_TYPE(_v)) and
		(f_ = ALIF_TYPE(_w)->richCompare) != nullptr) {
		checkedReverseOp = 1;
		res_ = (*f_)(_w, _v, _alifSwappedOp_[_op]);
		if (res_ != ALIF_NOTIMPLEMENTED)
			return res_;
		ALIF_DECREF(res_);
	}
	if ((f_ = ALIF_TYPE(_v)->richCompare) != nullptr) {
		res_ = (*f_)(_v, _w, _op);
		if (res_ != ALIF_NOTIMPLEMENTED)
			return res_;
		ALIF_DECREF(res_);
	}
	if (!checkedReverseOp and (f_ = ALIF_TYPE(_w)->richCompare) != nullptr) {
		res_ = (*f_)(_w, _v, _alifSwappedOp_[_op]);
		if (res_ != ALIF_NOTIMPLEMENTED)
			return res_;
		ALIF_DECREF(res_);
	}

	switch (_op) {
	case ALIF_EQ:
		res_ = (_v == _w) ? ALIF_TRUE : ALIF_FALSE;
		break;
	case ALIF_NE:
		res_ = (_v != _w) ? ALIF_TRUE : ALIF_FALSE;
		break;
	default:
		//alifErr_format(_thread, _alifExctypeError_,
		//	"'%s' ليس مدعوم بين حالات من '%.100 s' و '%.100 s'",
		//	_opStrings_[_op],
		//	ALIF_TYPE(_v)->name, ALIF_TYPE(_w)->name);
		return nullptr;
	}
	return ALIF_NEWREF(res_);
}

AlifObject* alifObject_richCompare(AlifObject* _v, AlifObject* _w, AlifIntT _op) { // 1011
	AlifThread* thread = alifThread_get();

	if (_v == nullptr or _w == nullptr) {
		//if (!alifErr_occurred(thread)) {
		//	ALIFERR_BADINTERNALCALLl();
		//}
		return nullptr;
	}
	if (alif_enterRecursiveCallTstate(thread, " في المقارنة")) {
		return nullptr;
	}
	AlifObject* res_ = do_richCompare(thread, _v, _w, _op);

	alif_leaveRecursiveCallTstate(thread);
	return res_;
}


AlifIntT alifObject_richCompareBool(AlifObject* _v, AlifObject* _w, AlifIntT _op) { // 1033
	AlifObject* res_;
	AlifIntT ok_;
	if (_v == _w) {
		if (_op == ALIF_EQ)
			return 1;
		else if (_op == ALIF_NE)
			return 0;
	}

	res_ = alifObject_richCompare(_v, _w, _op);
	if (res_ == nullptr)
		return -1;
	if (ALIFBOOL_CHECK(res_))
		ok_ = (res_ == ALIF_TRUE);
	else
		ok_ = alifObject_isTrue(res_);
	ALIF_DECREF(res_);
	return ok_;
}

AlifHashT alifObject_hashNotImplemented(AlifObject* _v) { // 1059
	//alifErrFormat(_alifExcTypeError_, "unhashable type: '%.200s'",
	//	ALIF_TYPE(_v)->name);
	return -1;
}

AlifHashT alifObject_hash(AlifObject* _v) { // 1067
	AlifTypeObject* tp_ = ALIF_TYPE(_v);
	if (tp_->hash != nullptr)
		return (*tp_->hash)(_v);
	if (!alifType_isReady(tp_)) {
		if (alifType_ready(tp_) < 0)
			return -1;
		if (tp_->hash != nullptr)
			return (*tp_->hash)(_v);
	}
	return alifObject_hashNotImplemented(_v);
}


AlifIntT alifObject_setAttrString(AlifObject* _v, const char* _name, AlifObject* _w) { // 1126
	AlifObject* s_{};
	AlifIntT res_{};

	if (ALIF_TYPE(_v)->setAttr != nullptr)
		return (*ALIF_TYPE(_v)->setAttr)(_v, (char*)_name, _w);
	s_ = alifUStr_internFromString(_name);
	if (s_ == nullptr) return -1;
	res_ = alifObject_setAttr(_v, s_, _w);
	ALIF_XDECREF(s_);
	return res_;
}

AlifIntT alifObject_getOptionalAttr(AlifObject* _v, AlifObject* _name, AlifObject** _result) { // 1238
	AlifTypeObject* tp_ = ALIF_TYPE(_v);

	if (!ALIFUSTR_CHECK(_name)) {
		//alifErr_format(_alifExcTypeError_,
			//"attribute name must be string, not '%.200s'",
			//ALIF_TYPE(_name)->name);
		*_result = nullptr;
		return -1;
	}

	if (tp_->getAttro == alifObject_genericGetAttr) {
		*_result = alifObject_genericGetAttrWithDict(_v, _name, nullptr, 1);
		if (*_result != nullptr) {
			return 1;
		}
		//if (alifErr_occurred()) {
			//return -1;
		//}
		return 0;
	}
	if (tp_->getAttro == alifType_getattro) {
		int suppressMissingAttributeException = 0;
		*_result = alifType_getAttroImpl((AlifTypeObject*)_v, _name, &suppressMissingAttributeException);
		if (suppressMissingAttributeException) {
			return 0;
		}
	}
	else if (tp_->getAttro == (GetAttroFunc)alifModule_getAttro) {
		*_result = alifModule_getAttroImpl((AlifModuleObject*)_v, _name, 1);
		if (*_result != nullptr) {
			return 1;
		}
		//if (alifErr_occurred()) {
			//return -1;
		//}
		return 0;
	}
	else if (tp_->getAttro != nullptr) {
		*_result = (*tp_->getAttro)(_v, _name);
	}
	else if (tp_->getAttr != nullptr) {
		const char* nameStr = alifUStr_asUTF8(_name);
		if (nameStr == nullptr) {
			*_result = nullptr;
			return -1;
		}
		*_result = (*tp_->getAttr)(_v, (char*)nameStr);
	}
	else {
		*_result = nullptr;
		return 0;
	}

	if (*_result != nullptr) {
		return 1;
	}
	//if (!alifErr_exceptionMatches(_alifExcAttributeError_)) {
		//return -1;
	//}
	//alifErr_clear();
	return 0;
}


AlifIntT alifObject_setAttr(AlifObject* _v, AlifObject* _name, AlifObject* _value) { // 1354
	AlifTypeObject* tp_ = ALIF_TYPE(_v);
	AlifIntT err{};

	if (!ALIFUSTR_CHECK(_name)) {
		//alifErr_format(_alifExcTypeError_,
			//"attribute name must be string, not '%.200s'",
			//ALIF_TYPE(_name)->name);
		return -1;
	}
	ALIF_INCREF(_name);

	AlifInterpreter* interp = alifInterpreter_get();
	alifUStr_internMortal(interp, &_name);
	if (tp_->setAttro != nullptr) {
		err = (*tp_->setAttro)(_v, _name, _value);
		ALIF_DECREF(_name);
		return err;
	}
	if (tp_->setAttr != nullptr) {
		const char* nameStr = alifUStr_asUTF8(_name);
		if (nameStr == nullptr) {
			ALIF_DECREF(_name);
			return -1;
		}
		err = (*tp_->setAttr)(_v, (char*)nameStr, _value);
		ALIF_DECREF(_name);
		return err;
	}
	ALIF_DECREF(_name);
	if (tp_->getAttr == nullptr and tp_->getAttro == nullptr) {
		//alfiErr_format(_alifExcTypeError_,
			//"'%.100s' object has no attributes "
			//"(%s .%U)", tp_->name,
			//_value == nullptr ? "del" : "assign to", _name);
	}
	else {
		//alifErr_format(_alifExcTypeError_,
			//"'%.100s' object has only read-only attributes "
			//"(%s .%U)", tp_->name,
			//_value == nullptr ? "del" : "assign to", _name);
	}
	return -1;
}

AlifIntT alifObject_delAttr(AlifObject* _v, AlifObject* _name) { // 1404
	return alifObject_setAttr(_v, _name, nullptr);
}

AlifIntT alifObject_isTrue(AlifObject* _v) { // 1845
	AlifSizeT res_{};
	if (_v == ALIF_TRUE)
		return 1;
	if (_v == ALIF_FALSE)
		return 0;
	if (_v == ALIF_NONE)
		return 0;
	else if (ALIF_TYPE(_v)->asNumber != nullptr and
		ALIF_TYPE(_v)->asNumber->bool_ != nullptr)
		res_ = (*ALIF_TYPE(_v)->asNumber->bool_)(_v);
	else if (ALIF_TYPE(_v)->asMapping != nullptr and
		ALIF_TYPE(_v)->asMapping->length != nullptr)
		res_ = (*ALIF_TYPE(_v)->asMapping->length)(_v);
	else if (ALIF_TYPE(_v)->asSequence != nullptr and
		ALIF_TYPE(_v)->asSequence->length != nullptr)
		res_ = (*ALIF_TYPE(_v)->asSequence->length)(_v);
	else
		return 1;
	return (res_ > 0) ? 1 : ALIF_SAFE_DOWNCAST(res_, AlifSizeT, AlifIntT);
}

AlifObject* alifObject_genericGetAttrWithDict(AlifObject* _obj, AlifObject* _name,
	AlifObject* _dict, AlifIntT _suppress) { // 1587

	AlifTypeObject* tp = ALIF_TYPE(_obj);
	AlifObject* descr = nullptr;
	AlifObject* res = nullptr;
	DescrGetFunc f;

	if (!ALIFUSTR_CHECK(_name)) {
		//alifErr_format(_alifExcTypeError_,
			//"attribute name must be string, not '%.200s'",
			//ALIF_TYPE(name)->tname);
		return nullptr;
	}
	ALIF_INCREF(_name);

	if (!alifType_isReady(tp)) {
		if (alifType_ready(tp) < 0)
			goto done;
	}

	descr = alifType_lookupRef(tp, _name);

	f = nullptr;
	if (descr != nullptr) {
		f = ALIF_TYPE(descr)->descrGet;
		if (f != nullptr and alifDescr_isData(descr)) {
			res = f(descr, _obj, (AlifObject*)ALIF_TYPE(_obj));
			//if (res == nullptr and suppress and
				//alifErr_exceptionMatches(alifExcAttributeError)) {
				//alifErr_Clear();
			//}
			goto done;
		}
	}
	if (_dict == nullptr) {
		if ((tp->flags & ALIF_TPFLAGS_INLINE_VALUES)) {
			if (ALIFUSTR_CHECKEXACT(_name) and
				alifObject_tryGetInstanceAttribute(_obj, _name, &res)) {
				if (res != nullptr) {
					goto done;
				}
			}
			else {
				dict = (AlifObject*)alifObject_materializeManagedDict(obj);
				if (dict == nullptr) {
					res = nullptr;
					goto done;
				}
			}
		}
		else if ((tp->flags & ALIF_TPFLAGS_MANAGED_DICT)) {
			dict = (AlifObject*)alifObject_getManagedDict(obj);
		}
		else {
			AlifObject** dictptr = alifObject_computedDictPointer(obj);
			if (dictptr) {
				dict = *dictptr;
			}
		}
	}
	if (dict != nullptr) {
		ALIF_INCREF(dict);
		int rc = alifDict_getItemRef(dict, _name, &res);
		ALIF_DECREF(dict);
		if (res != nullptr) {
			goto done;
		}
		else if (rc < 0) {
			//if (suppress and alifErr_exceptionMatches(alifExcAttributeError)) {
				//alifErr_clear();
			//}
			//else {
				goto done;
			//}
		}
	}

	if (f != nullptr) {
		res = f(descr, _obj, (AlifObject*)ALIF_TYPE(_obj));
		//if (res == nullptr and suppress and
			//alifErr_exceptionMatches(alifExAttributeError)) {
			//alifErr_clear();
		//}
		goto done;
	}

	if (descr != nullptr) {
		res = descr;
		descr = nullptr;
		goto done;
	}

	//if (!suppress) {
		//alifErr_format(_alifExcAttributeError_,
			//"'%.100s' object has no attribute '%U'",
			//tp->name, name);

		//alifObject_setAttributeErrorContext(_obj, _name);
	//}
done:
	ALIF_XDECREF(descr);
	ALIF_DECREF(name);
	return res;
}

AlifObject* alifObject_genericGetAttr(AlifObject* _obj, AlifObject* _name)
{
	return alifObject_genericGetAttrWithDict(_obj, _name, nullptr, 0);
}

AlifTypeObject _alifNoneType_ = { // 2049
	.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0),
	.name = "عدم",
	.basicSize = 0,
	.itemSize = 0,
};

AlifObject _alifNoneClass_ = ALIFOBJECT_HEAD_INIT(&_alifNoneType_); // 2090

AlifTypeObject _alifNotImplementedType_ = { // 2149
	.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0),
	.name = "نوع_غير_مطبق",
	.basicSize = 0,
	.itemSize = 0,
};

AlifObject _alifNotImplementedClass_ = ALIFOBJECT_HEAD_INIT(&_alifNotImplementedType_); // 2190


static inline void new_reference(AlifObject* _op) { // 2405
	_op->threadID = alif_threadID();
	_op->padding = 0;
	_op->mutex = {.bits = 0};
	_op->gcBits = 0;
	_op->refLocal = 1;
	_op->refShared = 0;

	RefTracerDureRunState* tracer = &_alifDureRun_.refTracer;
	if (tracer->tracerFunc != nullptr) {
		void* data = tracer->tracerData;
		tracer->tracerFunc(_op, AlifRefTracerEvent_::Alif_RefTracer_Create, data);
	}
}

void alif_newReference(AlifObject* _op) { // 2429
	new_reference(_op);
}



void alif_newReferenceNoTotal(AlifObject* _op) { //2438
	new_reference(_op);
}

void alif_setImmortalUntracked(AlifObject* _op) { // 2444

	_op->threadID = ALIF_UNOWNED_TID;
	_op->refLocal = ALIF_IMMORTAL_REFCNT_LOCAL;
	_op->refShared = 0;
}

void alif_setImmortal(AlifObject* _op) { // 2463
	if (alifObject_isGC(_op) and ALIFOBJECT_GC_IS_TRACKED(_op)) {
		ALIFOBJECT_GC_UNTRACK(_op);
	}
	alif_setImmortalUntracked(_op);
}

void alifObject_setDeferredRefcount(AlifObject* _op) { // 2472
#ifdef ALIF_GIL_DISABLED
	alifObject_setGCBits(_op, ALIFGC_BITS_DEFERRED);
	_op->refShared = ALIF_REF_SHARED(ALIF_REF_DEFERRED, 0);
#endif
}



void alif_dealloc(AlifObject* _op) { // 2868
	AlifTypeObject* type = ALIF_TYPE(_op);
	Destructor dealloc = type->dealloc;

	RefTracerDureRunState* tracer = &_alifDureRun_.refTracer;
	if (tracer->tracerFunc != nullptr) {
		void* data = tracer->tracerData;
		tracer->tracerFunc(_op, AlifRefTracerEvent_::Alif_RefTracer_Destroy, data);
	}

	(*dealloc)(_op);
}
