#include "alif.h"

#include "AlifCore_Call.h"
#include "AlifCore_Eval.h"
#include "AlifCore_Dict.h"
#include "AlifCore_InitConfig.h"
#include "AlifCore_ModSupport.h"
#include "AlifCore_Object.h"
#include "AlifCore_Errors.h"
#include "AlifCore_LifeCycle.h"
#include "AlifCore_State.h"
#include "AlifCore_StructSeq.h"
#include "AlifCore_SysModule.h"



#include "clinic/SysModule.cpp.h"

AlifObject* _alifSys_getAttr(AlifThread* _thread, AlifObject* _name) { // 73
	AlifObject* sd = _thread->interpreter->sysDict;
	if (sd == nullptr) {
		return nullptr;
	}
	//AlifObject* exc = _alifErr_getRaisedException(_thread);
	AlifObject* value = _alifDict_getItemWithError(sd, _name);
	//_alifErr_setRaisedException(_thread, exc);
	return value;
}


static AlifObject* _alifSys_getObject(AlifInterpreter* _interp, const char* _name) { // 89
	AlifObject* sysDict = _interp->sysDict;
	if (sysDict == nullptr) {
		return nullptr;
	}
	AlifObject* value;
	if (alifDict_getItemStringRef(sysDict, _name, &value) != 1) {
		return nullptr;
	}
	ALIF_DECREF(value);  // return a borrowed reference
	return value;
}


AlifObject* alifSys_getObject(const char* _name) { // 104
	AlifThread* thread = _alifThread_get();

	AlifObject* exc_ = _alifErr_getRaisedException(thread);
	AlifObject* value = _alifSys_getObject(thread->interpreter, _name);

	if (_alifErr_occurred(thread)) {
		//alifErr_formatUnraisable("Exception ignored in alifSys_getObject()");
	}
	_alifErr_setRaisedException(thread, exc_);
	return value;
}


static AlifIntT sys_setObject(AlifInterpreter* _interp,
	AlifObject* _key, AlifObject* _v) { // 120
	if (_key == nullptr) {
		return -1;
	}
	AlifObject* sd = _interp->sysDict;
	if (_v == nullptr) {
		if (alifDict_pop(sd, _key, nullptr) < 0) {
			return -1;
		}
		return 0;
	}
	else {
		return alifDict_setItem(sd, _key, _v);
	}
}

AlifIntT _alifSys_setAttr(AlifObject* _key, AlifObject* _v) {
	AlifInterpreter* interp = _alifInterpreter_get();
	return sys_setObject(interp, _key, _v);
}

static AlifIntT should_audit(AlifInterpreter* _interp) { // 177
	if (!_interp) {
		return 0;
	}
	return (_interp->dureRun->auditHooks.head
		or _interp->auditHooks
		or 0);
}

static AlifIntT sys_auditTState(AlifThread* _ts, const char* _event,
	const char* _argFormat, va_list _vArgs) { // 190

	if (!_ts) {
		return 0;
	}

	AlifInterpreter* is = _ts->interpreter;
	if (!should_audit(is)) {
		return 0;
	}

	AlifObject* eventName = nullptr;
	AlifObject* eventArgs = nullptr;
	AlifObject* hooks = nullptr;
	AlifObject* hook = nullptr;
	AlifIntT res = -1;
	AlifAuditHookEntry* e{};
	AlifIntT dtrace = 0;


	AlifObject* exc = _alifErr_getRaisedException(_ts);

	if (_argFormat and _argFormat[0]) {
		eventArgs = alif_vaBuildValue(_argFormat, _vArgs);
		if (eventArgs and !ALIFTUPLE_CHECK(eventArgs)) {
			AlifObject* argTuple = alifTuple_pack(1, eventArgs);
			ALIF_SETREF(eventArgs, argTuple);
		}
	}
	else {
		eventArgs = alifTuple_new(0);
	}
	if (!eventArgs) {
		goto exit;
	}

	e = is->dureRun->auditHooks.head;
	for (; e; e = e->next) {
		if (e->hookCFunction(_event, eventArgs, e->userData) < 0) {
			goto exit;
		}
	}

	/* Dtrace USDT point */
	if (dtrace) {
		//alifDTrace_audit(_event, (void*)eventArgs);
	}

	/* Call interpreter hooks */
	if (is->auditHooks) {
		eventName = alifUStr_fromString(_event);
		if (!eventName) {
			goto exit;
		}

		hooks = alifObject_getIter(is->auditHooks);
		if (!hooks) {
			goto exit;
		}

		/* Disallow tracing in hooks unless explicitly enabled */
		alifThreadState_enterTracing(_ts);
		while ((hook = alifIter_next(hooks)) != nullptr) {
			AlifObject* o;
			AlifIntT canTrace = alifObject_getOptionalAttr(hook, &ALIF_ID(__cantrace__), &o);
			if (o) {
				canTrace = alifObject_isTrue(o);
				ALIF_DECREF(o);
			}
			if (canTrace < 0) {
				break;
			}
			if (canTrace) {
				alifThreadState_leaveTracing(_ts);
			}
			AlifObject* args[2] = { eventName, eventArgs };
			o = alifObject_vectorCallThread(_ts, hook, args, 2, nullptr);
			if (canTrace) {
				alifThreadState_enterTracing(_ts);
			}
			if (!o) {
				break;
			}
			ALIF_DECREF(o);
			ALIF_CLEAR(hook);
		}
		alifThreadState_leaveTracing(_ts);
		if (_alifErr_occurred(_ts)) {
			goto exit;
		}
	}

	res = 0;

exit:
	ALIF_XDECREF(hook);
	ALIF_XDECREF(hooks);
	ALIF_XDECREF(eventName);
	ALIF_XDECREF(eventArgs);

	if (!res) {
		_alifErr_setRaisedException(_ts, exc);
	}
	else {
		ALIF_XDECREF(exc);
	}

	return res;
}


