#include "alif.h"

#include "AlifCore_Eval.h"
#include "AlifCore_Code.h"
#include "AlifCore_Dict.h"
#include "AlifCore_Function.h"
#include "AlifCore_ModuleObject.h"
#include "AlifCore_Object.h"
#include "AlifCore_OpcodeMetaData.h"

#include "FrameObject.h"
#include "AlifCore_Frame.h"
#include "Opcode.h"



static AlifObject* frameLocalsProxy_getVal(AlifInterpreterFrame* frame,
	AlifCodeObject *co, AlifIntT i) { // 23
	AlifStackRef *fast = _alifFrame_getLocalsArray(frame);
	AlifLocalsKind kind = _alifLocals_getKind(co->localsPlusKinds, i);

	AlifObject* value = alifStackRef_asAlifObjectBorrow(fast[i]);
	AlifObject* cell = nullptr;

	if (value == nullptr) {
		return nullptr;
	}

	if (kind == CO_FAST_FREE or kind & CO_FAST_CELL) {
		cell = value;
	}

	if (cell != nullptr) {
		value = ALIFCELL_GET(cell);
	}

	if (value == nullptr) {
		return nullptr;
	}

	return value;
}











static AlifObject* frameLocalsProxy_new(AlifTypeObject* _type,
	AlifObject* _args, AlifObject* _kwds) { // 320
	if (ALIFTUPLE_GET_SIZE(_args) != 1) {
		alifErr_format(_alifExcTypeError_,
			"وثيقة_إطار_القيم_المحلية تتوقع معامل واحد, ولكن تم تمرير %zd",
			ALIFTUPLE_GET_SIZE(_args));
		return nullptr;
	}
	AlifObject *item = ALIFTUPLE_GET_ITEM(_args, 0);

	if (!ALIFFRAME_CHECK(item)) {
		alifErr_format(_alifExcTypeError_, "يتوقع تمرير إطار, وليس %T", item);
		return nullptr;
	}
	AlifFrameObject* frame = (AlifFrameObject*)item;

	if (_kwds != nullptr and alifDict_size(_kwds) != 0) {
		alifErr_setString(_alifExcTypeError_,
			"وثيقة_إطار_القيم_المحلية لا تأخذ معاملات مفتاحية");
		return 0;
	}

	AlifFrameLocalsProxyObject* self = (AlifFrameLocalsProxyObject*)_type->alloc(_type, 0);
	if (self == nullptr) {
		return nullptr;
	}

	((AlifFrameLocalsProxyObject*)self)->frame = (AlifFrameObject*)ALIF_NEWREF(frame);

	return (AlifObject *)self;
}












AlifTypeObject _alifFrameLocalsProxyType_ = { // 816
	.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0),
	.name = "وثيقة_إطار_القيم_المحلية",
	.basicSize = sizeof(AlifFrameLocalsProxyObject),
	//.dealloc = (Destructor)framelocalsproxy_dealloc,
	//.repr = &frameLocalsProxy_repr,
	//.asNumber = &_frameLocalsProxyAsNumber_,
	//.asSequence = &_frameLocalsProxyAsSequence_,
	//.asMapping = &_frameLocalsProxyAsMapping_,
	.getAttro = alifObject_genericGetAttr,
	.setAttro = alifObject_genericSetAttr,
	.flags = ALIF_TPFLAGS_DEFAULT | ALIF_TPFLAGS_HAVE_GC | ALIF_TPFLAGS_MAPPING,
	//.traverse = frameLocalsProxy_visit,
	//.clear = frameLocalsProxy_tpClear,
	//.richCompare = frameLocalsProxy_richCompare,
	//.iter = frameLocalsProxy_iter,
	//.methods = _frameLocalsProxyMethods_,
	.alloc = alifType_genericAlloc,
	.new_ = frameLocalsProxy_new,
	.free = alifObject_gcDel,
};

AlifObject* _alifFrameLocalsProxy_new(AlifFrameObject* _frame) { // 838
	AlifObject* args = alifTuple_pack(1, _frame);
	if (args == nullptr) {
		return nullptr;
	}

	AlifObject* proxy = (AlifObject*)frameLocalsProxy_new(&_alifFrameLocalsProxyType_, args, nullptr);
	ALIF_DECREF(args);
	return proxy;
}













AlifTypeObject _alifFrameType_ = { // 1758
	.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTypeType_, 0),
	.name = "إطار",
	.basicSize = offsetof(AlifFrameObject, frameData) +
	offsetof(AlifInterpreterFrame, localsPlus),
	.itemSize = sizeof(AlifObject*),
	//.dealloc = (Destructor)frame_dealloc,
	//(ReprFunc)frame_repr,
	.getAttro = alifObject_genericGetAttr,
	.setAttro = alifObject_genericSetAttr,
	.flags = ALIF_TPFLAGS_DEFAULT | ALIF_TPFLAGS_HAVE_GC,
	//(TraverseProc)frame_traverse,
	//(Inquiry)frame_tpClear,
	//frame_memberList,
	//frame_getSetList,
};





AlifFrameObject* _alifFrame_newNoTrack(AlifCodeObject* _code) { // 1802
	AlifIntT slots = _code->nLocalsPlus + _code->stackSize;
	AlifFrameObject* f = ALIFOBJECT_GC_NEWVAR(AlifFrameObject, &_alifFrameType_, slots);
	if (f == nullptr) {
		return nullptr;
	}
	f->back = nullptr;
	f->trace = nullptr;
	f->traceLines = 1;
	f->traceOpcodes = 0;
	f->lineno = 0;
	f->extraLocals = nullptr;
	f->localsCache = nullptr;
	return f;
}



bool _alifFrame_hasHiddenLocals(AlifInterpreterFrame* _frame) { // 2009
	AlifCodeObject* co = _alifFrame_getCode(_frame);

	for (AlifIntT i = 0; i < co->nLocalsPlus; i++) {
		AlifLocalsKind kind = _alifLocals_getKind(co->localsPlusKinds, i);

		if (kind & CO_FAST_HIDDEN) {
			AlifObject* value = frameLocalsProxy_getVal(_frame, co, i);

			if (value != nullptr) {
				return true;
			}
		}
	}

	return false;
}

AlifObject* _alifFrame_getLocals(AlifInterpreterFrame* _frame) { // 2034
	AlifCodeObject* co = _alifFrame_getCode(_frame);

	if (!(co->flags & CO_OPTIMIZED) and !_alifFrame_hasHiddenLocals(_frame)) {
		if (_frame->locals == nullptr) {
			_frame->locals = alifDict_new();
			if (_frame->locals == nullptr) {
				return nullptr;
			}
		}
		return ALIF_NEWREF(_frame->locals);
	}

	AlifFrameObject* f = _alifFrame_getFrameObject(_frame);

	return _alifFrameLocalsProxy_new(f);
}


AlifCodeObject* alifFrame_getCode(AlifFrameObject* _frame) { // 2074
	AlifCodeObject* code = _alifFrame_getCode(_frame->frame);
	return (AlifCodeObject*)ALIF_NEWREF(code);
}



