#include "alif.h"

#include "AlifCore_ModuleObject.h"
#include "AlifCore_DureRun.h"
#include "AlifCore_Time.h"



#include <time.h>                 // clock()
#ifdef HAVE_SYS_TIMES_H
#  include <sys/times.h>          // times()
#endif
#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif

#if defined(__APPLE__ ) and defined(__has_builtin)
#  if __has_builtin(__builtin_available)
#    define HAVE_CLOCK_GETTIME_RUNTIME __builtin_available(macOS 10.12, iOS 10.0, tvOS 10.0, watchOS 3.0, *)
#  endif
#endif
#ifndef HAVE_CLOCK_GETTIME_RUNTIME
#  define HAVE_CLOCK_GETTIME_RUNTIME 1
#endif

static AlifIntT alif_sleep(AlifTimeT); // 74


class TimeModuleState { // 77
public:
	AlifTypeObject* structTimeType{};
#ifdef HAVE_CLOCK
	// clock() frequency in hertz
	AlifTimeFraction clockBase{};
#endif
};


static inline TimeModuleState* get_timeState(AlifObject* module) { // 90
	void* state = _alifModule_getState(module);
	return (TimeModuleState*)state;
}


static AlifObject* _alifFloat_fromAlifTime(AlifTimeT _t) { // 99
	double d = alifTime_asSecondsDouble(_t);
	return alifFloat_fromDouble(d);
}

static AlifObject* time_time(AlifObject* _self, AlifObject* _unused) { // 107
	AlifTimeT t{};
	if (alifTime_time(&t) < 0) {
		return nullptr;
	}
	return _alifFloat_fromAlifTime(t);
}


#ifdef HAVE_CLOCK

#ifndef CLOCKS_PER_SEC
#  ifdef CLK_TCK
#    define CLOCKS_PER_SEC CLK_TCK
#  else
#    define CLOCKS_PER_SEC 1000000
#  endif
#endif

static AlifIntT alif_clock(TimeModuleState* _state, AlifTimeT* _tp, AlifClockInfoT* _info) { // 150
	AlifTimeFraction* base = &_state->clockBase;

	if (_info) {
		_info->implementation = "clock()";
		_info->resolution = alifTimeFraction_resolution(base);
		_info->monotonic = 1;
		_info->adjustable = 0;
	}

	clock_t ticks = clock();
	if (ticks == (clock_t)-1) {
		//alifErr_setString(_alifExcRuntimeError_,
			//"the processor time used is not available "
			//"or its value cannot be represented");
		return -1;
	}
	*_tp = alifTimeFraction_mul(ticks, base);
	return 0;
}
#endif 

static AlifObject* time_sleep(AlifObject* _self, AlifObject* _timeoutObj) { // 391
	//if (alifSys_audit("time.sleep", "O", timeout_obj) < 0) {
	//	return nullptr;
	//}

	AlifTimeT timeout{};
	if (_alifTime_fromSecondsObject(&timeout, _timeoutObj,
		AlifTimeRoundT::AlifTime_Round_TIMEOUT))
		return nullptr;
	if (timeout < 0) {
		//alifErr_setString(_alifExcValueError_,
		//	"sleep length must be non-negative");
		return nullptr;
	}
	if (alif_sleep(timeout) != 0) {
		return nullptr;
	}

	return ALIF_NONE;
}



static AlifStructSequenceField _structTimeTypeFields_[] = { // 418
	{0}
};

static AlifStructSequenceDesc _structTimeTypeDesc_ = { // 433
	"time.struct_time",
	"",
	_structTimeTypeFields_,
	0,
};

 // 446
#if defined(_WINDOWS)
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

static DWORD _timerFlags_ = (DWORD)-1;
#endif



 // 594
