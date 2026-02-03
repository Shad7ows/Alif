#include "alif.h"



#ifdef HAVE_UNISTD_H
#  include <unistd.h>             // close()
#endif
#ifdef _WINDOWS
#  include <windows.h>
#  include <bcrypt.h>
#else
#  include <fcntl.h>              // O_RDONLY
#  ifdef HAVE_SYS_STAT_H
#    include <sys/stat.h>
#  endif
#  ifdef HAVE_LINUX_RANDOM_H
#    include <linux/random.h>     // GRND_NONBLOCK
#  endif
#  if defined(HAVE_SYS_RANDOM_H) && (defined(HAVE_GETRANDOM) || defined(HAVE_GETENTROPY))
#    include <sys/random.h>       // getrandom()
#  endif
#  if !defined(HAVE_GETRANDOM) && defined(HAVE_GETRANDOM_SYSCALL)
#    include <sys/syscall.h>      // SYS_getrandom
#  endif
#endif



// 49
#ifdef _WINDOWS

static AlifIntT win32_urandom(unsigned char *_buffer, AlifSizeT _size,
	AlifIntT _raise) { // 53
	while (_size > 0)
	{
		DWORD chunk = (DWORD)ALIF_MIN(_size, ALIF_DWORD_MAX);
		NTSTATUS status = BCryptGenRandom(nullptr, _buffer, chunk, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		if (!BCRYPT_SUCCESS(status)) {
			/* BCryptGenRandom() failed */
			if (_raise) {
				//alifErr_setFromWindowsErr(0);
			}
			return -1;
		}
		_buffer += chunk;
		_size -= chunk;
	}
	return 0;
}

#else /* !_WINDOWS */


#endif /* !_WINDOWS */ // 409



static AlifIntT alif_urandom(void *_buffer, AlifSizeT _size,
	AlifIntT _blocking, AlifIntT _raise) { // 476
#if defined(ALIF_GETRANDOM) or defined(ALIF_GETENTROPY)
	AlifIntT res{};
#endif

	if (_size < 0) {
		if (_raise) {
			alifErr_format(_alifExcValueError_,
				"negative argument not allowed");
		}
		return -1;
	}

	if (_size == 0) {
		return 0;
	}

#ifdef _WINDOWS
	return win32_urandom((unsigned char *)_buffer, _size, _raise);
#else

#if defined(ALIF_GETRANDOM) or defined(ALIF_GETENTROPY)
	if (HAVE_GETENTRYPY_GETRANDOM_RUNTIME) {
	#ifdef ALIF_GETRANDOM
		res = alif_getRandom(_buffer, _size, _blocking, _raise);
	#else
		res = alif_getEntropy(_buffer, _size, _raise);
	#endif
		if (res < 0) {
			return -1;
		}
		if (res == 1) {
			return 0;
		}
	}
#endif

	return dev_urandom(_buffer, _size, _raise);
#endif
}




AlifIntT _alifOS_uRandomNonBlock(void *_buffer, AlifSizeT _size) { // 544
	return alif_urandom(_buffer, _size, 0, 1);
}
