#include "AlifCore_Abstract.h"
#include "AlifCore_ModSupport.h"  





// 147
#define UNICODE_COUNT_METHODDEF    \
    {"كم", ALIF_CPPFUNCTION_CAST(uStr_count), METHOD_FASTCALL},

static AlifSizeT uStr_countImpl(AlifObject*, AlifObject*, AlifSizeT, AlifSizeT);

static AlifObject* uStr_count(AlifObject* str, AlifObject*const* args, AlifSizeT nargs) { // 154
	AlifObject* returnValue{};
	AlifObject* substr{};
	AlifSizeT start = 0;
	AlifSizeT end = ALIF_SIZET_MAX;
	AlifSizeT returnValue_;

	if (!_ALIFARG_CHECKPOSITIONAL("كم", nargs, 1, 3)) {
		goto exit;
	}
	if (!ALIFUSTR_CHECK(args[0])) {
		//_alifArg_badArgument("كم", "argument 1", "str", args[0]);
		goto exit;
	}
	substr = args[0];
	if (nargs < 2) {
		goto skip_optional;
	}
	if (!_alifEval_sliceIndex(args[1], &start)) {
		goto exit;
	}
	if (nargs < 3) {
		goto skip_optional;
	}
	if (!_alifEval_sliceIndex(args[2], &end)) {
		goto exit;
	}
skip_optional:
	returnValue_ = uStr_countImpl(str, substr, start, end);
	if ((returnValue_ == -1) and alifErr_occurred()) {
		goto exit;
	}
	returnValue = alifLong_fromSizeT(returnValue_);

exit:
	return returnValue;
}





// 368
#define UNICODE_FIND_METHODDEF    \
    {"اوجد", ALIF_CPPFUNCTION_CAST(uStr_find), METHOD_FASTCALL},

static AlifSizeT uStr_findImpl(AlifObject*, AlifObject*, AlifSizeT, AlifSizeT);

static AlifObject* uStr_find(AlifObject* str,
	AlifObject *const* args, AlifSizeT nargs) {
	AlifObject* returnValue{};
	AlifObject* substr{};
	AlifSizeT start = 0;
	AlifSizeT end = ALIF_SIZET_MAX;
	AlifSizeT returnValue_{};

	if (!_alifArg_checkPositional("اوجد", nargs, 1, 3)) {
		goto exit;
	}
	if (!ALIFUSTR_CHECK(args[0])) {
		//_alifArg_badArgument("find", "argument 1", "str", args[0]);
		goto exit;
	}
	substr = args[0];
	if (nargs < 2) {
		goto skip_optional;
	}
	if (!_alifEval_sliceIndex(args[1], &start)) {
		goto exit;
	}
	if (nargs < 3) {
		goto skip_optional;
	}
	if (!_alifEval_sliceIndex(args[2], &end)) {
		goto exit;
	}
skip_optional:
	returnValue_ = uStr_findImpl(str, substr, start, end);
	if ((returnValue_ == -1) and alifErr_occurred()) {
		goto exit;
	}
	returnValue = alifLong_fromSizeT(returnValue_);

exit:
	return returnValue;
}






// 919
#define UNICODE_REPLACE_METHODDEF    \
    {"استبدل", ALIF_CPPFUNCTION_CAST(unicode_replace), METHOD_FASTCALL|METHOD_KEYWORDS},

static AlifObject* unicode_replaceImpl(AlifObject*, AlifObject*, AlifObject*, AlifSizeT);

static AlifObject* unicode_replace(AlifObject* self, AlifObject* const* args,
	AlifSizeT nargs, AlifObject* kwnames) { // 926
	AlifObject* return_value = nullptr;
#if defined(ALIF_BUILD_CORE) and !defined(ALIF_BUILD_CORE_MODULE)

#define NUM_KEYWORDS 1
	static struct {
		AlifGCHead _thisIsNotUsed;
		ALIFOBJECT_VAR_HEAD;
		AlifObject* item[NUM_KEYWORDS];
	} _kwtuple = {
		.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, NUM_KEYWORDS)
		.item = { &ALIF_ID(Count), },
	};
#undef NUM_KEYWORDS
#define KWTUPLE (&_kwtuple.objBase.objBase)

#else 
#  define KWTUPLE nullptr
#endif

	static const char* const _keywords[] = { "", "", "التكرار", nullptr };
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "استبدل",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[3]{};
	AlifSizeT noptargs = nargs + (kwnames ? ALIFTUPLE_GET_SIZE(kwnames) : 0) - 2;
	AlifObject* old{};
	AlifObject* new_{};
	AlifSizeT count = -1;

	args = ALIFARG_UNPACKKEYWORDS(args, nargs, nullptr, kwnames, &_parser, 2, 3, 0, argsbuf);
	if (!args) {
		goto exit;
	}
	if (!ALIFUSTR_CHECK(args[0])) {
		//_alifArg_badArgument("replace", "argument 1", "str", args[0]);
		goto exit;
	}
	old = args[0];
	if (!ALIFUSTR_CHECK(args[1])) {
		//_alifArg_badArgument("replace", "argument 2", "str", args[1]);
		goto exit;
	}
	new_ = args[1];
	if (!noptargs) {
		goto skip_optional_pos;
	}
	{
		AlifSizeT ival = -1;
		AlifObject* iobj = _alifNumber_index(args[2]);
		if (iobj != nullptr) {
			ival = alifLong_asSizeT(iobj);
			ALIF_DECREF(iobj);
		}
		if (ival == -1 and alifErr_occurred()) {
			goto exit;
		}
		count = ival;
	}
