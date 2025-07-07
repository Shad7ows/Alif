#pragma once





AlifIntT alifTraceBack_here(AlifFrameObject*); // 9
AlifIntT alifTraceBack_print(AlifObject*, AlifObject* ); // 10


 // 13
extern AlifTypeObject _alifTraceBackType_;
#define ALIFTRACEBACK_CHECK(_v) ALIF_IS_TYPE(_v, &_alifTraceBackType_)



/* ------------------------------------------------------------------------------ */




class AlifTracebackObject { // 7
public:
	ALIFOBJECT_HEAD;
	AlifTracebackObject* next{};
	AlifFrameObject* frame{};
	AlifIntT lasti{};
	AlifIntT lineno{};
};