AlifIntT _alifSys_audit(AlifThread* _tState, const char* _event,
	const char* _argFormat, ...) { // 318
	va_list vArgs{};
	va_start(vArgs, _argFormat);
	AlifIntT res = sys_auditTState(_tState, _event, _argFormat, vArgs);
	va_end(vArgs);
	return res;
}

AlifIntT alifSys_audit(const char* _event, const char* _argFormat, ...) { // 329
	AlifThread* tstate = alifThread_get();
	va_list vArgs{};
	va_start(vArgs, _argFormat);
	AlifIntT res = sys_auditTState(tstate, _event, _argFormat, vArgs);
	va_end(vArgs);
	return res;
}


static AlifObject* sys_excepthookImpl(AlifObject* module, AlifObject* exctype,
	AlifObject* value, AlifObject* traceback) { // 791
	alifErr_display(nullptr, value, traceback);
	return ALIF_NONE;
}



static AlifObject* sys_exitImpl(AlifObject* _module, AlifObject* _status) { // 880
	//alifErr_setObject(_alifExcSystemExit_, _status);
	return nullptr;
}

static AlifObject* sys_getTraceImpl(AlifObject* _module) { // 1150
	AlifThread* tState = _alifThread_get();
	AlifObject* temp = tState->traceObj;

	if (temp == nullptr)
		temp = ALIF_NONE;
	return ALIF_NEWREF(temp);
}

static AlifObject* sys_setRecursionLimitImpl(AlifObject* _module, AlifIntT _newLimit) { // 1298
	AlifThread* tState = alifThread_get();

	if (_newLimit < 1) {
		_alifErr_setString(tState, _alifExcValueError_,
			"recursion limit must be greater or equal than 1");
		return nullptr;
	}

	AlifIntT depth = tState->alifRecursionLimit - tState->alifRecursionRemaining;
	if (depth >= _newLimit) {
		//_alifErr_format(tState, _alifExcRecursionError_,
			//"cannot set the recursion limit to %i at "
			//"the recursion depth %i: the limit is too low",
			//_newLimit, depth);
		return nullptr;
	}

	alif_setRecursionLimit(_newLimit);
	return ALIF_NONE;
}

static AlifObject* sys_getRecursionLimitImpl(AlifObject* _module) { // 1558
	return alifLong_fromLong(alif_getRecursionLimit());
}

