﻿#include "alif.h"

#include "AlifCore_AST.h"
//#include "AlifCore_AlifRun.h"

#ifdef _WINDOWS
#include "windows.h"
#endif // _WINDOWS


// Forward Declaration
//static AlifObject* alifRun_file(FILE*, AlifObject*, int, int);
Module* alifParser_astFromFile(FILE*, AlifObject*, int, AlifASTMem*); // temp

static AlifObject* alifRun_file(FILE* _fp, AlifObject* _fn, int _start, int _fClose) { 

	AlifASTMem* astMem = alifASTMem_new();

	Module* module_{};
	module_ = alifParser_astFromFile(_fp, _fn, _start, astMem);

	if (_fClose) {
		fclose(_fp);
	}

	AlifObject* res{};
	if (module_) {
		//res_ = alifRun_module(module_, _fn, alifMem);
	}
	else {
		res = nullptr;
	}


	return res;
}

int alifRun_simpleFileObj(FILE* _fp, AlifObject* _fn, int _fClose) { 
	int exitcode = -1;

	AlifObject* mod{};

	mod = alifRun_file(_fp, _fn, ALIFFILE_INPUT, _fClose);
	if (mod == nullptr) {
		goto done;
	}

	ALIF_DECREF(mod);
	exitcode = 0;

done:
	return exitcode;
}

int alifRun_fileObj(FILE* _fp, AlifObject* _fn, int _fClose) { 

	int res{};
	if (_isatty(_fileno(_fp))) { // هذا يعني انه تفاعلي
		//res_ = alifRun_interactiveLoop(_fp, _fn);
		if (_fClose) {
			fclose(_fp);
		}
	}
	else {
		res = alifRun_simpleFileObj(_fp, _fn, _fClose);
	}

	return res;
}

