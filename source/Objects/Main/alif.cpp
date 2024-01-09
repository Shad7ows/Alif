﻿#include "alif.h"
#include "AlifCore_Memory.h"
#include "AlifCore_InitConfig.h"
#include "AlifCore_AlifCycle.h"


#define ALIF_COPYRIGHT L"للمزيد من المعلومات اكتب \"help\", \"copyright\", \"license\" "


/* ----------------------------------- تهيئة اللغة ----------------------------------- */
AlifConfig tempConfig{};
static void alifMain_init(AlifArgv* _args) {

	AlifConfig config{};

	alif_localeInit();

	alif_memoryInit();

	alifArgv_asWStrList(&config, _args);

	alif_initFromConfig(&config);
	tempConfig = config;
}


static void alifMain_runAlif(int* exitcode)
{
	AlifConfig config = tempConfig;

	if (config.runCommand) {
		//*exitcode = alifMain_runCommand(config->runCommand);
	}
	else if (config.runModule) {
		//*exitcode = alifMain_runModule(config->runModule, 1);
	}
	else if (config.runFilename != NULL) {
		//*exitcode = alifMain_runFile(config);
	}
	else {
		//*exitcode = alifMain_runStdin(config);
	}
}

int alif_runMain()
{
	int exitcode = 0;

	alifMain_runAlif(&exitcode);

	return exitcode;
}


static int alifMain_main(AlifArgv* args) {
	alifMain_init(args);

	return alif_runMain();
}


int alif_mainWchar(int _argc, wchar_t** _argv) {
	AlifArgv args = { _argc, 0, nullptr, _argv };
	return alifMain_main(&args);
}



int alif_mainBytes(int _argc, char** _argv) {
	AlifArgv args = { _argc, 1, _argv, nullptr };
	return alifMain_main(&args);
}

#ifdef _WINDOWS
int wmain(int _argc, wchar_t** _argv)
{
	wchar_t* argsv[] = { (wchar_t*)L"alif", (wchar_t*)L"example.alif" };
	alif_mainWchar(2, argsv);
	std::wcout << tempConfig.argv.items[0] << std::endl;
	std::wcout << tempConfig.origArgv.items[1] << std::endl;
	std::wcout << tempConfig.programName << std::endl;
	std::wcout << tempConfig.runFilename << std::endl;
	return 0;
}
#else
int main(int _argc, char** _argv)
{
	char* argsv[] = { (char*)"alif", (char*)"example.alif" };
	return alif_mainBytes(2, argsv);
}
#endif
