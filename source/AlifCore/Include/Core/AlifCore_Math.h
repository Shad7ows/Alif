#pragma once


static inline void _alif_adjustErange1(double _x) { // 33
	if (errno == 0) {
		if (_x == ALIF_INFINITY or _x == -ALIF_INFINITY) {
			errno = ERANGE;
		}
	}
	else if (errno == ERANGE and _x == 0.0) {
		errno = 0;
	}
}












// 197
#ifndef _ALIF_SHORT_FLOAT_REPR
	#define _ALIF_SHORT_FLOAT_REPR 1
#endif
