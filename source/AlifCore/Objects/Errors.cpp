#include "alif.h"

#include "AlifCore_Call.h"
#include "AlifCore_InitConfig.h"
#include "AlifCore_Errors.h"
#include "AlifCore_State.h"
#include "AlifCore_StructSeq.h"
#include "AlifCore_SysModule.h"
#include "AlifCore_Traceback.h"





void _alifErr_setRaisedException(AlifThread* _thread, AlifObject* _exc) { // 24
	AlifObject* old_exc = _thread->currentException;
	_thread->currentException = _exc;
	ALIF_XDECREF(old_exc);
}


static AlifObject* _alifErr_createException(AlifObject* _exceptionType,
	AlifObject* _value) { // 32
	AlifObject* exc{};

	if (_value == nullptr or _value == ALIF_NONE) {
		exc = _alifObject_callNoArgs(_exceptionType);
	}
	else if (ALIFTUPLE_CHECK(_value)) {
		exc = alifObject_call(_exceptionType, _value, nullptr);
	}
	else {
		exc = alifObject_callOneArg(_exceptionType, _value);
	}

	if (exc != nullptr and !ALIFEXCEPTIONINSTANCE_CHECK(exc)) {
		alifErr_format(nullptr /*_alifExcTypeError_*/,
			"calling %R should have returned an instance of "
			"BaseException, not %s",
			_exceptionType, ALIF_TYPE(exc)->name);
		ALIF_CLEAR(exc);
	}

	return exc;
}


void _alifErr_restore(AlifThread* _thread, AlifObject* _type, AlifObject* _value,
	AlifObject* _traceback) { // 58
	if (_type == nullptr) {
		_alifErr_setRaisedException(_thread, nullptr);
		return;
	}
	if (_value != nullptr and _type == (AlifObject*)ALIF_TYPE(_value)) {
		/* Already normalized */
	}
	else {
		AlifObject* exc = _alifErr_createException(_type, _value);
		ALIF_XDECREF(_value);
		if (exc == nullptr) {
			ALIF_DECREF(_type);
			ALIF_XDECREF(_traceback);
			return;
		}
		_value = exc;
	}

	if (_traceback != nullptr and !ALIFTRACEBACK_CHECK(_traceback)) {
		if (_traceback == ALIF_NONE) {
			ALIF_DECREF(ALIF_NONE);
			_traceback = nullptr;
		}
		else {
			//alifErr_setString(_alifExcTypeError_, "traceback must be a Traceback or None");
			ALIF_XDECREF(_value);
			ALIF_DECREF(_type);
			ALIF_XDECREF(_traceback);
			return;
		}
	}
	AlifObject* old_traceback = ((AlifBaseExceptionObject*)_value)->traceback;
	((AlifBaseExceptionObject*)_value)->traceback = _traceback;
	ALIF_XDECREF(old_traceback);
	_alifErr_setRaisedException(_thread, _value);
	ALIF_DECREF(_type);
}






void alifErr_restore(AlifObject* _type,
	AlifObject* _value, AlifObject* _traceback) { // 104
	AlifThread* tstate = _alifThread_get();
	_alifErr_restore(tstate, _type, _value, _traceback);
}


void alifErr_setRaisedException(AlifObject* _exc) { // 111
	AlifThread* thread = _alifThread_get();
	_alifErr_setRaisedException(thread, _exc);
}


AlifErrStackItem* _alifErr_getTopMostException(AlifThread* _thread) { // 118
	AlifErrStackItem* excInfo = _thread->excInfo;

	while (excInfo->excValue == nullptr
		and excInfo->previousItem != nullptr)
	{
		excInfo = excInfo->previousItem;
	}
	return excInfo;
}



static AlifObject* getNormalization_failureNote(AlifThread* tstate,
	AlifObject* exception, AlifObject* value) { // 132
	AlifObject* args = alifObject_repr(value);
	if (args == nullptr) {
		_alifErr_clear(tstate);
		args = alifUStr_fromFormat("<unknown>");
	}
	AlifObject* note{};
	const char* tpname = ((AlifTypeObject*)exception)->name;
	if (args == nullptr) {
		_alifErr_clear(tstate);
		note = alifUStr_fromFormat("Normalization failed: type=%s", tpname);
	}
	else {
		note = alifUStr_fromFormat("Normalization failed: type=%s args=%S",
			tpname, args);
		ALIF_DECREF(args);
	}
	return note;
}




