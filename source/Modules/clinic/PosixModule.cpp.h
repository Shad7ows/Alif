#include "AlifCore_Abstract.h"
#include "AlifCore_Long.h"
#include "AlifCore_ModSupport.h"




// 38
#define OS_STAT_METHODDEF    \
    {"حالة", ALIF_CPPFUNCTION_CAST(os_stat), METHOD_FASTCALL|METHOD_KEYWORDS},

static AlifObject* os_statImpl(AlifObject*, PathT*, AlifIntT, AlifIntT);

static AlifObject* os_stat(AlifObject* _module,
	AlifObject* const* _args, AlifSizeT _nargs, AlifObject* _kwnames) { // 44
	AlifObject* returnValue = nullptr;

#define KWTUPLE nullptr

	static const char* const _keywords[] = { "مسار", "مجلد_واصف_ملف", "روابط_رمزية", nullptr };
	static AlifArgParser _parser = {
		.keywords = _keywords,
		.fname = "حالة",
		.kwTuple = KWTUPLE,
	};
#undef KWTUPLE
	AlifObject* argsbuf[3];
	AlifSizeT noptargs = _nargs + (_kwnames ? ALIFTUPLE_GET_SIZE(_kwnames) : 0) - 1;
	PathT path = PATH_T_INITIALIZE_P("حالة", "مسار", 0, 0, 0, 1);
	AlifIntT dir_fd = DEFAULT_DIR_FD;
	AlifIntT followSymlinks = 1;

	_args = _ALIFARG_UNPACKKEYWORDS(_args, _nargs, nullptr, _kwnames, &_parser,
		/*minpos*/ 1, /*maxpos*/ 1, /*minkw*/ 0, /*varpos*/ 0, argsbuf);
	if (!_args) {
		goto exit;
	}
	if (!path_converter(_args[0], &path)) {
		goto exit;
	}
	if (!noptargs) {
		goto skip_optional_kwonly;
	}
	if (_args[1]) {
		if (!FSTATAT_DIR_FD_CONVERTER(_args[1], &dir_fd)) {
			goto exit;
		}
		if (!--noptargs) {
			goto skip_optional_kwonly;
		}
	}
	followSymlinks = alifObject_isTrue(_args[2]);
	if (followSymlinks < 0) {
		goto exit;
	}
skip_optional_kwonly:
	returnValue = os_statImpl(_module, &path, dir_fd, followSymlinks);

exit:
	/* Cleanup for path */
	path_cleanup(&path);

	return returnValue;
}






















// 1408
#define OS_GETCWD_METHODDEF    \
    {"اجلب_مسار_العمل", (AlifCPPFunction)os_getcwd, METHOD_NOARGS},

static AlifObject* os_getcwdImpl(AlifObject*);

static AlifObject* os_getcwd(AlifObject* _module,
	AlifObject* ALIF_UNUSED(ignored)) { // 1414
	return os_getcwdImpl(_module);
}























































// 11419
#define OS_CPU_COUNT_METHODDEF    \
    {"عدد_المعالجات", (AlifCPPFunction)os_cpuCount, METHOD_NOARGS},

static AlifObject* os_cpuCountImpl(AlifObject*);

static AlifObject* os_cpuCount(AlifObject* _module,
	AlifObject* ALIF_UNUSED(ignored)) { // 11425
	return os_cpuCountImpl(_module);
}