//#if defined(__linux__) && !defined(__GLIBC__)
//static const char* _utcString_ = nullptr;
//#endif
//
//static AlifIntT alifProcess_time(TimeModuleState* state, AlifTimeT* tp,
//	AlifClockInfoT* info) { // 1256
//#if defined(MS_WINDOWS)
//	HANDLE process{};
//	FILETIME creationTime{}, exitTime{}, kernelTime{}, userTime{};
//	ULARGE_INTEGER large{};
//	AlifTimeT kTime{}, uTime{};
//	BOOL ok{};
//
//	process = GetCurrentProcess();
//	ok = GetProcessTimes(process, &creationTime, &exitTime,
//		&kernelTime, &userTime);
//	if (!ok) {
//		//alifErr_setFromWindowsErr(0);
//		return -1;
//	}
//
//	if (info) {
//		info->implementation = "GetProcessTimes()";
//		info->resolution = 1e-7;
//		info->monotonic = 1;
//		info->adjustable = 0;
//	}
//
//	large.u.LowPart = kernelTime.dwLowDateTime;
//	large.u.HighPart = kernelTime.dwHighDateTime;
//	kTime = large.QuadPart;
//
//	large.u.LowPart = userTime.dwLowDateTime;
//	large.u.HighPart = userTime.dwHighDateTime;
//	uTime = large.QuadPart;
//
//	*tp = (kTime + uTime) * 100;
//	return 0;
//#else
//
////#if defined(HAVE_CLOCK_GETTIME) \
//    and (defined(CLOCK_PROCESS_CPUTIME_ID) || defined(CLOCK_PROF)) \
//    and !defined(__wasi__) \
//    and !defined(__NetBSD__)
//	struct timespec ts;
//
//	if (HAVE_CLOCK_GETTIME_RUNTIME) {
//
//#ifdef CLOCK_PROF
//		const clockid_t clk_id = CLOCK_PROF;
//		const char* function = "clock_gettime(CLOCK_PROF)";
//#else
//		const clockid_t clk_id = CLOCK_PROCESS_CPUTIME_ID;
//		const char* function = "clock_gettime(CLOCK_PROCESS_CPUTIME_ID)";
//#endif
//
//		if (clock_getTime(clk_id, &ts) == 0) {
//			if (info) {
//				struct timespec res;
//				info->implementation = function;
//				info->monotonic = 1;
//				info->adjustable = 0;
//				if (clock_getRes(clk_id, &res)) {
//					//alifErr_setFromErrno(_alifExcOSError_);
//					return -1;
//				}
//				info->resolution = res.tv_sec + res.tv_nsec * 1e-9;
//			}
//
//			if (alifTime_fromTimespec(tp, &ts) < 0) {
//				return -1;
//			}
//			return 0;
//		}
//	}
////#endif
//
//#if defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRUSAGE)
//	struct rusage ru;
//
//	if (getrusage(RUSAGE_SELF, &ru) == 0) {
//		AlifTimeT utime, stime;
//
//		if (info) {
//			info->implementation = "getrusage(RUSAGE_SELF)";
//			info->monotonic = 1;
//			info->adjustable = 0;
//			info->resolution = 1e-6;
//		}
//
//		if (_PyTime_FromTimeval(&utime, &ru.ru_utime) < 0) {
//			return -1;
//		}
//		if (_PyTime_FromTimeval(&stime, &ru.ru_stime) < 0) {
//			return -1;
//		}
//
//		AlifTimeT total = utime + stime;
//		*tp = total;
//		return 0;
//	}
//#endif
//
//	/* times() */
//// gh-115714: Don't use times() on WASI.
//#if defined(HAVE_TIMES) && !defined(__wasi__)
//	int res = process_time_times(state, tp, info);
//	if (res < 0) {
//		return -1;
//	}
//	if (res == 1) {
//		return 0;
//	}
//#endif
//
//	return alif_clock(state, tp, info);
//#endif
//}
//
//static AlifObject* time_process_time(AlifObject* _module, AlifObject* _unused) { // 1383
//	TimeModuleState* state = get_timeState(_module);
//	AlifTimeT t{};
//	if (alifprocess_time(state, &t, NULL) < 0) {
//		return NULL;
//	}
//	return alifFloat_fromPyTime(t);
//}