void _alifErr_setObject(AlifThread* _thread,
	AlifObject* _exception, AlifObject* _value) { // 154
	AlifObject* excValue{};
	AlifObject* tb = nullptr;

	if (_exception != nullptr and
		not ALIFEXCEPTIONCLASS_CHECK(_exception)) {
		//_alifErr_format(_thread, _alifExcSystemError_,
		//	"_alifErr_setObject: "
		//	"exception %R is not a BaseException subclass",
		//	_exception);
		return;
	}
	/* Normalize the exception */
	AlifIntT isSubclass = 0;
	if (_value != nullptr and ALIFEXCEPTIONINSTANCE_CHECK(_value)) {
		isSubclass = alifObject_isSubclass((AlifObject*)ALIF_TYPE(_value), _exception);
		if (isSubclass < 0) {
			return;
		}
	}
	ALIF_XINCREF(_value);
	if (!isSubclass) {
		_alifErr_clear(_thread);

		AlifObject* fixed_value = _alifErr_createException(_exception, _value);
		if (fixed_value == nullptr) {
			AlifObject* exc = _alifErr_getRaisedException(_thread);

			AlifObject* note = getNormalization_failureNote(_thread, _exception, _value);
			ALIF_XDECREF(_value);
			if (note != nullptr) {
				_alifException_addNote(exc, note);
				ALIF_DECREF(note);
			}
			_alifErr_setRaisedException(_thread, exc);
			return;
		}
		ALIF_XSETREF(_value, fixed_value);
	}

	excValue = _alifErr_getTopMostException(_thread)->excValue;
	if (excValue != nullptr and excValue != ALIF_NONE) {
		ALIF_INCREF(excValue);
		if (excValue != _value) {
			AlifObject* o_ = excValue, * context;
			AlifObject* slowObj = o_; 
			AlifIntT slowUpdateToggle = 0;
			while ((context = alifException_getContext(o_))) {
				ALIF_DECREF(context);
				if (context == _value) {
					alifException_setContext(o_, nullptr);
					break;
				}
				o_ = context;
				if (o_ == slowObj) {
					break;
				}
				if (slowUpdateToggle) {
					slowObj = alifException_getContext(slowObj);
					ALIF_DECREF(slowObj);
				}
				slowUpdateToggle = not slowUpdateToggle;
			}
			alifException_setContext(_value, excValue);
		}
		else {
			ALIF_DECREF(excValue);
		}
	}
	if (ALIFEXCEPTIONINSTANCE_CHECK(_value)) {
		tb = alifException_getTraceback(_value);
	}
	_alifErr_restore(_thread, ALIF_NEWREF(ALIF_TYPE(_value)), _value, tb);
}







void alifErr_setObject(AlifObject* _exception, AlifObject* _value) { // 246
	AlifThread* thread = _alifThread_get();
	_alifErr_setObject(thread, _exception, _value);
}


void _alifErr_setString(AlifThread* _thread, AlifObject* _exception,
	const char* _string) { // 285
	AlifObject* value = alifUStr_fromString(_string);
	if (value != nullptr) {
		_alifErr_setObject(_thread, _exception, value);
		ALIF_DECREF(value);
	}
}


void alifErr_setString(AlifObject* _exception, const char* _string) { // 296
	AlifThread* thread = _alifThread_get();
	_alifErr_setString(thread, _exception, _string);
}


AlifObject* ALIF_HOT_FUNCTION alifErr_occurred(void) { // 304
	AlifThread* thread = _alifThread_get();
	return _alifErr_occurred(thread);
}


AlifIntT alifErr_givenExceptionMatches(AlifObject* _err, AlifObject* _exc) { // 315
	if (_err == nullptr or _exc == nullptr) {
		return 0;
	}
	if (ALIFTUPLE_CHECK(_exc)) {
		AlifSizeT i{}, n{};
		n = alifTuple_size(_exc);
		for (i = 0; i < n; i++) {
			/* Test recursively */
			if (alifErr_givenExceptionMatches(
				_err, ALIFTUPLE_GET_ITEM(_exc, i)))
			{
				return 1;
			}
		}
		return 0;
	}
	/* err might be an instance, so check its class. */
	if (ALIFEXCEPTIONINSTANCE_CHECK(_err))
		_err = ALIFEXCEPTIONINSTANCE_CLASS(_err);

	if (ALIFEXCEPTIONCLASS_CHECK(_err) and ALIFEXCEPTIONCLASS_CHECK(_exc)) {
		return alifType_isSubType((AlifTypeObject*)_err, (AlifTypeObject*)_exc);
	}

	return _err == _exc;
}

