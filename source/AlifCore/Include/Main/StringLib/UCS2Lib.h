
#define STRINGLIB_IS_UNICODE     1

// 6
#define STRINGLIB(F)            ucs2Lib_##F
#define STRINGLIB_OBJECT         AlifUStrObject
#define STRINGLIB_SIZEOF_CHAR    2
#define STRINGLIB_MAX_CHAR       0xFFFFu
#define STRINGLIB_CHAR           AlifUCS2