AlifSizeT _alifSys_getSizeOf(AlifObject* _o) { // 1868
	AlifObject* res = nullptr;
	AlifObject* method{};
	AlifSizeT size{};
	AlifThread* tstate = alifThread_get();

	/* Make sure the type is initialized. float gets initialized late */
	if (alifType_ready(ALIF_TYPE(_o)) < 0) {
		return (size_t)-1;
	}

	method = alifObject_lookupSpecial(_o, &ALIF_ID(__sizeof__));
	if (method == nullptr) {
		if (!_alifErr_occurred(tstate)) {
			_alifErr_format(tstate, _alifExcTypeError_,
				"Type %.100s doesn't define __sizeof__",
				ALIF_TYPE(_o)->name);
		}
	}
	else {
		res = _alifObject_callNoArgs(method);
		ALIF_DECREF(method);
	}

	if (res == nullptr)
		return (size_t)-1;

	size = alifLong_asSizeT(res);
	ALIF_DECREF(res);
	if (size == -1 and _alifErr_occurred(tstate))
		return (size_t)-1;

	if (size < 0) {
		_alifErr_setString(tstate, _alifExcValueError_,
			"__sizeof__() should return >= 0");
		return (size_t)-1;
	}

	size_t presize = 0;
	if (!ALIF_IS_TYPE(_o, &_alifTypeType_) or
		alifType_hasFeature((AlifTypeObject*)_o, ALIF_TPFLAGS_HEAPTYPE))
	{
		/* Add the size of the pre-header if "o" is not a static type */
		presize = alifType_preHeaderSize(ALIF_TYPE(_o));
	}

	return (size_t)size + presize;
}


static AlifObject* sys_getSizeOf(AlifObject* _self, AlifObject* _args, AlifObject* _kwds) { // 1919
	static const char* kwList[] = { "object", "default", 0 };
	AlifSizeT size{};
	AlifObject* o{}, * dflt = nullptr;
	AlifThread* tState = alifThread_get();

	if (!alifArg_parseTupleAndKeywords(_args, _kwds, "O|O:getsizeof",
		kwList, &o, &dflt)) {
		return nullptr;
	}

	size = _alifSys_getSizeOf(o);

	if (size == (size_t)-1 and _alifErr_occurred(tState)) {
		//if (dflt != nullptr and _alifErr_exceptionMatches(tState, _alifExcTypeError_)) {
			//_alifErr_clear(tState);
			//return ALIF_NEWREF(dflt);
		//}
		//else
			return nullptr;
	}

	return alifLong_fromSizeT(size);
}

static AlifMethodDef _sysMethods_[] = { // 2550
	SYS_EXCEPTHOOK_METHODDEF,
	SYS_EXIT_METHODDEF
	{"حجم", ALIF_CPPFUNCTION_CAST(sys_getSizeOf),
	 METHOD_VARARGS | METHOD_KEYWORDS},
	SYS_GETTRACE_METHODDEF
	SYS_SETRECURSIONLIMIT_METHODDEF
	SYS_GETRECURSIONLIMIT_METHODDEF
	{nullptr, nullptr}  // sentinel
};



static AlifObject* listBuiltin_moduleNames(void) { // 2628
	AlifObject* tuple{};

	AlifObject* list = _alifImport_getBuiltinModuleNames();
	if (list == nullptr) {
		return nullptr;
	}
	if (alifList_sort(list) != 0) {
		goto error;
	}
	tuple = alifList_asTuple(list);
	ALIF_DECREF(list);
	return tuple;

error:
	ALIF_DECREF(list);
	return nullptr;
}



static AlifTypeObject _flagsType_; // 3094

static AlifStructSequenceField _flagsFields_[] = {
	{"interactive",             "-i"},
	{"optimize",                "-O or -OO"},
	{"safePath",					 "-P"},
	{"intMaxStrDigits",      "-X int_max_str_digits"},
	{0}
};

#define SYS_FLAGS_INT_MAX_STR_DIGITS 17

static AlifStructSequenceDesc _flagsDesc_ = {
	.name = "النظام.اعلام",        /* name */
	.fields = _flagsFields_,       /* fields */
	.nInSequence = 4,
};


static void sys_setFlag(AlifObject* _flags, AlifSizeT _pos, AlifObject* _value) { // 3128
	AlifObject* oldValue = ALIFTUPLE_GET_ITEM(_flags, _pos); //* alif
	ALIFSTRUCTSEQUENCE_SET_ITEM(_flags, _pos, ALIF_NEWREF(_value));
	ALIF_XDECREF(oldValue);
}



