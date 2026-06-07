#pragma once




extern AlifTypeObject _alifFrameType_; // 5
extern AlifTypeObject _alifFrameLocalsProxyType_;

#define ALIFFRAME_CHECK(op) ALIF_IS_TYPE((op), &_alifFrameType_) // 8
#define ALIFFRAMELOCALSPROXY_CHECK(op) ALIF_IS_TYPE((op), &_alifFrameLocalsProxyType_)

AlifCodeObject* alifFrame_getCode(AlifFrameObject*);





AlifIntT alifUnstable_interpreterFrameGetLine(class AlifInterpreterFrame*); // 37