static AlifIntT time_exec(AlifObject* module) { // 1942
	TimeModuleState* state = get_timeState(module);
	// struct_time type
	state->structTimeType = alifStructSequence_newType(&_structTimeTypeDesc_);
	if (state->structTimeType == nullptr) {
		return -1;
	}
	if (alifModule_addType(module, state->structTimeType)) {
		return -1;
	}

#if defined(__linux__) && !defined(__GLIBC__)
	struct tm tm;
	const time_t zero = 0;
	if (gmtime_r(&zero, &tm) != nullptr)
		_utcString_ = tm.tm_zone;
#endif

#if defined(_WINDOWS)
	if (_timerFlags_ == (DWORD)-1) {
		DWORD test_flags = CREATE_WAITABLE_TIMER_HIGH_RESOLUTION;
		HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr, test_flags,
			TIMER_ALL_ACCESS);
		if (timer == nullptr) {
			// CREATE_WAITABLE_TIMER_HIGH_RESOLUTION is not supported.
			_timerFlags_ = 0;
		}
		else {
			// CREATE_WAITABLE_TIMER_HIGH_RESOLUTION is supported.
			_timerFlags_ = CREATE_WAITABLE_TIMER_HIGH_RESOLUTION;
			CloseHandle(timer);
		}
	}
#endif

	return 0;
}

static AlifMethodDef _timeMethods_[] = {
	{"الوقت", time_time, METHOD_NOARGS},
	{"غفوة", time_sleep, METHOD_O},
	{nullptr, nullptr}           /* sentinel */
};

static AlifModuleDefSlot _timeSlots_[] = {
	{ALIF_MOD_EXEC, (void*)time_exec},
	{ALIF_MOD_MULTIPLE_INTERPRETERS, ALIF_MOD_PER_INTERPRETER_GIL_SUPPORTED},
	{ALIF_MOD_GIL, ALIF_MOD_GIL_NOT_USED},
	{0, nullptr}
};

static AlifModuleDef _timeModule_ = {
	.base = ALIFMODULEDEF_HEAD_INIT,
	.name = "الوقت",
	//.size = sizeof(TimeModuleState),
	.methods = _timeMethods_,
	.slots = _timeSlots_,
	//.traverse = time_module_traverse,
	//.clear = time_module_clear,
	//.free = time_module_free,
};



AlifObject* alifInit_time(void) {
	return alifModuleDef_init(&_timeModule_);
}





static AlifIntT alif_sleep(AlifTimeT timeout) { // 2181
#ifndef _WINDOWS
#ifdef HAVE_CLOCK_NANOSLEEP
	struct timespec timeout_abs {};
#elif defined(HAVE_NANOSLEEP)
	struct timespec timeout_ts {};
#else
	struct timeval timeout_tv {};
#endif
	AlifTimeT deadline{}, monotonic{};
	AlifIntT err = 0;

	if (alifTime_monotonic(&monotonic) < 0) {
		return -1;
	}
	deadline = monotonic + timeout;
#ifdef HAVE_CLOCK_NANOSLEEP
	if (_alifTime_asTimeSpec(deadline, &timeout_abs) < 0) {
		return -1;
	}
#endif

	do {
#ifdef HAVE_CLOCK_NANOSLEEP
		// use timeout_abs
#elif defined(HAVE_NANOSLEEP)
		if (_alifTime_asTimeSpec(timeout, &timeout_ts) < 0) {
			return -1;
		}
#else
		if (_alifTime_asTimEval(timeout, &timeout_tv, AlifTimeRoundT::AlifTime_Round_CEILING) < 0) {
			return -1;
		}
#endif

		AlifIntT ret{};
		ALIF_BEGIN_ALLOW_THREADS
#ifdef HAVE_CLOCK_NANOSLEEP
			ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timeout_abs, nullptr);
		err = ret;
#elif defined(HAVE_NANOSLEEP)
			ret = nanosleep(&timeout_ts, nullptr);
		err = errno;
#else
			ret = select(0, (fd_set*)0, (fd_set*)0, (fd_set*)0, &timeout_tv);
		err = errno;
#endif
		ALIF_END_ALLOW_THREADS

			if (ret == 0) {
				break;
			}

		if (err != EINTR) {
			errno = err;
			//alifErr_setFromErrno(_alifExcOSError_);
			return -1;
		}

		/* sleep was interrupted by SIGINT */
		//if (alifErr_checkSignals()) {
		//	return -1;
		//}

#ifndef HAVE_CLOCK_NANOSLEEP
		if (alifTime_monotonic(&monotonic) < 0) {
			return -1;
		}
		timeout = deadline - monotonic;
		if (timeout < 0) {
			break;
		}
		/* retry with the recomputed delay */
#endif
	} while (1);

	return 0;
