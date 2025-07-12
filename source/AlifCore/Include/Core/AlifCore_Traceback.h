#pragma once


















AlifObject* _alifTraceBack_fromFrame(AlifObject* , AlifFrameObject* ); // 89


#define EXCEPTION_TB_HEADER "التتبع العكسي للخطأ (المعروض اولا مستدعى اخرا):\n"
#define EXCEPTION_GROUP_TB_HEADER "خلل مجموعة التتبع العكسي للخطأ (المعروض اولا مستدعى اخرا):\n"



extern AlifIntT _alifTraceBack_print(AlifObject*, const char*, AlifObject*); // 98
