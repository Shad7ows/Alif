#include "alif.h"
#include "AlifCore_Call.h"
#include "AlifCore_AlifEval.h"
#include "AlifCore_Dict.h"
#include "AlifCore_Frame.h"
#include "AlifCore_InitConfig.h"
#include "AlifCore_Integer.h"
#include "AlifCore_ModSupport.h"

int alifSys_create(AlifThread* _tstate, AlifObject** _sysModP)
{

	AlifInterpreter* interp = _tstate->interpreter;

	AlifObject* modules = alifImport_initModules(interp);
	if (modules == nullptr) {
		goto error;
	}

	//AlifObject* sysmod = _PyModule_CreateInitialized(&sysmodule, PYTHON_API_VERSION);
	//if (sysmod == nullptr) {
	//	return _PyStatus_ERR("failed to create a module object");
	//}

	//AlifObject* sysdict = PyModule_GetDict(sysmod);
	//if (sysdict == nullptr) {
	//	goto error;
	//}
	//interp->sysdict = Py_NewRef(sysdict);

	//interp->sysdict_copy = PyDict_Copy(sysdict);
	//if (interp->sysdict_copy == nullptr) {
	//	goto error;
	//}

	//if (PyDict_SetItemString(sysdict, "modules", modules) < 0) {
	//	goto error;
	//}

	//PyStatus status = _PySys_SetPreliminaryStderr(sysdict);
	//if (_PyStatus_EXCEPTION(status)) {
	//	return status;
	//}

	//status = _PySys_InitCore(tstate, sysdict);
	//if (_PyStatus_EXCEPTION(status)) {
	//	return status;
	//}

	//if (_PyImport_FixupBuiltin(sysmod, "sys", modules) < 0) {
	//	goto error;
	//}

	//AlifObject* monitoring = _Py_CreateMonitoringObject();
	//if (monitoring == nullptr) {
	//	goto error;
	//}
	//int err = PyDict_SetItemString(sysdict, "monitoring", monitoring);
	//ALIF_DECREF(monitoring);
	//if (err < 0) {
	//	goto error;
	//}


	//*_sysModP = sysmod;
	return 1;

error:
	return 0;
	//return alifStatus_ERR("can't initialize sys module");
}
