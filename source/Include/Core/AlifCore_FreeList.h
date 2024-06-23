#pragma once




#ifdef WITH_FREELISTS
#  define ALIFTUPLE_MAXSAVESIZE 20
#  define ALIFTUPLE_NFREELISTS ALIFTUPLE_MAXSAVESIZE
#  define ALIFTUPLE_MAXFREELIST 2000
#  define ALIFLIST_MAXFREELIST 80
#  define ALIFDICT_MAXFREELIST 80
#  define ALIFFLOAT_MAXFREELIST 100
#  define ALIFCONTEXT_MAXFREELIST 255
# define ALIFASYNCGEN_MAXFREELIST 80
#  define ALIFOBJECTSTACKCHUNK_MAXFREELIST 4
#else
#  define ALIFTUPLE_NFREELISTS 0
#  define ALIFTUPLE_MAXFREELIST 0
#  define ALIFLIST_MAXFREELIST 0
#  define ALIFDICT_MAXFREELIST 0
#  define ALIFFLOAT_MAXFREELIST 0
#  define ALIFCONTEXT_MAXFREELIST 0
#  define ALIFASYNCGEN_MAXFREELIST 0
#  define ALIFOBJECTSTACKCHUNK_MAXFREELIST 0
#endif

class AlifListFreeList { // 36
public:
#ifdef WITH_FREELISTS
	AlifListObject* items[ALIFLIST_MAXFREELIST];
	AlifIntT numfree;
#endif
};



class AlifObjectFreeLists {
public:
	AlifListFreeList lists{};
};