AlifIntT _alifErr_exceptionMatches(AlifThread* _thread, AlifObject* _exc) { // 347
	return alifErr_givenExceptionMatches(_alifErr_occurred(_thread), _exc);
}


AlifIntT alifErr_exceptionMatches(AlifObject* _exc) { // 354
	AlifThread* thread = _alifThread_get();
	return _alifErr_exceptionMatches(thread, _exc);
}


// 362
#ifndef ALIF_NORMALIZE_RECURSION_LIMIT
#define ALIF_NORMALIZE_RECURSION_LIMIT 32
#endif

void _alifErr_normalizeException(AlifThread* _tState, AlifObject** _exc,
	AlifObject** _val, AlifObject** _tb) { // 373
	AlifIntT recursionDepth = 0;
	_tState->recursionHeadroom++;
	AlifObject* type{}, * value{}, * initialTb{};

restart:
	type = *_exc;
	if (type == nullptr) {
		/* There was no exception, so nothing to do. */
		_tState->recursionHeadroom--;
		return;
	}

	value = *_val;

	if (!value) {
		value = ALIF_NEWREF(ALIF_NONE);
	}

	/* Normalize the exception so that if the type is a class, the
	   value will be an instance.
	*/
	if (ALIFEXCEPTIONCLASS_CHECK(type)) {
		AlifObject* inclass = nullptr;
		AlifIntT isSubclass = 0;

		if (ALIFEXCEPTIONINSTANCE_CHECK(value)) {
			inclass = ALIFEXCEPTIONINSTANCE_CLASS(value);
			isSubclass = alifObject_isSubclass(inclass, type);
			if (isSubclass < 0) {
				goto error;
			}
		}

		/* If the value was not an instance, or is not an instance
		   whose class is (or is derived from) type, then use the
		   value as an argument to instantiation of the type
		   class.
		*/
		if (!isSubclass) {
			AlifObject* fixed_value = _alifErr_createException(type, value);
			if (fixed_value == nullptr) {
				goto error;
			}
			ALIF_SETREF(value, fixed_value);
		}
		/* If the class of the instance doesn't exactly match the
		   class of the type, believe the instance.
		*/
		else if (inclass != type) {
			ALIF_SETREF(type, ALIF_NEWREF(inclass));
		}
	}
	*_exc = type;
	*_val = value;
	_tState->recursionHeadroom--;
	return;

error:
	ALIF_DECREF(type);
	ALIF_DECREF(value);
	recursionDepth++;
	if (recursionDepth == ALIF_NORMALIZE_RECURSION_LIMIT) {
		//_alifErr_setString(_tState, _alifExcRecursionError_,
			//"maximum recursion depth exceeded "
			//"while normalizing an exception");
	}
	/* If the new exception doesn't set a traceback and the old
	   exception had a traceback, use the old traceback for the
	   new exception.  It's better than nothing.
	*/
	initialTb = *_tb;
	_alifErr_fetch(_tState, _exc, _val, _tb);
	if (initialTb != nullptr) {
		if (*_tb == nullptr)
			*_tb = initialTb;
		else
			ALIF_DECREF(initialTb);
	}
	/* Abort when ALIF_NORMALIZE_RECURSION_LIMIT has been exceeded, and the
	   corresponding RecursionError could not be normalized, and the
	   MemoryError raised when normalize this RecursionError could not be
	   normalized. */
	if (recursionDepth >= ALIF_NORMALIZE_RECURSION_LIMIT + 2) {
		if (alifErr_givenExceptionMatches(*_exc, _alifExc_memoryError_)) {
			//alif_fatalError("Cannot recover from MemoryErrors "
				//"while normalizing exceptions.");
		}
		else {
			//alif_fatalError("Cannot recover from the recursive normalization "
				//"of an exception.");
		}
	}
	goto restart;
}


AlifObject* _alifErr_getRaisedException(AlifThread* _thread) { // 483
	AlifObject* exc = _thread->currentException;
	_thread->currentException = nullptr;
	return exc;
}

AlifObject* alifErr_getRaisedException(void) { // 490
	AlifThread* tstate = _alifThread_get();
	return _alifErr_getRaisedException(tstate);
}