#else  // _WINDOWS
	AlifTimeT timeout_100ns = _alifTime_as100Nanoseconds(timeout,
		AlifTimeRoundT::AlifTime_Round_CEILING);

	// Maintain Windows Sleep() semantics for time.sleep(0)
	if (timeout_100ns == 0) {
		ALIF_BEGIN_ALLOW_THREADS
			// A value of zero causes the thread to relinquish the remainder of its
			// time slice to any other thread that is ready to run. If there are no
			// other threads ready to run, the function returns immediately, and
			// the thread continues execution.
			Sleep(0);
		ALIF_END_ALLOW_THREADS
			return 0;
	}

	LARGE_INTEGER relative_timeout{};
	// SetWaitableTimer(): a negative due time indicates relative time
	relative_timeout.QuadPart = -timeout_100ns;

	HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr, _timerFlags_,
		TIMER_ALL_ACCESS);
	if (timer == nullptr) {
		//alifErr_setFromWindowsErr(0);
		return -1;
	}

	if (!SetWaitableTimerEx(timer, &relative_timeout,
		0, // no period; the timer is signaled once
		nullptr, nullptr, // no completion routine
		nullptr,  // no wake context; do not resume from suspend
		0)) // no tolerable delay for timer coalescing
	{
		//alifErr_setFromWindowsErr(0);
		goto error;
	}

	// Only the main thread can be interrupted by SIGINT.
	// Signal handlers are only executed in the main thread.
	if (_alifOS_isMainThread()) {
		HANDLE sigint_event = _alifOS_sigintEvent();

		while (1) {
			// Check for pending SIGINT signal before resetting the event
			//if (alifErr_checkSignals()) {
			//	goto error;
			//}
			ResetEvent(sigint_event);

			HANDLE events[] = { timer, sigint_event };
			DWORD rc;

			ALIF_BEGIN_ALLOW_THREADS
				rc = WaitForMultipleObjects(ALIF_ARRAY_LENGTH(events), events,
					// bWaitAll
					FALSE,
					// No wait timeout
					INFINITE);
			ALIF_END_ALLOW_THREADS

				if (rc == WAIT_FAILED) {
					//alifErr_setFromWindowsErr(0);
					goto error;
				}

			if (rc == WAIT_OBJECT_0) {
				// Timer signaled: we are done
				break;
			}
		}
	}
	else {
		DWORD rc;
	
		ALIF_BEGIN_ALLOW_THREADS
		rc = WaitForSingleObject(timer, INFINITE);
		ALIF_END_ALLOW_THREADS
	
			if (rc == WAIT_FAILED) {
				//alifErr_setFromWindowsErr(0);
				goto error;
			}
	}

	CloseHandle(timer);
	return 0;

error:
	CloseHandle(timer);
	return -1;
#endif
}
