#include "alif.h"



extern AlifObject* alifInit__io(void);



class InitTable _alifImportInitTab_[] = { // 87
	{"_ورد", alifInit__imp},

	/* These entries are here for sys.builtin_module_names */
	{"الضمنيات", nullptr},
	{"النظام", nullptr},


	{"تبادل", alifInit__io},

	/* Sentinel */
	{0, 0}
};
