


#include "AlifCore_ModSupport.h"






#define BUILTIN___IMPORT___METHODDEF    \
    {"__استورد__", ALIF_CPPFUNCTION_CAST(builtin___import__), METHOD_FASTCALL | METHOD_KEYWORDS}

static AlifObject* builtin___import__Impl(AlifObject*, AlifObject*, AlifObject*,
	AlifObject*, AlifObject*, AlifIntT); // 35


static AlifObject* builtin___import__(AlifObject* _module, AlifObject* const* _args,
	AlifSizeT _nargs, AlifObject* _kwNames) { // 39
	AlifObject* returnValue = nullptr;

#define NUM_KEYWORDS 5
	static class {
	public:
		AlifGCHead thisIsNotUsed{};
		ALIFOBJECT_VAR_HEAD;
		AlifObject* item[NUM_KEYWORDS]{};
	} _kwtuple = {
		.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, NUM_KEYWORDS),
		//.item = { &ALIF_ID(Name), &ALIF_ID(Globals), &ALIF_ID(Locals), &ALIF_ID(Fromlist), &ALIF_ID(Level), },
	};
#undef NUM_KEYWORDS
#define KWTUPLE (&_kwtuple.objBase.objBase)

	static const char* const _keywords[] = { "name", "globals", "locals", "fromlist", "level", nullptr };
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "__import__",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[5];
	AlifSizeT noptargs = _nargs + (_kwNames ? ALIFTUPLE_GET_SIZE(_kwNames) : 0) - 1;
	AlifObject* name;
	AlifObject* globals = nullptr;
	AlifObject* locals = nullptr;
	AlifObject* fromlist = nullptr;
	AlifIntT level = 0;

	_args = _ALIFARG_UNPACKKEYWORDS(_args, _nargs, nullptr, _kwNames, &_parser,
		/*minpos*/ 1, /*maxpos*/ 5, /*minkw*/ 0, /*varpos*/ 0, argsbuf);
	if (!_args) {
		goto exit;
	}
	name = _args[0];
	if (!noptargs) {
		goto skip_optional_pos;
	}
	if (_args[1]) {
		globals = _args[1];
		if (!--noptargs) {
			goto skip_optional_pos;
		}
	}
	if (_args[2]) {
		locals = _args[2];
		if (!--noptargs) {
			goto skip_optional_pos;
		}
	}
	if (_args[3]) {
		fromlist = _args[3];
		if (!--noptargs) {
			goto skip_optional_pos;
		}
	}
	level = alifLong_asInt(_args[4]);
	if (level == -1 and alifErr_occurred()) {
		goto exit;
	}
skip_optional_pos:
	returnValue = builtin___import__Impl(_module, name, globals, locals, fromlist, level);

exit:
	return returnValue;
}



#define BUILTIN_ALL_METHODDEF    \
    {"تحقق_الكل", (AlifCPPFunction)builtin_all, METHOD_O} // 131

#define BUILTIN_ANY_METHODDEF    \
    {"تحقق_اي", (AlifCPPFunction)builtin_any, METHOD_O} // 141




#define BUILTIN_LEN_METHODDEF {"طول", (AlifCPPFunction)builtin_len, METHOD_O} // 771









#define BUILTIN_PRINT_METHODDEF {"اطبع", ALIF_CPPFUNCTION_CAST(builtin_print), METHOD_FASTCALL | METHOD_KEYWORDS} // 900



static AlifObject* builtin_printImpl(AlifObject*, AlifObject* const*,
	AlifSizeT, AlifObject*, AlifObject*, AlifObject*, AlifIntT); // 904

static AlifObject* builtin_print(AlifObject* _module, AlifObject* const* _args,
	AlifSizeT _nargs, AlifObject* _kwnames) { // 907

	AlifObject* returnValue = nullptr;
#define NUM_KEYWORDS 4
	static class {
	public:
		AlifGCHead thisNotUsed{};
		ALIFOBJECT_VAR_HEAD;
		AlifObject* item[NUM_KEYWORDS]{};
	} _kwtuple = {
		.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, NUM_KEYWORDS),
		.item = { &ALIF_STR(Sep), &ALIF_STR(End), &ALIF_ID(File), &ALIF_STR(Flush), },
	};