static AlifIntT setFlags_fromConfig(AlifInterpreter* _interp, AlifObject* _flags) { // 3170
	//const AlifPreConfig* preconfig = &_interp->dureRun->preConfig;
	const AlifConfig* config = alifInterpreter_getConfig(_interp);

	AlifSizeT pos = 0;
#define SetFlagObj(expr) \
    do { \
        AlifObject *value = (expr); \
        if (value == nullptr) { \
            return -1; \
        } \
        sys_setFlag(_flags, pos, value); \
        ALIF_DECREF(value); \
        pos++; \
    } while (0)
#define SetFlag(expr) SetFlagObj(alifLong_fromLong(expr))


	SetFlag(config->interactive);
	SetFlag(config->optimizationLevel);
	//SetFlag(!config->writeBytecode);
	//SetFlag(!config->userSiteDirectory);
	//SetFlag(!config->siteImport);
	//SetFlag(!config->useEnvironment);
	//SetFlag(config->useHashSeed == 0 or config->hashSeed != 0);
	//SetFlag(config->isolated);
	//SetFlag(config->utf8Mode);
	SetFlagObj(alifBool_fromLong(config->safePath));
	SetFlag(config->intMaxStrDigits);

#undef SetFlagObj
#undef SetFlag
	return 0;
}

static AlifObject* make_flags(AlifInterpreter* interp) { // 3225
	AlifObject* flags = alifStructSequence_new(&_flagsType_);
	if (flags == nullptr) {
		return nullptr;
	}

	if (setFlags_fromConfig(interp, flags) < 0) {
		ALIF_DECREF(flags);
		return nullptr;
	}
	return flags;
}






static AlifModuleDef _sysModule_ = { // 3447
	ALIFMODULEDEF_HEAD_INIT,
	"النظام",
	0, //_sysDoc_
	-1, /* multiple "initialization" just copies the module dict. */
	_sysMethods_,
	nullptr,
	nullptr,
	nullptr,
	nullptr
};


 // 3460
#define SET_SYS(key, value)                                \
    do {                                                   \
        AlifObject *v = (value);                             \
        if (v == nullptr) {                                   \
            goto err_occurred;                             \
        }                                                  \
        res = alifDict_setItemString(sysdict, key, v);       \
        ALIF_DECREF(v);                                      \
        if (res < 0) {                                     \
            goto err_occurred;                             \
        }                                                  \
    } while (0)

#define SET_SYS_FROM_STRING(_key, _value) \
        SET_SYS(_key, alifUStr_fromString(_value))

static AlifIntT _alifSys_initCore(AlifThread* tstate, AlifObject* sysdict) { // 3476
	AlifObject* version_info{};
	AlifIntT res{};
	AlifInterpreter* interp = tstate->interpreter;

#undef COPY_SYS_ATTR

	SET_SYS_FROM_STRING("version", alif_getVersion());
	/* initialize hash_info */

	SET_SYS("builtin_module_names", listBuiltin_moduleNames()); //* review
	//SET_SYS("stdlib_module_names", listStdlib_moduleNames());
#if ALIF_BIG_ENDIAN
	SET_SYS_FROM_STRING("byteorder", "big");
#else
	SET_SYS_FROM_STRING("byteorder", "little");
#endif


#define ENSURE_INFO_TYPE(_type, _desc) \
    do { \
        if (_alifStructSequence_initBuiltinWithFlags( \
                interp, &_type, &_desc, ALIF_TPFLAGS_DISALLOW_INSTANTIATION) < 0) { \
            goto type_init_failed; \
        } \
    } while (0)

	ENSURE_INFO_TYPE(_flagsType_, _flagsDesc_);
	SET_SYS("flags", make_flags(tstate->interpreter));


	/* adding sys.path_hooks and sys.path_importer_cache */
	SET_SYS("meta_path", alifList_new(0));
	SET_SYS("path_importer_cache", alifDict_new());
	SET_SYS("path_hooks", alifList_new(0));

	if (_alifErr_occurred(tstate)) {
		goto err_occurred;
	}
	return 1;

type_init_failed:
	//return _ALIFSTATUS_ERR("failed to initialize a type");
	return -1; //* alif

err_occurred:
	//return _ALIFSTATUS_ERR("can't initialize sys module");
	return -1; //* alif
}



