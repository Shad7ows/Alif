﻿#include "alif.h"
#include "AlifCore_GetConsoleLine.h"
#include "AlifCore_Memory.h"
#include "AlifCore_InitConfig.h"

#ifdef _WINDOWS
#include <windows.h>
#include <fcntl.h>
#else
#include <unistd.h>
typedef uint32_t DWORD;
#define MAX_PATH 260
#endif

#define MAXPATHLEN 256

#pragma warning(disable : 4996) // for disable unsafe functions error

#define ALIF_ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

static const wchar_t usageLine[] =
L"usage: %ls [option] ... [-c cmd | -m mod | file | - ] [arg] ...\n";

static const wchar_t usageHelp[] = L"\
-c cmd : program passed in as string (terminates option list)\n\
-m mod : run library module as a script (terminates option list)\n\
file   : program read from script file\n\
-      : program read from stdin (default; interactive mode if a tty)\n\
arg ...: arguments passed to program in sys.argv[1:]\n\
-h     : print this help message and exit (--help)\n\
-v     : print the Python version number and exit (also --version)\n\
";

static void config_usage(const wchar_t* program)
{
	FILE* f = stdout;

	fwprintf(f, usageLine, program);

	fputws(usageHelp, f);
}


void alif_localeInit() {
#ifdef _WINDOWS
	bool modeIn = _setmode(_fileno(stdin), _O_WTEXT);
	bool modeOut = _setmode(_fileno(stdout), _O_WTEXT);
	if (!modeIn or !modeOut) {
		std::wcout << L"لم يستطع تهيئة الطرفية لقراءة الأحرف العربية" << std::endl;
		exit(-1);
	}
#endif // _WINDOWS

	const char* locale = setlocale(LC_CTYPE, nullptr);
	if (locale == nullptr) {
		std::wcout << L"لم يستطع تهيئة الطرفية لقراءة الأحرف العربية" << std::endl;
		exit(-1);
	}

	setlocale(LC_CTYPE, locale);
}

void alifArgv_asWStrList(AlifConfig* _config, AlifArgv* _args) {
	if (_args->useBytesArgv)
	{
		AlifWStringList wArgv = { 0, nullptr };
		//wArgv.items = new wchar_t* [_args->argc] {};
		wArgv.items = (wchar_t**)alifMem_dataAlloc(_args->argc * sizeof(wchar_t*));

		for (int i = 0; i < _args->argc; i++) {
			size_t len = mbstowcs(nullptr, (const char*)_args->bytesArgv[i], 0);
			//wchar_t* arg = new wchar_t[len];
			wchar_t* arg = (wchar_t*)alifMem_dataAlloc(len * sizeof(wchar_t));
			mbstowcs(arg, (const char*)_args->bytesArgv[i], len);
			wArgv.items[i] = arg;
			wArgv.length++;
		}
		_config->argv = wArgv;
		_config->origArgv = wArgv;

	}
	else {
		_config->argv.length = _args->argc;
		_config->argv.items = (wchar_t**)_args->wcharArgv;

		_config->origArgv.length = _args->argc;
		_config->origArgv.items = (wchar_t**)_args->wcharArgv;
	}
}


