#include "alif.h"



AlifIntT alifOS_snPrintF(char* _str, AlifSizeT _size, const  char* _format, ...) { // 41
	AlifIntT rc{};
	va_list va{};

	va_start(va, _format);
	rc = alifOS_vSnprintF(_str, _size, _format, va);
	va_end(va);
	return rc;
}

AlifIntT alifOS_vSnprintF(char* _str, AlifSizeT _size, const char* _format, va_list _va) { // 53
	AlifIntT len{};
	if (_size > INT_MAX - 1) {
		len = -666;
		goto Done;
	}

#if defined(_MSC_VER)
	len = _vsnprintf(_str, _size, _format, _va);
#else
	len = vsnprintf(_str, _size, _format, _va);
#endif

Done:
	if (_size > 0) {
		_str[_size - 1] = '\0';
	}
	return len;
}
