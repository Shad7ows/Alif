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
