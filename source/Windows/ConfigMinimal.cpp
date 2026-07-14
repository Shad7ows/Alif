#include "alif.h"



extern AlifObject* alifInit_nt(void);

extern AlifObject* alifInit__io(void);



class InitTable _alifImportInitTab_[] = { // 87

	{"واجهة_نظام_ويندوز", alifInit_nt}, /* Use the NT os functions, not posix */


	{"_ورد", alifInit__imp},

	/* These entries are here for النظام.اسماء_الوحدات_الضمنية */
	{"الضمنيات", nullptr},
	{"النظام", nullptr},


	{"تبادل", alifInit__io},

	/* Sentinel */
	{0, 0}
};
