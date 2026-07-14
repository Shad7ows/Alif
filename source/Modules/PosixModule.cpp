#include "alif.h"

#include "AlifCore_Abstract.h"
#include "AlifCore_Call.h"
#include "AlifCore_Eval.h"
#include "AlifCore_FileUtils.h"
#include "AlifCore_InitConfig.h"
#include "AlifCore_Long.h"
#include "AlifCore_ModuleObject.h"
#include "AlifCore_Object.h"
#include "AlifCore_LifeCycle.h"
#include "AlifCore_State.h"
#include "AlifCore_Signal.h"
#include "AlifCore_Time.h"
#include "AlifCore_TypeObject.h"
#include "AlifCore_Memory.h"


























































// 543
#ifdef _WINDOWS
#  define INITFUNC alifInit_nt
#  define MODNAME "واجهة_نظام_ويندوز"
#  define MODNAME_OBJ &ALIF_ID(nt)
#else
#  define INITFUNC alifInit_posix
#  define MODNAME "واجهة_نظام_يونكس"
#  define MODNAME_OBJ &ALIF_ID(posix)
#endif







class  PosixState {
public:
	AlifObject* billion{};
	AlifObject* DirEntryType{};
	AlifObject* ScandirIteratorType{};
#if defined(HAVE_SCHED_SETPARAM) or defined(HAVE_SCHED_SETSCHEDULER)
	or defined(POSIX_SPAWN_SETSCHEDULER) or defined(POSIX_SPAWN_SETSCHEDPARAM)
		AlifObject* SchedParamType{};
#endif
	NewFunc statresult_new_orig{};
	AlifObject* StatResultType{};
	AlifObject* StatVFSResultType{};
	AlifObject* TerminalSizeType{};
	AlifObject* TimesResultType{};
	AlifObject* UnameResultType{};
#if defined(HAVE_WAITID)
	AlifObject* WaitidResultType{};
#endif
#if defined(HAVE_WAIT3) or defined(HAVE_WAIT4)
	AlifObject* struct_rusage{};
#endif
	AlifObject* st_mode;
#ifndef _WINDOWS
	// times() clock frequency in hertz; used by os.times()
	long ticks_per_second{};
#endif
};









#ifdef _WINDOWS // 4641





AlifIntT _alifOS_getFullPathName(const wchar_t* _path, wchar_t** _absPathP) { // 4921
	wchar_t woutbuf[MAX_PATH], * woutbufp = woutbuf;
	DWORD result{};

	result = GetFullPathNameW(_path,
		ALIF_ARRAY_LENGTH(woutbuf), woutbuf, nullptr);
	if (!result) {
		return -1;
	}

	if (result >= ALIF_ARRAY_LENGTH(woutbuf)) {
		if ((AlifUSizeT)result <= (AlifUSizeT)ALIF_SIZET_MAX / sizeof(wchar_t)) {
			woutbufp = (wchar_t*)alifMem_dataAlloc((AlifUSizeT)result * sizeof(wchar_t));
		}
		else {
			woutbufp = nullptr;
		}
		if (!woutbufp) {
			*_absPathP = nullptr;
			return 0;
		}

		result = GetFullPathNameW(_path, result, woutbufp, nullptr);
		if (!result) {
			alifMem_dataFree(woutbufp);
			return -1;
		}
	}

	if (woutbufp != woutbuf) {
		*_absPathP = woutbufp;
		return 0;
	}

	*_absPathP = alifMem_wcsDup(woutbufp);
	return 0;
}



#endif // 5494










static AlifObject* os_cpuCountImpl(AlifObject* _module) { // 15362
	const AlifConfig* config = alif_getConfig();
	if (config->cpuCount > 0) {
		return alifLong_fromLong(config->cpuCount);
	}

	AlifIntT ncpu = 0;
#ifdef _WINDOWS
# ifdef MS_WINDOWS_DESKTOP
	ncpu = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
# else
	ncpu = 0;
# endif

#elif defined(__hpux)
	ncpu = mpctl(MPC_GETNUMSPUS, NULL, NULL);

#elif defined(HAVE_SYSCONF) && defined(_SC_NPROCESSORS_ONLN)
	ncpu = sysconf(_SC_NPROCESSORS_ONLN);

#elif defined(__VXWORKS__)
	ncpu = _alif_popcount32(vxCpuEnabledGet());

#elif defined(__DragonFly__) or \
      defined(__OpenBSD__)   or \
      defined(__FreeBSD__)   or \
      defined(__NetBSD__)    or \
      defined(__APPLE__)
	ncpu = 0;
	size_t len = sizeof(ncpu);
	int mib[2] = { CTL_HW, HW_NCPU };
	if (sysctl(mib, 2, &ncpu, &len, NULL, 0) != 0) {
		ncpu = 0;
	}
#endif

	if (ncpu < 1) {
		return ALIF_NONE;
	}
	return alifLong_fromLong(ncpu);
}





#include "clinic/PosixModule.cpp.h" // 15935
















AlifObject* alifOS_fsPath(AlifObject* path) { // 16477
	AlifObject* func = nullptr;
	AlifObject* pathRepr = nullptr;

	if (ALIFUSTR_CHECK(path) or ALIFBYTES_CHECK(path)) {
		return ALIF_NEWREF(path);
	}

	//func = _alifObject_lookupSpecial(path, &ALIF_ID(__fspath__));
	//if ((func == nullptr) or (func == ALIF_NONE)) {
	//	return alifErr_format(_alifExcTypeError_,
	//		"expected str, bytes or os.PathLike object, "
	//		"not %.200s",
	//		_alifType_name(ALIF_TYPE(path)));
	//}

	//pathRepr = _alifObject_callNoArgs(func);
	//ALIF_DECREF(func);
	//if (nullptr == pathRepr) {
	//	return nullptr;
	//}

	if (!(ALIFUSTR_CHECK(pathRepr) or ALIFBYTES_CHECK(pathRepr))) {
		alifErr_format(_alifExcTypeError_,
			"expected %.200s.__fspath__() to return str or bytes, "
			"not %.200s", _alifType_name(ALIF_TYPE(path)),
			_alifType_name(ALIF_TYPE(pathRepr)));
		ALIF_DECREF(pathRepr);
		return nullptr;
	}

	return pathRepr;
}













static AlifMethodDef _posixMethods_[] = { // 16898
	OS_CPU_COUNT_METHODDEF
	{nullptr,              nullptr}            /* Sentinel */
};







static AlifModuleDefSlot _posixModuleSlots_[] = { // 18138
	//{ALIF_MOD_EXEC, posixModule_exec},
	{ALIF_MOD_MULTIPLE_INTERPRETERS, ALIF_MOD_PER_INTERPRETER_GIL_SUPPORTED},
	{ALIF_MOD_GIL, ALIF_MOD_GIL_NOT_USED},
	{0, nullptr}
};

static AlifModuleDef _posixModule_ = { // 18145
	.base = ALIFMODULEDEF_HEAD_INIT,
	.name = MODNAME,
	.size = sizeof(PosixState),
	.methods = _posixMethods_,
	.slots = _posixModuleSlots_,
	//.traverse = _posix_traverse,
	//.clear = _posix_clear,
	//.free = _posix_free,
};



AlifObject* INITFUNC(void) { // 18157
	return alifModuleDef_init(&_posixModule_);
}


