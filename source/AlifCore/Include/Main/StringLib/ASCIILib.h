

#define STRINGLIB_IS_UNICODE     1

#define STRINGLIB(_F)			 asciiLib_##_F
#define STRINGLIB_OBJECT         AlifUStrObject
#define STRINGLIB_SIZEOF_CHAR    1
#define STRINGLIB_MAX_CHAR       0x7Fu
#define STRINGLIB_CHAR           AlifUCS1
