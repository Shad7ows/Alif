#include "AlifCore_ModSupport.h"









// 186
#define _IMP_FIND_FROZEN_METHODDEF    \
    {"اوجد_مجرد", ALIF_CPPFUNCTION_CAST(_imp_findFrozen), METHOD_FASTCALL|METHOD_KEYWORDS},

static AlifObject* _imp_findFrozenImpl(AlifObject*, AlifObject*, AlifIntT);

static AlifObject* _imp_findFrozen(AlifObject* module, AlifObject* const* args,
	AlifSizeT nargs, AlifObject* kwnames) { // 192
	AlifObject* returnValue = nullptr;
#if defined(ALIF_BUILD_CORE) and !defined(ALIF_BUILD_CORE_MODULE)

#define NUM_KEYWORDS 1
	static struct {
		AlifGCHead _thisIsNotUsed;
		ALIFOBJECT_VAR_HEAD;
		AlifObject* item[NUM_KEYWORDS];
	} _kwtuple = {
			.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, NUM_KEYWORDS),
			.item = { &ALIF_ID(withdata), },
	};
#undef NUM_KEYWORDS
#define KWTUPLE (&_kwtuple.objBase.objBase)

#else  // !ALIF_BUILD_CORE
#  define KWTUPLE nullptr
#endif  // !ALIF_BUILD_CORE

	static const char * const _keywords[] = {"", "مع_بيانات", nullptr};
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "اوجد_مجرد",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[2]{};
	AlifSizeT noptargs = nargs + (kwnames ? ALIFTUPLE_GET_SIZE(kwnames) : 0) - 1;
	AlifObject* name{};
	AlifIntT withdata = 0;

	args = _ALIFARG_UNPACKKEYWORDS(args, nargs, nullptr, kwnames, &_parser,
		/*minpos*/ 1, /*maxpos*/ 1, /*minkw*/ 0, /*varpos*/ 0, argsbuf);
	if (!args) {
		goto exit;
	}
	if (!ALIFUSTR_CHECK(args[0])) {
		//_alifArg_badArgument("اوجد_مجرد", "argument 1", "str", args[0]);
		goto exit;
	}
	name = args[0];
	if (!noptargs) {
		goto skip_optional_kwonly;
	}
	withdata = alifObject_isTrue(args[1]);
	if (withdata < 0) {
		goto exit;
	}
skip_optional_kwonly:
	returnValue = _imp_findFrozenImpl(module, name, withdata);

exit:
	return returnValue;
}













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
