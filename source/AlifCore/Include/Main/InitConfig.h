#pragma once


/* --------------------------------- AlifWStringList --------------------------------- */
class AlifWStringList { // 31
public:
	AlifSizeT length{};
	wchar_t** items{};
};

// لماذا يوجد append و insert وتقومان بنفس الوظيفة ??
AlifIntT alifWStringList_append(AlifWStringList*, const wchar_t*); // 38
AlifIntT alifWStringList_insert(AlifWStringList*, AlifSizeT, const wchar_t*);

/* ----------------------------------- AlifConfig ------------------------------------ */
class AlifConfig { // 134
public:
	AlifIntT configInit{};

	AlifIntT tracemalloc{};
	AlifIntT parseArgv{};

	AlifWStringList origArgv{};
	AlifWStringList argv{};

	AlifIntT interactive{};
	AlifIntT optimizationLevel{};
	//AlifIntT configStdio{}; // نظام ألف يستخدم ترميز utf-8 بشكل إفتراضي
	AlifIntT bufferedStdio{};

	AlifIntT quite{};

	wchar_t* programName{};

	AlifIntT skipFirstLine{};
	wchar_t* runCommand{};
	wchar_t* runModule{};
	wchar_t* runFilename{};

	AlifIntT initMain{};
};


AlifIntT alif_setStdioLocale(const AlifConfig*); // alif
void alifConfig_initAlifConfig(AlifConfig*); // 239

void alifConfig_clear(AlifConfig*); // 241
AlifIntT alifConfig_setString(AlifConfig*, wchar_t**, const wchar_t*);