void _alifErr_fetch(AlifThread* _tState, AlifObject** _pType, AlifObject** _pValue,
	AlifObject** _pTraceback) { // 497
	AlifObject* exc = _alifErr_getRaisedException(_tState);
	*_pValue = exc;
	if (exc == nullptr) {
		*_pType = nullptr;
		*_pTraceback = nullptr;
	}
	else {
		*_pType = ALIF_NEWREF(ALIF_TYPE(exc));
		*_pTraceback = ALIF_XNEWREF(((AlifBaseExceptionObject*)exc)->traceback);
	}
}

void alifErr_fetch(AlifObject** _pType, AlifObject** _pValue,
	AlifObject** _pTraceback) { // 514
	AlifThread* tstate = _alifThread_get();
	_alifErr_fetch(tstate, _pType, _pValue, _pTraceback);
}


void _alifErr_clear(AlifThread* _thread) { // 522
	_alifErr_restore(_thread, nullptr, nullptr, nullptr);
}


void alifErr_clear(void) { // 529
	AlifThread* tstate = _alifThread_get();
	_alifErr_clear(tstate);
}


void _alifErr_setHandledException(AlifThread* _thread, AlifObject* _exc) { // 593
	ALIF_XSETREF(_thread->excInfo->excValue, ALIF_XNEWREF(_exc == ALIF_NONE ? nullptr : _exc));
}

void alifErr_setHandledException(AlifObject* _exc) { // 599
	AlifThread* thread = _alifThread_get();
	_alifErr_setHandledException(thread, _exc);
}


void _alifErr_chainExceptions1(AlifObject* _exc) { // 686
	if (_exc == nullptr) {
		return;
	}
	AlifThread* thread = _alifThread_get();
	if (_alifErr_occurred(thread)) {
		AlifObject* exc2 = _alifErr_getRaisedException(thread);
		alifException_setContext(exc2, _exc);
		_alifErr_setRaisedException(thread, exc2);
	}
	else {
		_alifErr_setRaisedException(thread, _exc);
	}
}

AlifObject* alifErr_setFromErrnoWithFileNameObjects(AlifObject* _exc, AlifObject* _fileNameObject, AlifObject* _fileNameObject2) { // 790
	AlifThread* tstate = _alifThread_get();
	AlifObject* message{};
	AlifObject* v{}, * args{};
	AlifIntT i = errno;
#ifdef _WINDOWS
	WCHAR* sBuf = nullptr;
#endif /* Unix/Windows */

#ifdef EINTR
	if (i == EINTR and alifErr_checkSignals())
		return nullptr;
#endif

#ifndef _WINDOWS
	if (i != 0) {
		const char* s = strerror(i);
		message = alifUStr_decodeLocale(s, "surrogateescape");
	}
	else {
		/* Sometimes errno didn't get set */
		message = alifUStr_fromString("Error");
	}
#else
	if (i == 0)
		message = alifUStr_fromString("Error"); /* Sometimes errno didn't get set */
	else
	{
		if (i > 0 and i < _sys_nerr) {
			message = alifUStr_fromString(_sys_errlist[i]);
		}
		else {
			AlifIntT len = FormatMessageW(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,                   /* no message source */
				i,
				MAKELANGID(LANG_NEUTRAL,
					SUBLANG_DEFAULT),
				/* Default language */
				(LPWSTR)&sBuf,
				0,                      /* size not used */
				nullptr);                  /* no args */
			if (len == 0) {
				/* Only ever seen this in out-of-mem
				   situations */
				sBuf = nullptr;
				message = alifUStr_fromFormat("Windows Error 0x%x", i);
			}
			else {
				/* remove trailing cr/lf and dots */
				while (len > 0 and (sBuf[len - 1] <= L' ' or sBuf[len - 1] == L'.'))
					sBuf[--len] = L'\0';
				message = alifUStr_fromWideChar(sBuf, len);
			}
		}
	}
#endif /* Unix/Windows */

	if (message == nullptr)
	{
#ifdef _WINDOWS
		LocalFree(sBuf);
#endif
		return nullptr;
	}

	if (_fileNameObject != nullptr) {
		if (_fileNameObject2 != nullptr)
			args = alif_buildValue("(iOOiO)", i, message, _fileNameObject, 0, _fileNameObject2);
		else
			args = alif_buildValue("(iOO)", i, message, _fileNameObject);
	}
	else {
		args = alif_buildValue("(iO)", i, message);
	}
	ALIF_DECREF(message);

	if (args != nullptr) {
		v = alifObject_call(_exc, args, nullptr);
		ALIF_DECREF(args);
		if (v != nullptr) {
			_alifErr_setObject(tstate, (AlifObject*)ALIF_TYPE(v), v);
			ALIF_DECREF(v);
		}
	}
#ifdef _WINDOWS
	LocalFree(sBuf);
#endif
	return nullptr;
}

