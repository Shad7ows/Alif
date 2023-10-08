#pragma once







































































































































#define ALIFObject_VAR_HEAD      AlifVarObject obBase;







class Object {
#if (defined(__GNUC__) || defined(__clang__)) \
        && !(defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L)

	__extension__
#endif
#ifdef _MSC_VER


		__pragma(warning(push))
		__pragma(warning(disable: 4201))
#endif
		union {
		AlifSizeT obRefCnt;
#if SIZEOF_VOID_P > 4
		ALIF_UINT32_T ob_refcnt_split[2];
#endif
	};
#ifdef _MSC_VER
	__pragma(warning(pop))
#endif

		//AlifTypeObject* obType;
};




class  AlifVarObject {
public:
	AlifObject obBase;
	AlifSizeT obSize; 
};








































































































typedef AlifObject * (*UnaryFunc)(AlifObject *);
typedef AlifObject* (*BinaryFunc)(AlifObject*, AlifObject*);
typedef AlifObject* (*TernaryFunc)(AlifObject*, AlifObject*, AlifObject*);
typedef int (*Inquiry)(AlifObject*);
typedef AlifSizeT(*LenFunc)(AlifObject*);
typedef AlifObject* (*SSizeArgFunc)(AlifObject*, AlifSizeT);
typedef AlifObject* (*SSizeSSizeArgSunc)(AlifObject*, AlifSizeT, AlifSizeT);
typedef int(*SSizeObjArgProc)(AlifObject*, AlifSizeT, AlifObject*);
typedef int(*SSizeSSizeObjArgProc)(AlifObject*, AlifSizeT, AlifSizeT, AlifObject*);
typedef int(*ObjObjArgProc)(AlifObject*, AlifObject*, AlifObject*);

typedef int (*ObjObjProc)(AlifObject*, AlifObject*);
typedef int (*VisitProc)(AlifObject*, void*);
typedef int (*TraverseProc)(AlifObject*, VisitProc, void*);


typedef void (*FreeFunc)(void*);
typedef void (*Destructor)(AlifObject*);
typedef AlifObject* (*GetAttrFunc)(AlifObject*, char*);
typedef AlifObject* (*GetAttroFunc)(AlifObject*, AlifObject*);
typedef int (*SetAttrFunc)(AlifObject*, char*, AlifObject*);
typedef int (*SetAttroFunc)(AlifObject*, AlifObject*, AlifObject*);
typedef AlifObject* (*ReprFunc)(AlifObject*);
typedef AlifHashT(*HashFunc)(AlifObject*);
typedef AlifObject* (*RichCmpFunc) (AlifObject*, AlifObject*, int);
typedef AlifObject* (*GetIterFunc) (AlifObject*);
typedef AlifObject* (*IterNextFunc) (AlifObject*);
typedef AlifObject* (*DescrGetFunc) (AlifObject*, AlifObject*, AlifObject*);
typedef int (*DescrSetFunc) (AlifObject*, AlifObject*, AlifObject*);
typedef int (*InitProc)(AlifObject*, AlifObject*, AlifObject*);
typedef AlifObject* (*NewFunc)(AlifTypeObject*, AlifObject*, AlifObject*);
typedef AlifObject* (*AllocFunc)(AlifTypeObject*, AlifSizeT);

#if !defined(ALIF_LIMITED_API) || ALIF_LIMITED_API+0 >= 0x030c0000 // 3.12
typedef AlifObject* (*VectorCallFunc)(AlifObject* callable, AlifObject* const* args,
	size_t nargsf, AlifObject* kwnames);
#endif

















































































































































































































































































































































































































































































































































#if !defined(Py_LIMITED_API) || Py_LIMITED_API+0 >= 0x030A0000

enum AlifSendResult {
	ALIFGEN_RETURN = 0,
	ALIFGEN_ERROR = -1,
	ALIFGEN_NEXT = 1,
};
#endif











































































#include "AlifCpp/object.h"