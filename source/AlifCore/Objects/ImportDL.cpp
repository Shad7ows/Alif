#include "alif.h"

#include "AlifCore_Call.h"


#include "AlifCore_ImportDL.h"


#ifdef HAVE_DYNAMIC_LOADING // 18

#ifdef _WINDOWS
extern DLFuncPtr _alifImport_findSharedFuncptrWindows(const char*, const char*, AlifObject*, FILE*);
#else
extern DLFuncPtr _alifImport_findSharedFuncptr(const char*,
	const char*, const char*, FILE*);
#endif

#endif /* HAVE_DYNAMIC_LOADING */
// 31


static const char* const _asciiOnlyPrefix_ = "alifInit"; // 38
static const char* const _nonasciiPrefix_ = "alifInitU"; // 39







void _alifExtModule_loaderInfoClear(AlifExtModuleLoaderInfo* info) { // 103
	ALIF_CLEAR(info->filename);
#ifndef _WINDOWS
	ALIF_CLEAR(info->filenameEncoded);
#endif
	ALIF_CLEAR(info->name);
	ALIF_CLEAR(info->nameEncoded);
}




AlifIntT _alifExtModule_loaderInfoInitForBuiltin(AlifExtModuleLoaderInfo* _info,
	AlifObject* _name) { // 172
	AlifObject* name_encoded = alifUStr_asEncodedString(_name, "utf8", nullptr); //* alif
	if (name_encoded == nullptr) {
		return -1;
	}

	*_info = {
		.name = ALIF_NEWREF(_name),
		.nameEncoded = name_encoded,
		/* We won't need filename. */
		.path = _name,
		.origin = AlifExtModuleOrigin::Alif_Ext_Module_Origin_BUILTIN,
		.hookPrefix = _asciiOnlyPrefix_,
		.newcontext = nullptr,
	};
	return 0;
}



#ifdef HAVE_DYNAMIC_LOADING
AlifIntT _alifExtModule_loaderInfoInitFromSpec(
	class AlifExtModuleLoaderInfo* _pInfo,
	AlifObject* _spec) { // 210
	AlifObject* name = alifObject_getAttrString(_spec, "اسم");
	if (name == nullptr) {
		return -1;
	}
	AlifObject* filename = alifObject_getAttrString(_spec, "اساس");
	if (filename == nullptr) {
		ALIF_DECREF(name);
		return -1;
	}
	/* We could also accommodate builtin modules here without much trouble. */
	AlifExtModuleOrigin origin = AlifExtModuleOrigin::Alif_Ext_Module_Origin_DYNAMIC;
	AlifIntT err = _alifExtModule_loaderInfoInit(_pInfo, name, filename, origin);
	ALIF_DECREF(name);
	ALIF_DECREF(filename);
	return err;
}
#endif /* HAVE_DYNAMIC_LOADING */



void _alifExtModule_loaderResultClear(AlifExtModuleLoaderResult* res) { // 239
	*res = {0};
}




#ifdef HAVE_DYNAMIC_LOADING
AlifModInitFunction _alifImport_getModInitFunc(class AlifExtModuleLoaderInfo* _info,
	FILE* _fp) { // 380
	const char* name_buf = ALIFBYTES_AS_STRING(_info->nameEncoded);
	DLFuncPtr exportfunc{};
#ifdef _WINDOWS
	exportfunc = _alifImport_findSharedFuncptrWindows(
		_info->hookPrefix, name_buf, _info->filename, _fp);
#else
	{
		const char* path_buf = ALIFBYTES_AS_STRING(_info->filenameEncoded);
		exportfunc = _alifImport_findSharedFuncptr(
			_info->hookPrefix, name_buf, path_buf, _fp);
	}
#endif

	if (exportfunc == nullptr) {
		if (!alifErr_occurred()) {
			AlifObject* msg{};
			msg = alifUStr_fromFormat(
				"الوحدة الحيوية لا تعرف  "
				"دالة استخراج الوحدة (%s_%s)",
				_info->hookPrefix, name_buf);
			if (msg != nullptr) {
				//alifErr_setImportError(msg, _info->name, _info->filename);
				ALIF_DECREF(msg);
			}
		}
		return nullptr;
	}

	return (AlifModInitFunction)exportfunc;
}
#endif /* HAVE_DYNAMIC_LOADING */






AlifIntT _alifImport_runModInitFunc(AlifModInitFunction p0,
	AlifExtModuleLoaderInfo* info, AlifExtModuleLoaderResult* p_res) { // 416
	AlifExtModuleLoaderResult res = {
		.kind = AlifExtModuleKind::Alif_Ext_Module_Kind_UNKNOWN,
	};

	/* Call the module init function. */

	const char* oldcontext = _alifImport_swapPackageContext(info->newcontext);
	AlifObject* m = p0();
	_alifImport_swapPackageContext(oldcontext);

	/* Validate the result (and populate "res". */

	if (m == nullptr) {
		res.kind = AlifExtModuleKind::Alif_Ext_Module_Kind_SINGLEPHASE;
		if (alifErr_occurred()) {
			//alifExtModuleLoader_resultSetError(
			//	&res, Alif_Ext_Module_Loader_Result_EXCEPTION);
		}
		else {
			//alifExtModuleLoader_resultSetError(
			//	&res, Alif_Ext_Module_Loader_Result_ERR_MISSING);
		}
		goto error;
	}
	else if (alifErr_occurred()) {
		res.kind = AlifExtModuleKind::Alif_Ext_Module_Kind_SINGLEPHASE;
		//alifExtModuleLoader_resultSetError(
		//	&res, Alif_Ext_Module_Loader_Result_ERR_UNREPORTED_EXC);
		m = nullptr;
		goto error;
	}

	if (ALIF_IS_TYPE(m, nullptr)) {
		//alifExtModuleLoader_resultSetError(
		//	&res, Alif_Ext_Module_Loader_Result_ERR_UNINITIALIZED);
		m = nullptr; /* prevent segfault in DECREF */
		goto error;
	}

	if (ALIFOBJECT_TYPECHECK(m, &_alifModuleDefType_)) {
		res.kind = AlifExtModuleKind::Alif_Ext_Module_Kind_MULTIPHASE;
		res.def = (AlifModuleDef*)m;
	}
	else if (info->hookPrefix == _nonasciiPrefix_) {
		res.kind = AlifExtModuleKind::Alif_Ext_Module_Kind_MULTIPHASE;
		//alifExtModuleLoader_resultSetError(
		//	&res, Alif_Ext_Module_Loader_Result_ERR_NONASCII_NOT_MULTIPHASE);
		goto error;
	}
	else {
		res.kind = AlifExtModuleKind::Alif_Ext_Module_Kind_SINGLEPHASE;
		res.module = m;

		if (!ALIFMODULE_CHECK(m)) {
			//alifExtModuleLoader_resultSetError(
			//	&res, Alif_Ext_Module_Loader_Result_ERR_NOT_MODULE);
			goto error;
		}

		res.def = _alifModule_getDef(m);
		if (res.def == nullptr) {
			alifErr_clear();
			//alifExtModuleLoader_resultSetError(
			//	&res, Alif_Ext_Module_Loader_Result_ERR_MISSING_DEF);
			goto error;
		}
	}

	*p_res = res;
	return 0;

error:
	ALIF_CLEAR(res.module);
	res.def = nullptr;
	*p_res = res;
	p_res->err = &p_res->err_;
	return -1;
}