AlifObject* alifErr_setFromErrnoWithFilename(AlifObject* _exc, const char* _fileName) { // 889
	AlifObject* name = nullptr;
	if (_fileName) {
		AlifIntT i = errno;
		name = alifUStr_decodeFSDefault(_fileName);
		if (name == nullptr) {
			return nullptr;
		}
		errno = i;
	}
	AlifObject* result = alifErr_setFromErrnoWithFileNameObjects(_exc, name, nullptr);
	ALIF_XDECREF(name);
	return result;
}


AlifObject* alifErr_setFromErrno(AlifObject* _exc) { // 906
	return alifErr_setFromErrnoWithFileNameObjects(_exc, nullptr, nullptr);
}

#ifdef _WINDOWS // 911


AlifObject* alifErr_setExcFromWindowsErrWithFilenameObjects(
	AlifObject* _exc,
	AlifIntT _ierr,
	AlifObject* _fileNameObject,
	AlifObject* _fileNameObject2) { // 922
	AlifThread* tstate = alifThread_get();
	AlifIntT len{};
	WCHAR* sBuf = nullptr; /* Free via LocalFree */
	AlifObject* message;
	AlifObject* args{}, * v{};

	DWORD err = (DWORD)_ierr;
	if (err == 0) {
		err = GetLastError();
	}

	len = FormatMessageW(
		/* Error API error */
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,           /* no message source */
		err,
		MAKELANGID(LANG_NEUTRAL,
			SUBLANG_DEFAULT), /* Default language */
		(LPWSTR)&sBuf,
		0,              /* size not used */
		nullptr);          /* no args */
	if (len == 0) {
		/* Only seen this in out of mem situations */
		message = alifUStr_fromFormat("Windows Error 0x%x", err);
		sBuf = nullptr;
	}
	else {
		/* remove trailing cr/lf and dots */
		while (len > 0 and (sBuf[len - 1] <= L' ' or sBuf[len - 1] == L'.'))
			sBuf[--len] = L'\0';
		message = alifUStr_fromWideChar(sBuf, len);
	}

	if (message == nullptr)
	{
		LocalFree(sBuf);
		return nullptr;
	}

	if (_fileNameObject == nullptr) {
		_fileNameObject = _fileNameObject2 = ALIF_NONE;
	}
	else if (_fileNameObject2 == nullptr)
		_fileNameObject2 = ALIF_NONE;
	/* This is the constructor signature for OSError.
	   The POSIX translation will be figured out by the constructor. */
	args = alif_buildValue("(iOOiO)", 0, message, _fileNameObject, err, _fileNameObject2);
	ALIF_DECREF(message);

	if (args != nullptr) {
		v = alifObject_call(_exc, args, nullptr);
		ALIF_DECREF(args);
		if (v != nullptr) {
			_alifErr_setObject(tstate, (AlifObject*)ALIF_TYPE(v), v);
			ALIF_DECREF(v);
		}
	}
	LocalFree(sBuf);
	return nullptr;
}

AlifObject* alifErr_setExcFromWindowsErrWithFilename(
	AlifObject* _exc,
	AlifIntT _iErr,
	const char* _fileName) { // 991
	AlifObject* name = nullptr;
	if (_fileName) {
		if ((DWORD)_iErr == 0) {
			_iErr = (AlifIntT)GetLastError();
		}
		name = alifUStr_decodeFSDefault(_fileName);
		if (name == nullptr) {
			return nullptr;
		}
	}
	AlifObject* ret = alifErr_setExcFromWindowsErrWithFilenameObjects(_exc,
		_iErr,
		name,
		nullptr);
	ALIF_XDECREF(name);
	return ret;
}

AlifObject* alifErr_setFromWindowsErr(AlifIntT _iErr) { // 1019
	return alifErr_setExcFromWindowsErrWithFilename(nullptr,
		_iErr, nullptr);
}

