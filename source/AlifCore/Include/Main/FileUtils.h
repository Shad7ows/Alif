#pragma once


#ifdef HAVE_SYS_STAT_H
#  include <sys/stat.h>           // S_ISREG()
#elif defined(HAVE_STAT_H)
#  include <stat.h>               // S_ISREG()
#endif

// 14
#ifndef S_IFMT
#  define S_IFMT 0170000
#endif
#ifndef S_IFLNK
#  define S_IFLNK 0120000
#endif
#ifndef S_ISREG
#  define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#  define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISCHR
#  define S_ISCHR(x) (((x) & S_IFMT) == S_IFCHR)
#endif
#ifndef S_ISLNK
#  define S_ISLNK(x) (((x) & S_IFMT) == S_IFLNK)
#endif



wchar_t* alif_decodeLocale(const char*, AlifUSizeT*); // 44


#ifdef MS_WINDOWS // 78
class AlifStatClass {
public:
	uint64_t dev{};
	uint64_t ino{};
	unsigned short mode{};
	AlifIntT nLink{};
	AlifIntT uid{};
	AlifIntT gid{};
	unsigned long rDev{};
	__int64 size{};
	time_t aTime{};
	AlifIntT aTimeNsec{};
	time_t mTime{};
	AlifIntT mTimeNsec{};
	time_t cTime{};
	AlifIntT cTimeNsec{};
	time_t birthTime{};
	AlifIntT birthTimeNsec{};
	unsigned long fileAttributes{};
	unsigned long reparseTag{};
	uint64_t inoHigh{};
};
#else
#  define _ALIF_STAT_CLASS stat
#endif




/* --------------------------------------------------------------------------------------------- */






FILE* alif_fOpenObj(AlifObject*, const char*);
