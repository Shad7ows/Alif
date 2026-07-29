#include "alif.h"



//extern AlifObject* alifInit__abc(void); // 8
extern AlifObject* alifInit_math(void); // 16
#ifdef _WINDOWS
extern AlifObject* alifInit_nt(void); // 18
#else
extern AlifObject* alifInit_posix(void); // 18
#endif
extern AlifObject* alifInit_time(void);

extern AlifObject* alifInit__random(void); // 41

extern AlifObject* alifInit__io(void);



class InitTable _alifImportInitTab_[] = { // 87
	//{"صنف_اساس_مجرد", alifInit__abc},

	{"الرياضيات", alifInit_math}, // 96

#ifdef _WINDOWS
	{"واجهة_نظام_ويندوز", alifInit_nt}, /* Use the NT os functions, not posix */
#else
	{"واجهة_نظام_يونكس", alifInit_posix},
#endif

	{"الوقت", alifInit_time},



	{"عشوائي", alifInit__random},

	{"_ورد", alifInit__imp}, // _imp *import support system

	/* These entries are here for النظام.اسماء_الوحدات_الضمنية */
	{"الضمنيات", nullptr},
	{"النظام", nullptr},


	{"تبادل", alifInit__io},

	/* Sentinel */
	{0, 0}
};
