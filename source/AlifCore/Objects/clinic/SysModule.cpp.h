















// 81
#define SYS_EXCEPTHOOK_METHODDEF    \
    {"خطاف_الخلل", ALIF_CPPFUNCTION_CAST(sys_excepthook), METHOD_FASTCALL}

static AlifObject* sys_excepthookImpl(AlifObject* module, AlifObject* exctype,
	AlifObject* value, AlifObject* traceback);

static AlifObject* sys_excepthook(AlifObject* module, AlifObject* const* args, AlifSizeT nargs) { // 88
	AlifObject* return_value = nullptr;
	AlifObject* exctype;
	AlifObject* value;
	AlifObject* traceback;

	if (!_ALIFARG_CHECKPOSITIONAL("خطاف_الخلل", nargs, 3, 3)) {
		goto exit;
	}
	exctype = args[0];
	value = args[1];
	traceback = args[2];
	return_value = sys_excepthookImpl(module, exctype, value, traceback);

exit:
	return return_value;
}


// 179
#define SYS_EXIT_METHODDEF    \
    {"خروج", ALIF_CPPFUNCTION_CAST(sys_exit), METHOD_FASTCALL},

static AlifObject*
sys_exitImpl(AlifObject*, AlifObject* ); // 183

static AlifObject* sys_exit(AlifObject* _module, AlifObject* const* _args, AlifSizeT _nArgs) { // 186
	AlifObject* returnValue = nullptr;
	AlifObject* status = ALIF_NONE;

	if (!_alifArg_checkPositional("خروج", _nArgs, 0, 1)) {
		goto exit;
	}
	if (_nArgs < 1) {
		goto skipOptional;
	}
	status = _args[0];
skipOptional:
	returnValue = sys_exitImpl(_module, status);

exit:
	return returnValue;
}

//358
#define SYS_GETTRACE_METHODDEF    \
    {"تتبع", (AlifCPPFunction)sys_getTrace, METHOD_NOARGS},

static AlifObject* sys_getTraceImpl(AlifObject* );

static AlifObject* sys_getTrace(AlifObject* _module, AlifObject* ALIF_UNUSED(ignored)) { // 365
	return sys_getTraceImpl(_module);
}

// 493
#define SYS_SETRECURSIONLIMIT_METHODDEF    \
    {"ضبط_ارتداد", (AlifCPPFunction)sys_setRecursionLimit, METHOD_O},  // اقتراح اسم اخر وهو "استدعاء_ذاتي"

static AlifObject* sys_setRecursionLimitImpl(AlifObject* , AlifIntT );

static AlifObject* sys_setRecursionLimit(AlifObject* _module, AlifObject* _arg) { // 500
	AlifObject* returnValue = nullptr;
	AlifIntT newLimit{};

	newLimit = alifLong_asInt(_arg);
	if (newLimit == -1 and alifErr_occurred()) {
		goto exit;
	}
	returnValue = sys_setRecursionLimitImpl(_module, newLimit);

exit:
	return returnValue;
}

// 636
#define SYS_GETRECURSIONLIMIT_METHODDEF    \
    {"جلب_ارتداد", (AlifCPPFunction)sys_getRecursionLimit, METHOD_NOARGS}, // اقتراح اسم اخر وهو "استدعاء_ذاتي"

static AlifObject* sys_getRecursionLimitImpl(AlifObject* );

static AlifObject* sys_getRecursionLimit(AlifObject* _module, AlifObject* ALIF_UNUSED(ignored)) { // 643
	return sys_getRecursionLimitImpl(_module);
}
