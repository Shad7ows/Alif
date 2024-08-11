#pragma once


/* --------------------------------- AlifWStringList --------------------------------- */
class AlifWStringList {
public:
	AlifSizeT length;
	wchar_t** items;
};


/* ----------------------------------- AlifConfig ------------------------------------ */
class AlifConfig {
public:
	AlifIntT configInit{};

	AlifIntT tracemalloc{};
	AlifIntT parseArgv{};

	AlifWStringList origArgv{};
	AlifWStringList argv;

	AlifIntT interactive{};
	AlifIntT optimizationLevel{};

	AlifIntT quite{};

	wchar_t* programName{};

	AlifIntT skipFirstLine{};
	wchar_t* runCommand{};
	wchar_t* runModule{};
	wchar_t* runFilename{};
	int installImportlib = 1;

	AlifIntT initMain{};
};


AlifIntT alif_setLocaleAndWChar();
void alifConfig_initAlifConfig(AlifConfig*);
