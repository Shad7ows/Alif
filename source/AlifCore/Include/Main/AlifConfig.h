#pragma once

/*
	هذا الملف يعمل على تحديد بعض المعلوامات الخاصة بنظام التشغيل
*/


#ifdef _WIN32
	#define _WINDOWS
	#if defined(_WIN64)
		#define _WINDOWS64
		#define _OS64
	#else
		#define _WINDOWS32
		#define _OS32
	#endif
#elif defined(__linux__)
	#ifdef __x86_64__
		#define _LINUX64
		#define _OS64
	#else
		#define _LINUX32
		#define _OS32
	#endif
#elif defined(__APPLE__)
	#define _MAC
	#if defined(__i386__) or defined (_M_IX86)
		#define _MAC32
		#define _OS32
	#elif defined(_M_X64) or defined(__amd64__)
		#define _MAC64
		#define _OS64
	#elif defined(__aarch64) or defined(_M_ARM64)
		#define _MAC64_ARM
		#define _OS64
	#endif 
#elif defined(__ARM_ARCH) // يحتاج مراجعة
	#if __ARM_ARCH >= 8
		#define _ARM64
		#define _OS64
	#else
		#define _ARM32
		#define _OS32
	#endif
#else
	#error L"منصة تشغيل غير معروفة"
#endif




/* ------------------------------------- Windows ------------------------------------- */
#ifdef _WINDOWS 

#define WITH_THREAD
#define SIZEOF_WCHART 2
#define NT_THREADS

/* ------------------------------ مترجم مايكروسوفت ------------------------------ */
#ifdef _MSC_VER

	#define ALIF_COMPILER_VERSION(SUFFIX) \
			("[MSC v." ALIF_STRINGIZE(_MSC_VER) " " SUFFIX "]")


	#define ALIF_STRINGIZE(X) ALIF_STRINGIZE1(X)
	#define ALIF_STRINGIZE1(X) #X

	#ifdef _WINDOWS64
		#if defined(_M_X64) or defined(_M_AMD64)
			#if defined(__clang__)
				#define COMPILER ("[Clang " __clang_version__ "] 64 bit (AMD64) with MSC v." ALIF_STRINGIZE(_MSC_VER) " CRT]")
			#elif defined(__INTEL_COMPILER)
				#define COMPILER ("[ICC v." ALIF_STRINGIZE(__INTEL_COMPILER) " 64 bit (amd64) with MSC v." ALIF_STRINGIZE(_MSC_VER) " CRT]")
			#else
				#define COMPILER ALIF_COMPILER_VERSION("64 bit (AMD64)")
			#endif
				#define ALIFD_PLATFORM_TAG "win_amd64"
		#elif defined(_M_ARM64)
			#define COMPILER ALIF_COMPILER_VERSION("64 bit (ARM64)")
			#define ALIFD_PLATFORM_TAG "win_arm64"
		#else
			#define COMPILER ALIF_COMPILER_VERSION("64 bit (Unknown)")
		#endif
	#endif


	#if defined(_WINDOWS32) && !defined(_WINDOWS64)
		#if defined(_M_IX86)
			#if defined(__clang__)
				#define COMPILER ("[Clang " __clang_version__ "] 32 bit (Intel) with MSC v." ALIF_STRINGIZE(_MSC_VER) " CRT]")
			#elif defined(__INTEL_COMPILER)
				#define COMPILER ("[ICC v." ALIF_STRINGIZE(__INTEL_COMPILER) " 32 bit (Intel) with MSC v." ALIF_STRINGIZE(_MSC_VER) " CRT]")
			#else
				#define COMPILER ALIF_COMPILER_VERSION("32 bit (Intel)")
			#endif
				#define ALIFD_PLATFORM_TAG "win32"
		#elif defined(_M_ARM)
			#define COMPILER ALIF_COMPILER_VERSION("32 bit (ARM)")
			#define ALIFD_PLATFORM_TAG "win_arm32"
		#else
			#define COMPILER ALIF_COMPILER_VERSION("32 bit (Unknown)")
		#endif
	#endif

	/* define some ANSI types that are not defined in earlier Win headers */
	#if _MSC_VER >= 1200
		/* This file only exists in VC 6.0 or higher */
		#include <basetsd.h>
	#endif
#endif



/* -------------------------------------- GNUC LCC ------------------------------------*/
/* egcs/gnu-win32 defines __GNUC__ and _WIN32 */
#if defined(__GNUC__) and defined(_WIN32)
#if (__GNUC__==2) and (__GNUC_MINOR__<=91)
#warning "Please use an up-to-date version of gcc! (>2.91 recommended)"
#endif

#define COMPILER "[gcc]"

/* lcc-win32 defines __LCC__ */
#elif defined(__LCC__)
#define COMPILER "[lcc-win32]"
#endif




#else


#define SIZEOF_WCHART 4
#define USE_PTHREADS
#define HAVE_PTHREAD_H 1

#endif


/* ------------------------------------- X86_X64 OS ------------------------------------- */
#if defined(_OS64)
	using AlifIntT = int32_t;
	using AlifUIntT = uint32_t;
	using AlifSizeT = int64_t;
	using AlifUSizeT = uint64_t;
	#define ALIF_SIZET_MAX LLONG_MAX
	#define ALIF_SIZET_MIN LLONG_MIN
	#define SIZEOF_SIZE_T 8
	#define ALIGNOF_SIZE_T 8
	#define SIZEOF_VOID_P 8
#else
	using AlifIntT = int16_t;
	using AlifUIntT = uint16_t;
	using AlifSizeT = int32_t;
	using AlifUSizeT = uint32_t;
	#define ALIF_SIZET INT_MAX
	#define ALIF_SIZET_MAX LLONG_MAX
	#define ALIF_SIZET_MIN LLONG_MIN
	#define SIZEOF_VOID_P 4
	#define ALIGNOF_SIZE_T 4
	#define SIZEOF_SIZE_T 4
#endif





/* ------------------------------------- Public ------------------------------------- */

#define WITH_FREELISTS 1

#define HAVE_SETVBUF