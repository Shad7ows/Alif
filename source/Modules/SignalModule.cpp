#include "alif.h"
#include "AlifCore_Call.h"
#include "AlifCore_Eval.h"
#include "AlifCore_EmscriptenSignal.h"
#include "AlifCore_Errors.h"
#include "AlifCore_State.h"




#define HANDLERS _alifDureRun_.signals.handlers // 105
#define IS_TRIPPED _alifDureRun_.signals.isTripped


typedef SignalsDureRunState SignalStateT; // 110
#define SIGNAL_GLOBAL_STATE _alifDureRun_.signals


ALIF_LOCAL_INLINE(AlifObject*) get_handler(AlifIntT _i) { // 128
	return (AlifObject*)alifAtomic_loadPtr(&HANDLERS[_i].func);
}

static inline AlifIntT compare_handler(AlifObject* _func, AlifObject* _dflIgnHandler) { // 152
	if (_func == nullptr or _dflIgnHandler == nullptr) {
		return 0;
	}
	if (!ALIFLONG_CHECKEXACT(_func)) {
		return 0;
	}
	return alifObject_richCompareBool(_func, _dflIgnHandler, ALIF_EQ) == 1;
}

AlifIntT alifErr_checkSignals(void) { // 1763
	AlifThread* tstate = alifThread_get();

	if (alifEval_breakerBitIsSet(tstate, ALIF_GC_SCHEDULED_BIT)) {
		alifUnset_evalBreakerBit(tstate, ALIF_GC_SCHEDULED_BIT);
		//_alif_runGC(tstate);
	}

	if (!alif_threadCanHandleSignals(tstate->interpreter)) {
		return 0;
	}

	return _alifErr_checkSignalsTstate(tstate);
}


AlifIntT _alifErr_checkSignalsTstate(AlifThread* _tState) { // 1788
	_ALIF_CHECK_EMSCRIPTEN_SIGNALS();
	if (!alifAtomic_loadInt(&IS_TRIPPED)) {
		return 0;
	}

	alifAtomic_storeInt(&IS_TRIPPED, 0);

	AlifInterpreterFrame* frame = _alifThreadState_getFrame(_tState);
	SignalStateT* state = &SIGNAL_GLOBAL_STATE;
	for (AlifIntT i = 1; i < ALIF_NSIG; i++) {
		if (!alifAtomic_loadIntRelaxed(&HANDLERS[i].tripped)) {
			continue;
		}
		alifAtomic_storeIntRelaxed(&HANDLERS[i].tripped, 0);

		AlifObject* func = get_handler(i);
		if (func == nullptr or func == ALIF_NONE or
			compare_handler(func, state->ignoreHandler) or
			compare_handler(func, state->defaultHandler)) {

			//alifErr_format(_alifExcOSError_,
				//"Signal %i ignored due to race condition",
				//i);
			alifErr_writeUnraisable(ALIF_NONE);
			continue;
		}
		AlifObject* arglist = nullptr;
		if (frame == nullptr) {
			arglist = alif_buildValue("(iO)", i, ALIF_NONE);
		}
		else {
			AlifFrameObject* f = _alifFrame_getFrameObject(frame);
			if (f != nullptr) {
				arglist = alif_buildValue("(iO)", i, f);
			}
		}
		AlifObject* result;
		if (arglist) {
			result = _alifObject_call(_tState, func, arglist, nullptr);
			ALIF_DECREF(arglist);
		}
		else {
			result = nullptr;
		}
		if (!result) {
			alifAtomic_storeInt(&IS_TRIPPED, 1);
			return -1;
		}

		ALIF_DECREF(result);
	}

	return 0;
}





static AlifIntT signal_installHandlers(void) { // 1909
#ifdef SIGPIPE
	alifOS_setSig(SIGPIPE, SIG_IGN);
#endif
#ifdef SIGXFZ
	alifOS_setSig(SIGXFZ, SIG_IGN);
#endif
#ifdef SIGXFSZ
	alifOS_setSig(SIGXFSZ, SIG_IGN);
#endif

	//AlifObject* module = alifImport_importModule("_signal");
	//if (!module) {
	//	return -1;
	//}
	//ALIF_DECREF(module);

	return 0;
}






AlifIntT _alifSignal_init(AlifIntT _installSignalHandlers) { // 1956
	SignalStateT* state = &SIGNAL_GLOBAL_STATE;

	state->defaultHandler = alifLong_fromVoidPtr((void*)SIG_DFL);
	if (state->defaultHandler == nullptr) {
		return -1;
	}

	state->ignoreHandler = alifLong_fromVoidPtr((void*)SIG_IGN);
	if (state->ignoreHandler == nullptr) {
		return -1;
	}

#ifdef _WINDOWS
	/* Create manual-reset event, initially unset */
	state->sigintEvent = (void*)CreateEvent(nullptr, TRUE, FALSE, FALSE);
	if (state->sigintEvent == nullptr) {
		//alifErr_setFromWindowsErr(0);
		return -1;
	}
#endif

	for (AlifIntT signum = 1; signum < ALIF_NSIG; signum++) {
		alifAtomic_storeIntRelaxed(&HANDLERS[signum].tripped, 0);
	}

	if (_installSignalHandlers) {
		if (signal_installHandlers() < 0) {
			return -1;
		}
	}

	return 0;
}









AlifIntT _alifOS_isMainThread(void) { // 2046
	AlifInterpreter* interp = _alifInterpreter_get();
	return alif_threadCanHandleSignals(interp);
}


#ifdef _WINDOWS
void* _alifOS_sigintEvent() { // 2059
	SignalStateT* state = &SIGNAL_GLOBAL_STATE;
	return state->sigintEvent;
}
#endif