AlifIntT _alifSys_updateConfig(AlifThread* _thread) { // 3645
	AlifInterpreter* interp = _thread->interpreter;
	AlifObject* sysdict = interp->sysDict;

	AlifObject* flags{}; //* alif
	const wchar_t* stdlibdir{}; //* alif

	const AlifConfig* config = alifInterpreter_getConfig(interp);
	AlifIntT res{};

#define COPY_LIST(_key, _value) \
        SET_SYS(_key, _alifWStringList_asList(&(_value)));

#define SET_SYS_FROM_WSTR(_key, _value) \
        SET_SYS(_key, alifUStr_fromWideChar(_value, -1));

#define COPY_WSTR(_sysAttr, _wstr) \
    if (_wstr != nullptr) { \
        SET_SYS_FROM_WSTR(_sysAttr, _wstr); \
    }

	if (config->moduleSearchPathsSet) {
		COPY_LIST("Path", config->moduleSearchPaths);
	}

	//COPY_WSTR("executable", config->executable);
	//COPY_WSTR("_base_executable", config->baseExecutable);
	//COPY_WSTR("prefix", config->prefix);
	//COPY_WSTR("base_prefix", config->basePrefix);
	//COPY_WSTR("exec_prefix", config->execPrefix);
	//COPY_WSTR("base_exec_prefix", config->baseExecPrefix);
	//COPY_WSTR("platlibdir", config->platlibdir);

	//if (config->alifCachePrefix != nullptr) {
	//	SET_SYS_FROM_WSTR("alifCachePrefix", config->alifCachePrefix);
	//}
	//else {
	//	if (alifDict_setItemString(sysdict, "alifCachePrefix", ALIF_NONE) < 0) {
	//		return -1;
	//	}
	//}

	COPY_LIST("argv", config->argv);
	COPY_LIST("origArgv", config->origArgv);
	//COPY_LIST("warnoptions", config->warnOptions);


	//stdlibdir = _alif_getStdLibDir();
	if (stdlibdir != nullptr) {
		SET_SYS_FROM_WSTR("_stdlibDir", stdlibdir);
	}
	else {
		if (alifDict_setItemString(sysdict, "_stdlibDir", ALIF_NONE) < 0) {
			return -1;
		}
	}

#undef SET_SYS_FROM_WSTR
#undef COPY_LIST
#undef COPY_WSTR

	// sys.flags
	flags = _alifSys_getObject(interp, "flags"); // borrowed ref
	if (flags == nullptr) {
		if (!_alifErr_occurred(_thread)) {
			//_alifErr_setString(_thread, _alifExcRuntimeError_, "lost sys.flags");
		}
		return -1;
	}
	if (setFlags_fromConfig(interp, flags) < 0) {
		return -1;
	}

	if (_alifErr_occurred(_thread)) {
		goto err_occurred;
	}

	return 0;

err_occurred:
	return -1;
}

#undef SET_SYS
#undef SET_SYS_FROM_STRING


AlifIntT alifSys_create(AlifThread* _thread, AlifObject** _sysModP) { // 3779

	AlifIntT status{};
	AlifIntT err{};
	AlifObject* sysmod{};
	AlifObject* sysdict{};
	AlifObject* monitoring{};
	AlifInterpreter* interp = _thread->interpreter;

	AlifObject* modules = alifImport_initModules(interp);
	if (modules == nullptr) {
		goto error;
	}

	sysmod = alifModule_createInitialized(&_sysModule_/*, ALIF_API_VERSION*/); //* alif
	if (sysmod == nullptr) {
		//return ALIFSTATUS_ERR("failed to create a module object");
		return -1; // temp
	}
	//alifUnstableModule_setGIL(sysmod, ALIF_MOD_GIL_NOT_USED);

	sysdict = alifModule_getDict(sysmod);
	if (sysdict == nullptr) {
		goto error;
	}
	interp->sysDict = ALIF_NEWREF(sysdict);

	//interp->sysDictCopy = alifDict_copy(sysdict);
	//if (interp->sysDictCopy == nullptr) {
	//	goto error;
	//}

	if (alifDict_setItemString(sysdict, "modules", modules) < 0) {
		goto error;
	}

	//status = _alifSys_setPreliminaryStderr(sysdict);
	//if (status < 1) {
	//	return status;
	//}

	status = _alifSys_initCore(_thread, sysdict);
	if (status < 1) {
		return status;
	}

	if (_alifImport_fixupBuiltin(_thread, sysmod, "النظام", modules) < 0) {
		goto error;
	}

//	monitoring = _alif_createMonitoringObject();
//	if (monitoring == nullptr) {
//		goto error;
//	}
//	err = alifDict_setItemString(sysdict, "monitoring", monitoring);
//	ALIF_DECREF(monitoring);
//	if (err < 0) {
//		goto error;
//	}


	*_sysModP = sysmod;
	return 1;

error:
	//return ALIFSTATUS_ERR("can't initialize sys module");
	return -1; //* alif
}