skip_optional_pos:
	return_value = unicode_replaceImpl(self, old, new_, count);

exit:
	return return_value;
}


// 1245
#define UNICODE_SPLIT_METHODDEF    \
    {"افصل", ALIF_CPPFUNCTION_CAST(uStr_split), METHOD_FASTCALL|METHOD_KEYWORDS},

static AlifObject* uStr_splitImpl(AlifObject*, AlifObject*, AlifSizeT);

static AlifObject* uStr_split(AlifObject* _self, AlifObject* const* _args,
	AlifSizeT _nargs, AlifObject* _kwnames) { // 1251
	AlifObject* returnValue = nullptr;
#if defined(ALIF_BUILD_CORE) and !defined(ALIF_BUILD_CORE_MODULE)

#define NUM_KEYWORDS 2
	static struct {
		AlifGCHead thisIsNotUsed;
		ALIFOBJECT_VAR_HEAD;
		AlifObject* item[NUM_KEYWORDS];
	} _kwtuple = {
		.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, NUM_KEYWORDS),
		.item = { &ALIF_STR(Sep), &ALIF_ID(MaxSplit), },
	};
#undef NUM_KEYWORDS
#define KWTUPLE (&_kwtuple.objBase.objBase)

#else 
#  define KWTUPLE nullptr
#endif

	static const char* const _keywords[] = { "الفاصل", "التكرار", nullptr };
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "افصل",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[2];
	AlifSizeT noptargs = _nargs + (_kwnames ? ALIFTUPLE_GET_SIZE(_kwnames) : 0) - 0;
	AlifObject* sep = ALIF_NONE;
	AlifSizeT maxsplit = -1;

	_args = ALIFARG_UNPACKKEYWORDS(_args, _nargs, nullptr, _kwnames, &_parser, 0, 2, 0, argsbuf);
	if (!_args) {
		goto exit;
	}
	if (!noptargs) {
		goto skip_optional_pos;
	}
	if (_args[0]) {
		sep = _args[0];
		if (!--noptargs) {
			goto skip_optional_pos;
		}
	}
	{
		AlifSizeT ival = -1;
		AlifObject* iobj = _alifNumber_index(_args[1]);
		if (iobj != NULL) {
			ival = alifLong_asSizeT(iobj);
			ALIF_DECREF(iobj);
		}
		if (ival == -1 and alifErr_occurred()) {
			goto exit;
		}
		maxsplit = ival;
	}
skip_optional_pos:
	returnValue = uStr_splitImpl(_self, sep, maxsplit);

exit:
	return returnValue;
}




// 1330
#define UNICODE_PARTITION_METHODDEF    \
    {"اقسم", (AlifCPPFunction)uStr_partition, METHOD_O},

#define UNICODE_RPARTITION_METHODDEF    \
    {"اقسم_ع", (AlifCPPFunction)uStr_rpartition, METHOD_O},


// 1367
#define UNICODE_RSPLIT_METHODDEF    \
    {"افصل_ع", ALIF_CPPFUNCTION_CAST(uStr_rsplit), METHOD_FASTCALL|METHOD_KEYWORDS},

static AlifObject* uStr_rsplitImpl(AlifObject*, AlifObject*, AlifSizeT);

static AlifObject* uStr_rsplit(AlifObject* self, AlifObject *const* args,
	AlifSizeT nargs, AlifObject* kwnames) { // 1373
	AlifObject* returnValue = nullptr;
#if defined(ALIF_BUILD_CORE) and !defined(ALIF_BUILD_CORE_MODULE)

#define NUM_KEYWORDS 2
	static struct {
		AlifGCHead thisIsNotUsed{};
		ALIFOBJECT_VAR_HEAD{};
		AlifObject* item[NUM_KEYWORDS]{};
	} _kwtuple = {
			.objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, NUM_KEYWORDS),
			.item = { &ALIF_STR(Sep), &ALIF_STR(MaxSplit), },
	};
#undef NUM_KEYWORDS
#define KWTUPLE (&_kwtuple.ob_base.ob_base)

#else 
#  define KWTUPLE nullptr
#endif

	static const char * const _keywords[] = {"الفاصل", "التكرار", nullptr};
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "افصل_ع",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject *argsbuf[2];
	AlifSizeT noptargs = nargs + (kwnames ? ALIFTUPLE_GET_SIZE(kwnames) : 0) - 0;
	AlifObject* sep = ALIF_NONE;
	AlifSizeT maxsplit = -1;

	args = ALIFARG_UNPACKKEYWORDS(args, nargs, nullptr, kwnames, &_parser, 0, 2, 0, argsbuf);
	if (!args) {
		goto exit;
	}
	if (!noptargs) {
		goto skip_optional_pos;
	}
	if (args[0]) {
		sep = args[0];
		if (!--noptargs) {
			goto skip_optional_pos;
		}
	}
	{
		AlifSizeT ival = -1;
		AlifObject* iobj = _alifNumber_index(args[1]);
		if (iobj != nullptr) {
			ival = alifLong_asSizeT(iobj);
			ALIF_DECREF(iobj);
		}
		if (ival == -1 and alifErr_occurred()) {
			goto exit;
		}
		maxsplit = ival;
	}
skip_optional_pos:
	returnValue = uStr_rsplitImpl(self, sep, maxsplit);

exit:
	return returnValue;
}