#undef NUM_KEYWORDS
#define KWTUPLE (&_kwtuple.objBase.objBase)


	static const char* const keywords[] = { "الفاصل", "النهاية", "File", "مباشر", nullptr };
	static AlifArgParser _parser = {
		.keywords = keywords,
		.fname = "اطبع",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[4]{};
	AlifObject* const* fastargs{};
	AlifSizeT noptargs = 0 + (_kwnames ? ALIFTUPLE_GET_SIZE(_kwnames) : 0) - 0;
	AlifObject* const* __clinicArgs{};
	AlifSizeT argsLength{};
	AlifObject* sep = ALIF_NONE;
	AlifObject* end = ALIF_NONE;
	AlifObject* file = ALIF_NONE;
	AlifIntT flush = 0;

	fastargs = _ALIFARG_UNPACKKEYWORDS(_args, _nargs, nullptr, _kwnames, &_parser,
		/*minpos*/ 0, /*maxpos*/ 0, /*minkw*/ 0, /*varpos*/ 1, argsbuf);
	if (!fastargs) {
		goto exit;
	}
	if (!noptargs) {
		goto skip_optional_kwonly;
	}
	if (fastargs[0]) {
		sep = fastargs[0];
		if (!--noptargs) {
			goto skip_optional_kwonly;
		}
	}
	if (fastargs[1]) {
		end = fastargs[1];
		if (!--noptargs) {
			goto skip_optional_kwonly;
		}
	}
	if (fastargs[2]) {
		file = fastargs[2];
		if (!--noptargs) {
			goto skip_optional_kwonly;
		}
	}
	flush = alifObject_isTrue(fastargs[3]);
	if (flush < 0) {
		goto exit;
	}
skip_optional_kwonly:
	__clinicArgs = _args;
	argsLength = _nargs;
	returnValue = builtin_printImpl(_module, __clinicArgs, argsLength, sep, end, file, flush);

exit:
	return returnValue;
}






// 994
#define BUILTIN_INPUT_METHODDEF    \
    {"ادخل", ALIF_CPPFUNCTION_CAST(builtin_input), METHOD_FASTCALL /*, builtin_input__doc__*/}

static AlifObject* builtin_inputImpl(AlifObject*, AlifObject*); // 997

static AlifObject* builtin_input(AlifObject* _module,
	AlifObject* const* _args, AlifSizeT _nargs) { // 1000
	AlifObject* return_value = nullptr;
	AlifObject* prompt = nullptr;

	if (!_ALIFARG_CHECKPOSITIONAL("ادخل", _nargs, 0, 1)) {
		goto exit;
	}
	if (_nargs < 1) {
		goto skip_optional;
	}
	prompt = _args[0];
skip_optional:
	return_value = builtin_inputImpl(_module, prompt);

exit:
	return return_value;
}


// 1106
#define BUILTIN_SUM_METHODDEF    \
    {"اجمع", ALIF_CPPFUNCTION_CAST(builtin_sum), METHOD_FASTCALL|METHOD_KEYWORDS}

static AlifObject* builtin_sumImpl(AlifObject*, AlifObject*, AlifObject*);

static AlifObject* builtin_sum(AlifObject* _module, AlifObject *const* _args,
	AlifSizeT _nargs, AlifObject* _kwNames) { // 1112
	AlifObject* return_value = nullptr;
#if defined(ALIF_BUILD_CORE) and !defined(ALIF_BUILD_CORE_MODULE)

#define NUM_KEYWORDS 1
	static struct {
		AlifGCHead thisIsNotUsed{};
		ALIFOBJECT_VAR_HEAD;
		AlifObject* item[NUM_KEYWORDS];
	} _kwtuple = {
			.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, NUM_KEYWORDS),
			.item = { &ALIF_ID(start), },
	};
#undef NUM_KEYWORDS
#define KWTUPLE (&_kwtuple.objBase.objBase)

#else 
#  define KWTUPLE nullptr
#endif

	static const char * const _keywords[] = {"", "بداية", nullptr};
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "اجمع",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[2]{};
	AlifSizeT noptargs = _nargs + (_kwNames ? ALIFTUPLE_GET_SIZE(_kwNames) : 0) - 1;
	AlifObject* iterable{};
	AlifObject* start{};

	_args = _ALIFARG_UNPACKKEYWORDS(_args, _nargs, nullptr, _kwNames, &_parser,
		/*minpos*/ 1, /*maxpos*/ 2, /*minkw*/ 0, /*varpos*/ 0, argsbuf);
	if (!_args) {
		goto exit;
	}
	iterable = _args[0];
	if (!noptargs) {
		goto skip_optional_pos;
	}
	start = _args[1];
skip_optional_pos:
	return_value = builtin_sumImpl(_module, iterable, start);

exit:
	return return_value;
}



#define BUILTIN_ISINSTANCE_METHODDEF    \
    {"هل_نوع", ALIF_CPPFUNCTION_CAST(builtin_isInstance), METHOD_FASTCALL}

static AlifObject * builtin_isInstanceImpl(AlifObject*, AlifObject*, AlifObject*);

static AlifObject * builtin_isInstance(AlifObject *module, AlifObject *const *args,
	AlifSizeT nargs) { // 1172
	AlifObject *returnValue{};
	AlifObject *obj{};
	AlifObject *classOrTuple{};

	if (!_ALIFARG_CHECKPOSITIONAL("هل_نوع", nargs, 2, 2)) {
		goto exit;
	}
	obj = args[0];
	classOrTuple = args[1];
	returnValue = builtin_isInstanceImpl(module, obj, classOrTuple);

exit:
	return returnValue;
}