static void parse_consoleLine(AlifConfig* _config, size_t* _index) {
	int printVer{};
	const AlifWStringList* argv = &_config->argv;
	const wchar_t* program = _config->programName;
	if (!program and argv->length >= 1) {
		program = argv->items[0];
	}

	alif_resetConsoleLine();
	do {
		int c = alif_getConsoleLine(argv->length, argv->items);

		if (c == EOF) {
			break;
		}

		if (c == 'c') {
			if (_config->runCommand == nullptr) {
				size_t len = wcslen(optArg) + 2;
				wchar_t* command = (wchar_t*)alifMem_dataAlloc(len * sizeof(wchar_t));
				if (command == nullptr) {
					exit(-1);
				}
				memcpy(command, optArg, (len - 2) * sizeof(wchar_t));
				command[len - 2] = '\n';
				command[len - 1] = 0;
				_config->runCommand = command;
			}
			break;
		}

		if (c == 'm') {
			if (_config->runModule == nullptr) {
				size_t len = wcslen(optArg) * sizeof(wchar_t) + 2;
				_config->runModule = (wchar_t*)alifMem_dataAlloc(len);
				if (_config->runModule) {
					memcpy(_config->runModule, optArg, len);
				}
			}
			break;
		}

		if (c == 0) {
			//config_complete_usage(program);
			exit(1);
		}
		else if (c == 1) {
			//config_envvars_usage();
			exit(1);
		}
		else if (c == 'h') {
			config_usage(program);
			exit(1);
		}
		else if (c == 'v') {
			printVer++;
			break;
		}
		else {
			//config_usage(1, program);
			exit(1);
		}

	} while (true);



	if (printVer) {
		wprintf(L"alif %ls\n", alif_getVersion());
	}

	if (_config->runModule == nullptr and _config->runCommand == nullptr
		and optInd < argv->length and wcscmp(argv->items[optInd], L"-") != 0
		and _config->runFilename == nullptr) {
		//_config->runFilename = new wchar_t[wcslen(argv->items[optInd]) + 1] {0};
		_config->runFilename = (wchar_t*)alifMem_dataAlloc(wcslen(argv->items[optInd]) * sizeof(wchar_t) + 2);
		memcpy(_config->runFilename, argv->items[optInd], wcslen(argv->items[optInd]) * sizeof(wchar_t));
	}

	if (_config->runCommand != nullptr or
		_config->runModule != nullptr) {
		optInd--;
	}

	_config->programName = (wchar_t*)program;

	*_index = optInd;
}

static void update_argv(AlifConfig* _config, AlifSizeT _index) {
	const AlifWStringList* cmdlineArgv = &_config->argv;
	AlifWStringList configArgv = { 0, nullptr };
	if (cmdlineArgv->length <= _index) {
		wchar_t* append = (wchar_t*)alifMem_dataAlloc(sizeof(L""));
		*append = L'\0';
		configArgv.items = &append;
		configArgv.length++;
	}
	else {
		AlifWStringList slice{};
		slice.length = cmdlineArgv->length - _index;
		slice.items = &cmdlineArgv->items[_index];
		configArgv = slice;
	}

	wchar_t* arg1{};
	if (_config->runCommand != nullptr) {
		arg1 = (wchar_t*)L"-c";
	}
	else if (_config->runModule != nullptr) {
		arg1 = (wchar_t*)L"-m";
	}

	if (arg1 != nullptr) {
		configArgv.items[0] = arg1;
	}

	_config->argv = configArgv;
}

static void run_absPathFilename(AlifConfig* _config) {

	if (!_config->runFilename) {
		return;
	}

	wchar_t* absFilename{};

#ifdef _WINDOWS
	wchar_t wOutBuf[MAX_PATH]{}, * wOutBufP = wOutBuf;
	DWORD result{};
	result = GetFullPathNameW(_config->runFilename, ALIF_ARRAY_LENGTH(wOutBuf), wOutBuf, nullptr);

	//absFilename = new wchar_t[result + 1] {};
	absFilename = (wchar_t*)alifMem_dataAlloc(result * sizeof(wchar_t) + 2);
	memcpy(absFilename, wOutBuf, result * sizeof(wchar_t));
#else
	char buf[MAXPATHLEN + 1]{};

	char* cwd = getcwd(buf, sizeof(buf));
	size_t len = mbstowcs(nullptr, cwd, 0);
	//wchar_t* wCwd = new wchar_t[len] {};
	wchar_t* wCwd = (wchar_t*)alifMem_dataAlloc(len * sizeof(wchar_t));
	mbstowcs(wCwd, cwd, len);

	size_t cwdLen = wcslen(wCwd);
	size_t pathLen = wcslen(_config->runFilename);
	size_t length = cwdLen + 1 + pathLen + 1;

	//absFilename = new wchar_t[length] {};
	absFilename = (wchar_t*)alifMem_dataAlloc(length * sizeof(wchar_t));

	wchar_t* absPath = absFilename;
	memcpy(absPath, wCwd, cwdLen * sizeof(wchar_t));
	absPath += cwdLen;

	*absPath = (wchar_t)L'/';
	absPath++;

	memcpy(absPath, _config->runFilename, pathLen * sizeof(wchar_t));
	absPath += pathLen;

	*absPath = 0;
#endif // _WINDOWS

	_config->runFilename = absFilename;
}

static void config_readConsole(AlifConfig* _config) {
	size_t index{};
	parse_consoleLine(_config, &index);

	run_absPathFilename(_config);

	update_argv(_config, index);
}


void alifConfig_read(AlifConfig* _config) {
	config_readConsole(_config);
}
