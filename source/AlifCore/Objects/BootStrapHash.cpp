#include "alif.h"
#include "AlifCore_DureRun.h"



#ifdef HAVE_UNISTD_H
#  include <unistd.h>             // close()
#endif
#ifdef MS_WINDOWS
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
#  if defined(HAVE_SYS_RANDOM_H) and (defined(HAVE_GETRANDOM) or defined(HAVE_GETENTROPY))
#    include <sys/random.h>       // getrandom()
#  endif
#  if !defined(HAVE_GETRANDOM) and defined(HAVE_GETRANDOM_SYSCALL)
#    include <sys/syscall.h>      // SYS_getrandom
#  endif
#endif

#if defined(__APPLE__) and defined(__has_builtin)
#  if __has_builtin(__builtin_available)
#    define HAVE_GETENTRYPY_GETRANDOM_RUNTIME __builtin_available(macOS 10.12, iOS 10.10, tvOS 10.0, watchOS 3.0, *)
#  endif
#endif
#ifndef HAVE_GETENTRYPY_GETRANDOM_RUNTIME
#  define HAVE_GETENTRYPY_GETRANDOM_RUNTIME 1
#endif

static AlifIntT win32_uRandom(unsigned char* _buffer, AlifSizeT _size, AlifIntT _raise) { // 54
	while (_size > 0)
	{
		DWORD chunk = (DWORD)ALIF_MIN(_size, ALIF_DWORD_MAX);
		NTSTATUS status = BCryptGenRandom(nullptr, _buffer, chunk, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		if (!BCRYPT_SUCCESS(status)) {
			/* BCryptGenRandom() failed */
			if (_raise) {
				alifErr_setFromWindowsErr(0);
			}
			return -1;
		}
		_buffer += chunk;
		_size -= chunk;
	}
	return 0;
}


#if defined(HAVE_GETRANDOM) or defined(HAVE_GETRANDOM_SYSCALL)
#define ALIF_GETRANDOM 1

static AlifIntT alif_getRandom(void* _buffer, AlifSizeT _size, AlifIntT _blocking, AlifIntT _raise) { // 91

	static AlifIntT getRandomWorks = 1;
	AlifIntT flags{};
	char* dest{};
	long n{};

	if (!getRandomWorks) {
		return 0;
	}

	flags = _blocking ? 0 : GRND_NONBLOCK;
	dest = (char*)_buffer;
	while (0 < _size) {
#if defined(__sun) and defined(__SVR4)
		/* Issue #26735: On Solaris, getrandom() is limited to returning up
		   to 1024 bytes. Call it multiple times if more bytes are
		   requested. */
		n = ALIF_MIN(_size, 1024);
#else
		n = ALIF_MIN(_size, LONG_MAX);
#endif

		errno = 0;
#ifdef HAVE_GETRANDOM
		if (_raise) {
			ALIF_BEGIN_ALLOW_THREADS
				n = getrandom(dest, n, flags);
			ALIF_END_ALLOW_THREADS
		}
		else {
			n = getrandom(dest, n, flags);
		}
#else

		if (_raise) {
			ALIF_BEGIN_ALLOW_THREADS
				n = syscall(SYS_getrandom, dest, n, flags);
			ALIF_END_ALLOW_THREADS
		}
		else {
			n = syscall(SYS_getrandom, dest, n, flags);
		}
#  ifdef _Py_MEMORY_SANITIZER
		if (n > 0) {
			__msan_unpoison(dest, n);
		}
#  endif
#endif

		if (n < 0) {
			/* ENOSYS: the syscall is not supported by the kernel.
			   EPERM: the syscall is blocked by a security policy (ex: SECCOMP)
			   or something else. */
			if (errno == ENOSYS or errno == EPERM) {
				getRandomWorks = 0;
				return 0;
			}

			/* getrandom(GRND_NONBLOCK) fails with EAGAIN if the system urandom
			   is not initialized yet. For _PyRandom_Init(), we ignore the
			   error and fall back on reading /dev/urandom which never blocks,
			   even if the system urandom is not initialized yet:
			   see the PEP 524. */
			if (errno == EAGAIN and !_raise and !_blocking) {
				return 0;
			}

			if (errno == EINTR) {
				if (_raise) {
					if (alifErr_checkSignals()) {
						return -1;
					}
				}

				/* retry getrandom() if it was interrupted by a signal */
				continue;
			}

			if (_raise) {
				alifErr_setFromErrno(_alifExcOSError_);
			}
			return -1;
		}

		dest += n;
		_size -= n;
	}
	return 1;
}

#elif defined(HAVE_GETENTROPY)
#define ALIF_GETENTROPY 1

#if defined(__APPLE__) and ALIF_HAS_ATTRIBUTE(availability)
static AlifIntT alif_getRandom(char* _buffer , AlifSizeT _size, AlifIntT _raise)
__attribute__((availability(macos, introduced = 10.12)))
__attribute__((availability(ios, introduced = 10.0)))
__attribute__((availability(tvos, introduced = 10.0)))
__attribute__((availability(watchos, introduced = 3.0)));
#endif

#endif /* defined(HAVE_GETENTROPY) && !(defined(__sun) && defined(__SVR4)) */

#define URANDOM_CACHE (_alifDureRun_.alifHashState.uRandomCache) // 271

static AlifIntT dev_uRandom(char* _buffer, AlifSizeT _size, AlifIntT _raise) { // 300
	AlifIntT fd{};
	AlifSizeT n{};

	if (_raise) {
		AlifStatClass st{};
		AlifIntT fStatResult{};

		if (URANDOM_CACHE.fd >= 0) {
			ALIF_BEGIN_ALLOW_THREADS
				fStatResult = _alifFStat_noRaise(URANDOM_CACHE.fd, &st);
			ALIF_END_ALLOW_THREADS

				if (fStatResult
					or st.dev != URANDOM_CACHE.dev
					or st.ino != URANDOM_CACHE.ino) {
					URANDOM_CACHE.fd = -1;
				}
		}
		if (URANDOM_CACHE.fd >= 0)
			fd = URANDOM_CACHE.fd;
		else {
			fd = _alif_open("/dev/urandom", O_RDONLY);
			if (fd < 0) {
				if (errno == ENOENT or errno == ENXIO or
					errno == ENODEV or errno == EACCES) {
					alifErr_setString(_alifExcNotImplementedError_,
						"/dev/urandom (or equivalent) not found");
				}
				return -1;
			}
			if (URANDOM_CACHE.fd >= 0) {

				close(fd);
				fd = URANDOM_CACHE.fd;
			}
			else {
				if (_alif_fStat(fd, &st)) {
					close(fd);
					return -1;
				}
				else {
					URANDOM_CACHE.fd = fd;
					URANDOM_CACHE.dev = st.dev;
					URANDOM_CACHE.ino = st.ino;
				}
			}
		}

		do {
			n = _alif_read(fd, _buffer, (AlifSizeT)_size);
			if (n == -1)
				return -1;
			if (n == 0) {
				alifErr_format(_alifExcRuntimeError_,
					"Failed to read %zi bytes from /dev/urandom",
					_size);
				return -1;
			}

			_buffer += n;
			_size -= n;
		} while (0 < _size);
	}
	else {
		fd = _alifOpen_noRaise("/dev/urandom", O_RDONLY);
		if (fd < 0) {
			return -1;
		}

		while (0 < _size)
		{
			do {
				n = read(fd, _buffer, (AlifSizeT)_size);
			} while (n < 0 and errno == EINTR);

			if (n <= 0) {
				/* stop on error or if read(size) returned 0 */
				close(fd);
				return -1;
			}

			_buffer += n;
			_size -= n;
		}
		close(fd);
	}
	return 0;
}



static AlifIntT alifURandom(void* _buffer, AlifSizeT _size, AlifIntT _blocking, AlifIntT _raise) { // 477
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

#ifdef MS_WINDOWS
	return win32_uRandom((unsigned char*)buffer, size, raise);
#else

#if defined(ALIF_GETRANDOM) or defined(ALIF_GETENTROPY)
	if (HAVE_GETENTRYPY_GETRANDOM_RUNTIME) {
#ifdef ALIF_GETRANDOM
		res = alif_getRandom(buffer, size, blocking, raise);
#else
		res = alif_getEntropy(buffer, size, raise);
#endif
		if (res < 0) {
			return -1;
		}
		if (res == 1) {
			return 0;
		}
		/* getrandom() or getentropy() function is not available: failed with
		   ENOSYS or EPERM. Fall back on reading from /dev/urandom. */
	} /* end of availability block */
#endif

	return dev_uRandom((char*)_buffer, _size, _raise);
#endif
}

AlifIntT _alifOS_uRandomNonblock(void* _buffer, AlifSizeT _size) { // 545
	return alifURandom(_buffer, _size, 0, 1);
}
