#pragma once


/* ___________ AlifStatus ___________ */

/* تستخدم هذه الحالات للتحقق تقريباً من معظم أخطاء تهيئة لغة ألف */

#  define ALIFSTATUS_GET_FUNC() __func__

// تستخدم هذه الحالة للتحقق في كل دالة ما إذا تمت بشكل صحيح أم لا
#define ALIFSTATUS_OK() {.type = 0,} // 0 -> Ok , 1 -> Error ,  2 -> Exit

#define ALIFSTATUS_ERR(_errMsg) {					\
			.type = 1,								\
			.func = ALIFSTATUS_GET_FUNC(),			\
			.mesError = (_errMsg),					\
		}

#define ALIFSTATUS_NO_MEMORY() ALIFSTATUS_ERR("فشل الحجز من الذاكرة")

#define ALIFSTATUS_EXIT(_exitCode) {				\
			.type = 2,								\
			.exitCode = (_exitCode),				\
		}

#define ALIFSTATUS_IS_ERROR(_error) ((_error).type == 1)
#define ALIFSTATUS_IS_EXIT(_error) ((_error).type == 2)
#define ALIFSTATUS_EXCEPTION(_error) ((_error).type != 0)
#define ALIFSTATUS_UPDATE_FUNC(_error) do { (_error).func = ALIFSTATUS_GET_FUNC(); } while (0)


/* ___________ AlifArgv ___________ */

class AlifArgv { // صنف يحتوي عدد المعاملات الممررة من الطرفية وقيمها
public:
	AlifSizeT argc;
	int useCharArgv;
	char* const* charArgv;
	wchar_t* const* wcharArgv;
};

AlifStatus alifArgv_asWstrList(const AlifArgv*, AlifWideStringList*);

/* ___________ Helper functions ___________ */

extern const wchar_t* alifGet_xOption(const AlifWideStringList*,const wchar_t*);
extern const char* alif_getEnv(int, const char*);

/* ___________ AlifPreCmdline ___________ */

class AlifPreCmdLine
{
public:
	AlifWideStringList argv;
	AlifWideStringList xoptions;     /* "-X value" option */
	int isolated;					 /* -I option */
	int useEnvironment;			 /* -E option */
	int warnDefaultEncoding;       /* -X warn_default_encoding and ALIFWARNDEFAULTENCODING */
};

#define ALIFPRECMDLINE_INIT {  .isolated = -1, .useEnvironment = -1, }

extern AlifStatus alifPreCmdLine_setArgv(AlifPreCmdLine*, const AlifArgv*);
extern AlifStatus alifPreCmdLine_read(AlifPreCmdLine*, const AlifPreConfig*);


























































/* ___________ AlifPreConfig ___________ */


void alifPreConfig_initCompatConfig(AlifPreConfig*);
extern void alifPreConfig_initFromConfig(AlifPreConfig*, const AlifConfig*);


extern AlifStatus alifPreConfig_initFromPreConfig(AlifPreConfig*, const AlifPreConfig*);

//extern AlifObject* alifPreConfig_asDict(const AlifPreConfig*);
//extern void alifPreConfig_getConfig(AlifPreConfig*, const AlifConfig*);
extern AlifStatus alifPreConfig_read(AlifPreConfig*, const AlifArgv*);
extern AlifStatus alifPreConfig_write(const AlifPreConfig*);








/* ___________ AlifConfig ___________ */

enum _PyConfigInitEnum {
	AlifConfig_Init_Compat = 1,
	AlifConfig_Init_Alif = 2,
	AlifConfig_Init_Isolated = 3
};


void _alifConfig_initCompatConfig(AlifConfig*);
extern AlifStatus alifConfig_read(AlifConfig*);
extern AlifStatus alifConfig_setAlifArgv(AlifConfig*, const AlifArgv*);

