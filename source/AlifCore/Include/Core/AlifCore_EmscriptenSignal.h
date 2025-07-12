#pragma once

#if defined(__EMSCRIPTEN__)

void _alif_checkEmscriptenSignals(void);

void _alif_checkEmscriptenSignalsPeriodically(void);

#define _ALIF_CHECK_EMSCRIPTEN_SIGNALS() _alif_checkEmscriptenSignals()

#define _ALIF_CHECK_EMSCRIPTEN_SIGNALS_PERIODICALLY() _alif_checkEmscriptenSignalsPeriodically()

extern int _alifEmscriptenSignalhandling_;
extern int _alifEmscriptenSignalClock_;

#else

#define _ALIF_CHECK_EMSCRIPTEN_SIGNALS()
#define _ALIF_CHECK_EMSCRIPTEN_SIGNALS_PERIODICALLY()

#endif // defined(__EMSCRIPTEN__)