#endif /* _WINDOWS */


static AlifObject* _alifErr_formatV(AlifThread* _thread, AlifObject* _exception,
	const char* _format, va_list _vargs) { // 1152
	AlifObject* string{};

	_alifErr_clear(_thread);

	string = alifUStr_fromFormatV(_format, _vargs);
	if (string != nullptr) {
		_alifErr_setObject(_thread, _exception, string);
		ALIF_DECREF(string);
	}
	return nullptr;
}

AlifObject* _alifErr_format(AlifThread* _thread, AlifObject* _exception,
	const char* _format, ...) { // 1179
	va_list vargs{};
	va_start(vargs, _format);
	_alifErr_formatV(_thread, _exception, _format, vargs);
	va_end(vargs);
	return nullptr;
}


AlifObject* alifErr_format(AlifObject* _exception,
	const char* _format, ...) { // 1191
	AlifThread* thread = _alifThread_get();
	va_list vargs{};
	va_start(vargs, _format);
	_alifErr_formatV(thread, _exception, _format, vargs);
	va_end(vargs);
	return nullptr;
}

static AlifTypeObject _unraisableHookArgsType_{}; // 1323


static AlifObject* makeUnraisable_hookArgs(AlifThread* _tState, AlifObject* _excType,
	AlifObject* _excValue, AlifObject* _excTb,
	AlifObject* _errMsg, AlifObject* _obj) { // 1362
	AlifObject* args = alifStructSequence_new(&_unraisableHookArgsType_);
	if (args == nullptr) {
		return nullptr;
	}

	AlifSizeT pos = 0;
#define ADD_ITEM(exc_type) \
        do { \
            if (_excType == nullptr) { \
                _excType = ALIF_NONE; \
            } \
            ALIFSTRUCTSEQUENCE_SET_ITEM(args, pos++, ALIF_NEWREF(_excType)); \
        } while (0)


	ADD_ITEM(_excType);
	ADD_ITEM(_excValue);
	ADD_ITEM(_excTb);
	ADD_ITEM(_errMsg);
	ADD_ITEM(_obj);
#undef ADD_ITEM

	if (_alifErr_occurred(_tState)) {
		ALIF_DECREF(args);
		return nullptr;
	}
	return args;
}

/* Default implementation of sys.unraisablehook.

   It can be called to log the exception of a custom sys.unraisablehook.

   Do nothing if sys.stderr attribute doesn't exist or is set to None. */
static AlifIntT write_unraisable_exc_file(AlifThread* _tState, AlifObject* _excType,
	AlifObject* _excValue, AlifObject* _excTb,
	AlifObject* _errMsg, AlifObject* _obj, AlifObject* _file) { // 1403
	if (_obj != nullptr and _obj != ALIF_NONE) {
		if (_errMsg != nullptr and _errMsg != ALIF_NONE) {
			if (alifFile_writeObject(_errMsg, _file, ALIF_PRINT_RAW) < 0) {
				return -1;
			}
			if (alifFile_writeString(": ", _file) < 0) {
				return -1;
			}
		}
		else {
			if (alifFile_writeString("Exception ignored in: ", _file) < 0) {
				return -1;
			}
		}

		if (alifFile_writeObject(_obj, _file, 0) < 0) {
			_alifErr_clear(_tState);
			if (alifFile_writeString("<object repr() failed>", _file) < 0) {
				return -1;
			}
		}
		if (alifFile_writeString("\n", _file) < 0) {
			return -1;
		}
	}
	else if (_errMsg != nullptr and _errMsg != ALIF_NONE) {
		if (alifFile_writeObject(_errMsg, _file, ALIF_PRINT_RAW) < 0) {
			return -1;
		}
		if (alifFile_writeString(":\n", _file) < 0) {
			return -1;
		}
	}

	if (_excTb != nullptr and _excTb != ALIF_NONE) {
		if (alifTraceBack_print(_excTb, _file) < 0) {
			/* continue even if writing the traceback failed */
			_alifErr_clear(_tState);
		}
	}

	if (_excType == nullptr or _excType == ALIF_NONE) {
		return -1;
	}


	AlifObject* moduleName = alifObject_getAttr(_excType, &ALIF_ID(__module__));
	if (moduleName == nullptr or !ALIFUSTR_CHECK(moduleName)) {
		ALIF_XDECREF(moduleName);
		_alifErr_clear(_tState);
		if (alifFile_writeString("<unknown>", _file) < 0) {
			return -1;
		}
	}
	else {
		if (!_alifUStr_equal(moduleName, &ALIF_ID(Builtins)) and
			!_alifUStr_equal(moduleName, &ALIF_ID(__main__))) {
			if (alifFile_writeObject(moduleName, _file, ALIF_PRINT_RAW) < 0) {
				ALIF_DECREF(moduleName);
				return -1;
			}
			ALIF_DECREF(moduleName);
			if (alifFile_writeString(".", _file) < 0) {
				return -1;
			}
		}
		else {
			ALIF_DECREF(moduleName);
		}
	}

	AlifObject* qualName = alifType_getQualName((AlifTypeObject*)_excType);
	if (qualName == nullptr or !ALIFUSTR_CHECK(qualName)) {
		ALIF_XDECREF(qualName);
		_alifErr_clear(_tState);
		if (alifFile_writeString("<unknown>", _file) < 0) {
			return -1;
		}
	}
	else {
		if (alifFile_writeObject(qualName, _file, ALIF_PRINT_RAW) < 0) {
			ALIF_DECREF(qualName);
			return -1;
		}
		ALIF_DECREF(qualName);
	}

	if (_excValue and _excValue != ALIF_NONE) {
		if (alifFile_writeString(": ", _file) < 0) {
			return -1;
		}
		if (alifFile_writeObject(_excValue, _file, ALIF_PRINT_RAW) < 0) {
			_alifErr_clear(_tState);
			if (alifFile_writeString("<exception str() failed>", _file) < 0) {
				return -1;
			}
		}
	}

	if (alifFile_writeString("\n", _file) < 0) {
		return -1;
	}

	/* Explicitly call file.flush() */
	if (_alifFile_flush(_file) < 0) {
		return -1;
	}

	return 0;
}


