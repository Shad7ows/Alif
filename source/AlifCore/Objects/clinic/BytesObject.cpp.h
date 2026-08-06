
#include "AlifCore_Abstract.h"
#include "AlifCore_ModSupport.h"































































static AlifObject* bytes_newImpl(AlifTypeObject*,
	AlifObject*, const char*, const char*); // 1302


static AlifObject* bytes_new(AlifTypeObject* _type,
	AlifObject* _args, AlifObject* _kwargs) { // 1306
	AlifObject* returnValue{};
#  define KWTUPLE nullptr

	static const char* const _keywords[] = { "مصدر", "ترميز", "اخطاء", nullptr };
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "ثمانيات",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[3]{};
	AlifObject* const* fastargs{};
	AlifSizeT nargs = ALIFTUPLE_GET_SIZE(_args);
	AlifSizeT noptargs = nargs + (_kwargs ? ALIFDICT_GET_SIZE(_kwargs) : 0) - 0;
	AlifObject* x = nullptr;
	const char* encoding = nullptr;
	const char* errors = nullptr;

	fastargs = _ALIFARG_UNPACKKEYWORDS(ALIFTUPLE_CAST(_args)->item, nargs, _kwargs, nullptr, &_parser,
		/*minpos*/ 0, /*maxpos*/ 3, /*minkw*/ 0, /*varpos*/ 0, argsbuf);
	if (!fastargs) {
		goto exit;
	}
	if (!noptargs) {
		goto skip_optional_pos;
	}
	if (fastargs[0]) {
		x = fastargs[0];
		if (!--noptargs) {
			goto skip_optional_pos;
		}
	}
	if (fastargs[1]) {
		if (!ALIFUSTR_CHECK(fastargs[1])) {
			//_alifArg_badArgument("bytes", "argument 'encoding'", "str", fastargs[1]);
			goto exit;
		}
		AlifSizeT encodingLength;
		encoding = alifUStr_asUTF8AndSize(fastargs[1], &encodingLength);
		if (encoding == nullptr) {
			goto exit;
		}
		if (strlen(encoding) != (size_t)encodingLength) {
			alifErr_setString(_alifExcValueError_, "يحتوي احرف فارغة");
			goto exit;
		}
		if (!--noptargs) {
			goto skip_optional_pos;
		}
	}
	if (!ALIFUSTR_CHECK(fastargs[2])) {
		//_alifArg_badArgument("bytes", "argument 'errors'", "str", fastargs[2]);
		goto exit;
	}
	AlifSizeT errorsLength;
	errors = alifUStr_asUTF8AndSize(fastargs[2], &errorsLength);
	if (errors == nullptr) {
		goto exit;
	}
	if (strlen(errors) != (size_t)errorsLength) {
		alifErr_setString(_alifExcValueError_, "يحتوي احرف فارغة");
		goto exit;
	}
skip_optional_pos:
	returnValue = bytes_newImpl(_type, x, encoding, errors);

exit:
	return returnValue;
}










