#include "AlifCore_ModSupport.h"


















// 324
#define _IMP_IS_BUILTIN_METHODDEF    \
    {"هل_ضمني", (AlifCPPFunction)_imp_isBuiltin, METHOD_O},

static AlifObject* _imp_isBuiltinImpl(AlifObject*, AlifObject*);

static AlifObject* _imp_isBuiltin(AlifObject* _module, AlifObject* _arg) { // 330
	AlifObject* returnValue = nullptr;
	AlifObject* name{};

	if (!ALIFUSTR_CHECK(_arg)) {
		//_alifArg_badArgument("is_builtin", "argument", "str", _arg);
		goto exit;
	}
	name = _arg;
	returnValue = _imp_isBuiltinImpl(_module, name);

exit:
	return returnValue;
}

// 353
#define _IMP_IS_FROZEN_METHODDEF    \
    {"هل_مجرد", (AlifCPPFunction)_imp_isFrozen, METHOD_O},

static AlifObject* _imp_isFrozenImpl(AlifObject*, AlifObject*);

static AlifObject* _imp_isFrozen(AlifObject* _module,
	AlifObject* _arg) { // 359
	AlifObject* returnValue = nullptr;
	AlifObject* name{};

	if (!ALIFUSTR_CHECK(_arg)) {
		//_alifArg_badArgument("is_frozen", "argument", "str", _arg);
		goto exit;
	}
	name = _arg;
	returnValue = _imp_isFrozenImpl(_module, name);

exit:
	return returnValue;
}



//* alif //* delete //* todo
#define _IMP_PRINT   \
    {"اطبعي", (AlifCPPFunction)_imp_print, METHOD_O},

static AlifObject* _imp_printImpl(AlifObject*, AlifObject*);

static AlifObject* _imp_print(AlifObject* _module, AlifObject* _arg) { //* alif //* delete //* todo
	AlifObject* returnValue = nullptr;
	AlifObject* name{};

	if (!ALIFUSTR_CHECK(_arg)) {
		//_alifArg_badArgument("is_builtin", "argument", "str", _arg);
		goto exit;
	}
	name = _arg;
	returnValue = _imp_printImpl(_module, name);

exit:
	return returnValue;
}