static AlifIntT write_unraisableExc(AlifThread* _tState, AlifObject* _excType,
	AlifObject* _excvalue, AlifObject* _excTb, AlifObject* _errMsg,
	AlifObject* _obj) { // 1521
	AlifObject* file = _alifSys_getAttr(_tState, &ALIF_ID(Stderr));
	if (file == nullptr or file == ALIF_NONE) {
		return 0;
	}

	/* Hold a strong reference to ensure that sys.stderr doesn't go away
	   while we use it */
	ALIF_INCREF(file);
	AlifIntT res = write_unraisable_exc_file(_tState, _excType, _excvalue, _excTb,
		_errMsg, _obj, file);
	ALIF_DECREF(file);

	return res;
}

static void format_unraisableV(const char* _format, va_list _va, AlifObject* _obj) { // 1580
	const char* errMsgStr{};
	AlifThread* tstate = alifThread_get();
	ALIF_ENSURETHREADNOTNULL(tstate);

	AlifObject* errMsg = nullptr;
	AlifObject* excType, * exc_value, * excTb;
	_alifErr_fetch(tstate, &excType, &exc_value, &excTb);

	AlifObject* hookArgs{};
	AlifObject* hook{};
	AlifObject* res{};

	if (excType == nullptr) {
		/* sys.unraisablehook requires that at least exc_type is set */
		goto default_hook;
	}

	if (excTb == nullptr) {
		AlifFrameObject* frame = alifThreadState_getFrame(tstate);
		if (frame != nullptr) {
			excTb = _alifTraceBack_fromFrame(nullptr, frame);
			if (excTb == nullptr) {
				_alifErr_clear(tstate);
			}
			ALIF_DECREF(frame);
		}
	}

	_alifErr_normalizeException(tstate, &excType, &exc_value, &excTb);

	if (excTb != nullptr and excTb != ALIF_NONE and ALIFTRACEBACK_CHECK(excTb)) {
		if (alifException_setTraceback(exc_value, excTb) < 0) {
			_alifErr_clear(tstate);
		}
	}

	if (_format != nullptr) {
		errMsg = alifUStr_fromFormatV(_format, _va);
		if (errMsg == nullptr) {
			alifErr_clear();
		}
	}

	hookArgs = makeUnraisable_hookArgs(
		tstate, excType, exc_value, excTb, errMsg, _obj);
	if (hookArgs == nullptr) {
		errMsgStr = ("Exception ignored on building "
			"sys.unraisablehook arguments");
		goto error;
	}

	hook = _alifSys_getAttr(tstate, &ALIF_ID(Unraisablehook));
	if (hook == nullptr) {
		ALIF_DECREF(hookArgs);
		goto default_hook;
	}

	if (_alifSys_audit(tstate, "sys.unraisablehook", "OO", hook, hookArgs) < 0) {
		ALIF_DECREF(hookArgs);
		errMsgStr = "Exception ignored in audit hook";
		_obj = nullptr;
		goto error;
	}

	if (hook == ALIF_NONE) {
		ALIF_DECREF(hookArgs);
		goto default_hook;
	}

	res = alifObject_callOneArg(hook, hookArgs);
	ALIF_DECREF(hookArgs);
	if (res != nullptr) {
		ALIF_DECREF(res);
		goto done;
	}

	/* sys.unraisablehook failed: log its error using default hook */
	_obj = hook;
	errMsgStr = nullptr;

error:
	/* err_msg_str and obj have been updated and we have a new exception */
	ALIF_XSETREF(errMsg, alifUStr_fromString(errMsgStr ?
		errMsgStr : "Exception ignored in sys.unraisablehook"));
	ALIF_XDECREF(excType);
	ALIF_XDECREF(exc_value);
	ALIF_XDECREF(excTb);
	_alifErr_fetch(tstate, &excType, &exc_value, &excTb);

default_hook:
	/* Call the default unraisable hook (ignore failure) */
	(void)write_unraisableExc(tstate, excType, exc_value, excTb,
		errMsg, _obj);

done:
	ALIF_XDECREF(excType);
	ALIF_XDECREF(exc_value);
	ALIF_XDECREF(excTb);
	ALIF_XDECREF(errMsg);
	_alifErr_clear(tstate); /* Just in case */
}

static void format_unraisable(AlifObject* _obj, const char* _format, ...) { // 1693
	va_list va{};

	va_start(va, _format);
	format_unraisableV(_format, va, _obj);
	va_end(va);
}


void alifErr_writeUnraisable(AlifObject* _obj) { // 1703
	format_unraisable(_obj, nullptr);
}



static AlifObject* err_programText(FILE* _fp, AlifIntT _lineno, const char* _encoding) { // 1905
	char lineBuf[1000]{};
	AlifUSizeT lineSize = 0;

	for (AlifIntT i = 0; i < _lineno; ) {
		lineSize = 0;
		if (alifUniversal_newLineFGetsWithSize(lineBuf, sizeof(lineBuf),
			_fp, nullptr, &lineSize) == nullptr)
		{
			/* Error or EOF. */
			return nullptr;
		}
		if (i + 1 < _lineno
			and lineSize == sizeof(lineBuf) - 1
			and lineBuf[sizeof(lineBuf) - 2] != '\n')
		{
			continue;
		}
		i++;
	}

	const char* line = lineBuf;
	/* Skip BOM. */
	if (_lineno == 1 and lineSize >= 3 and memcmp(line, "\xef\xbb\xbf", 3) == 0) {
		line += 3;
		lineSize -= 3;
	}
	AlifObject* res = alifUStr_decode(line, lineSize, _encoding, "replace");
	if (res == nullptr) {
		alifErr_clear();
	}
	return res;
}

extern char* _alifTokenizer_findEncodingFilename(AlifIntT, AlifObject*); // 1964

AlifObject* _alifErr_programDecodedTextObject(AlifObject* filename, AlifIntT lineno,
	const char* encoding) { // 1966
	char* found_encoding = nullptr;
	if (filename == nullptr or lineno <= 0) {
		return nullptr;
	}

	FILE* fp = alif_fOpenObj(filename, "r");
	if (fp == nullptr) {
		alifErr_clear();
		return nullptr;
	}
	if (encoding == nullptr) {
		AlifIntT fd = fileno(fp);
		found_encoding = _alifTokenizer_findEncodingFilename(fd, filename);
		encoding = found_encoding;
		if (encoding == nullptr) {
			alifErr_clear();
			encoding = "utf-8";
		}
		/* Reset position */
		if (lseek(fd, 0, SEEK_SET) == (off_t)-1) {
			fclose(fp);
			alifMem_dataFree(found_encoding);
			return nullptr;
		}
	}
	AlifObject* res = err_programText(fp, lineno, encoding);
	fclose(fp);
	alifMem_dataFree(found_encoding);
	return res;
}
