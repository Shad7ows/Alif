#pragma once










// 94
using AlifUCS4 = uint32_t;
using AlifUCS2 = uint16_t;
using AlifUCS1 = uint8_t;



extern AlifTypeObject _alifUStrType_; // 103

// 106
#define ALIFUSTR_CHECK(op) \
    ALIFTYPE_FASTSUBCLASS(ALIF_TYPE(op), ALIF_TPFLAGS_UNICODE_SUBCLASS)
#define ALIFUSTR_CHECKEXACT(op) ALIF_IS_TYPE((op), &_alifUStrType_) // 108



AlifObject* alifUStr_fromString(const char*); // 129




AlifObject* alifUStr_decodeUTF8Stateful(const char*, AlifSizeT, const char*, AlifSizeT*); // 435







/* ---------------------------------------------------------------------------------------------------------- */






/* ----------------------------------- Internal Unicode Operations ----------------------------------- */

// Static inline functions to work with surrogates
static inline int alifUnicode_isSurrogate(AlifUCS4 _ch) { // 16
	return (0xD800 <= _ch && _ch <= 0xDFFF);
}

// High surrogate = top 10 bits added to 0xD800.
// The character must be in the range [U+10000; U+10ffff].
static inline AlifUCS4 alifUnicode_highSurrogate(AlifUCS4 ch) { // 35
	return (0xD800 - (0x10000 >> 10) + (ch >> 10));
}

// Low surrogate = bottom 10 bits added to 0xDC00.
// The character must be in the range [U+10000; U+10ffff].
static inline AlifUCS4 alifUnicode_lowSurrogate(AlifUCS4 ch) { // 42
	return (0xDC00 + (ch & 0x3FF));
}








/* ---------------------------------------------------- UStr Type ---------------------------------------------------- */

class AlifASCIIObject { // 54
public:
	ALIFOBJECT_HEAD{};
	AlifSizeT length{};
	AlifHashT hash{};
	class {
	public:
		AlifUIntT interned : 2;
		AlifUIntT kind : 3;
		AlifUIntT compact : 1;
		AlifUIntT ascii : 1;
		AlifUIntT staticallyAllocated : 1;
		AlifUIntT : 24;
	} state{};
};

class AlifCompactUStrObject { // 156
public:
	AlifASCIIObject base{};
	AlifSizeT utf8Length{};
	char* utf8{};
};

class AlifUStrObject { // 164
public:
	AlifCompactUStrObject base{};
	union {
		void* any;
		AlifUCS1* latin1;
		AlifUCS2* ucs2;
		AlifUCS4* ucs4;
	} data{};
};


// 175
#define ALIFASCIIOBJECT_CAST(_op) ALIF_CAST(AlifASCIIObject*, _op)
#define ALIFCOMPACTUSTROBJECT_CAST(_op) ALIF_CAST(AlifCompactUStrObject*, _op)
#define ALIFUSTROBJECT_CAST(_op) ALIF_CAST(AlifUStrObject*, _op)


// 191
#define SSTATE_NOT_INTERNED 0
#define SSTATE_INTERNED_MORTAL 1
#define SSTATE_INTERNED_IMMORTAL 2
#define SSTATE_INTERNED_IMMORTAL_STATIC 3




static inline unsigned int alifUStr_isASCII(AlifObject* op) { // 211
	return ALIFASCIIOBJECT_CAST(op)->state.ascii;
}
#define ALIFUSTR_IS_ASCII(op) alifUStr_isASCII(ALIFOBJECT_CAST(op))

static inline AlifUIntT alifUstr_isCompact(AlifObject* _op) { // 218
	return ALIFASCIIOBJECT_CAST(_op)->state.compact;
}
#define ALIFUSTR_IS_COMPACT(_op) alifUstr_isCompact(ALIFOBJECT_CAST(_op))

static inline AlifIntT alifUStr_isCompactASCII(AlifObject* _op) { // 225
	return (ALIFASCIIOBJECT_CAST(_op)->state.ascii and ALIFUSTR_IS_COMPACT(_op));
}
#define ALIFUSTR_IS_COMPACT_ASCII(_op) alifUStr_isCompactASCII(ALIFOBJECT_CAST(_op))



enum AlifUStrKind_ { // 230
	AlifUStr_1Byte_Kind = 1,
	AlifUStr_2Byte_Kind = 2,
	AlifUStr_4Byte_Kind = 4
};



#define ALIFUSTR_KIND(_op) ALIF_RVALUE(ALIFASCIIOBJECT_CAST(_op)->state.kind) // 243

static inline void* alifUStr_compactData(AlifObject* _op) { // 246
	if (ALIFUSTR_IS_ASCII(_op)) {
		return ALIF_STATIC_CAST(void*, (ALIFASCIIOBJECT_CAST(_op) + 1));
	}
	return ALIF_STATIC_CAST(void*, (ALIFCOMPACTUSTROBJECT_CAST(_op) + 1));
}

static inline void* alifUStr_NonCompactData(AlifObject* op) { // 253
	void* data;
	data = ALIFUSTROBJECT_CAST(op)->data.any;
	return data;
}


static inline void* alifUStr_data(AlifObject* _op) { // 261
	if (ALIFUSTR_IS_COMPACT(_op)) {
		return alifUStr_compactData(_op);
	}
	return alifUStr_NonCompactData(_op);
}
#define ALIFUSTR_DATA(_op) alifUStr_data(ALIFOBJECT_CAST(_op))


// 274
#define ALIFUSTR_1BYTE_DATA(op) ALIF_STATIC_CAST(AlifUCS1*, ALIFUSTR_DATA(op))
#define ALIFUSTR_2BYTE_DATA(op) ALIF_STATIC_CAST(AlifUCS2*, ALIFUSTR_DATA(op))
#define ALIFUSTR_4BYTE_DATA(op) ALIF_STATIC_CAST(AlifUCS4*, ALIFUSTR_DATA(op))



static inline AlifSizeT alifUStr_getLength(AlifObject* _op) { // 279
	return ALIFASCIIOBJECT_CAST(_op)->length;
}
#define ALIFUSTR_GET_LENGTH(_op) alifUStr_getLength(ALIFOBJECT_CAST(_op))


static inline void alifUStr_write(int kind, void* data,
	AlifSizeT index, AlifUCS4 value) { // 289
	if (kind == AlifUStrKind_::AlifUStr_1Byte_Kind) {
		ALIF_STATIC_CAST(AlifUCS1*, data)[index] = ALIF_STATIC_CAST(AlifUCS1, value);
	}
	else if (kind == AlifUStrKind_::AlifUStr_2Byte_Kind) {
		ALIF_STATIC_CAST(AlifUCS2*, data)[index] = ALIF_STATIC_CAST(AlifUCS2, value);
	}
	else {
		ALIF_STATIC_CAST(AlifUCS4*, data)[index] = value;
	}
}
#define ALIFUSTR_WRITE(kind, data, index, value) \
    alifUStr_write(ALIF_STATIC_CAST(int, kind), ALIF_CAST(void*, data), (index), ALIF_STATIC_CAST(AlifUCS4, value))

static inline AlifUCS4 alifUStr_read(int kind,
	const void* data, AlifSizeT index) { // 313
	if (kind == AlifUStrKind_::AlifUStr_1Byte_Kind) {
		return ALIF_STATIC_CAST(const AlifUCS1*, data)[index];
	}
	if (kind == AlifUStrKind_::AlifUStr_2Byte_Kind) {
		return ALIF_STATIC_CAST(const AlifUCS2*, data)[index];
	}
	return ALIF_STATIC_CAST(const AlifUCS4*, data)[index];
}
#define ALIFUSTR_READ(kind, data, index) \
    alifUStr_read(ALIF_STATIC_CAST(int, kind), ALIF_STATIC_CAST(const void*, data), (index))


static inline AlifUCS4 alifUStr_maxCharValue(AlifObject* _op) { // 359
	AlifIntT kind{};

	if (ALIFUSTR_IS_ASCII(_op)) {
		return 0x7fU;
	}

	kind = ALIFUSTR_KIND(_op);
	if (kind == AlifUStrKind_::AlifUStr_1Byte_Kind) {
		return 0xffU;
	}
	if (kind == AlifUStrKind_::AlifUStr_2Byte_Kind) {
		return 0xffffU;
	}
	return 0x10ffffU;
}
#define ALIFUSTR_MAX_CHAR_VALUE(_op) alifUStr_maxCharValue(ALIFOBJECT_CAST(_op))









class AlifUStrWriter { // 496
public:
	AlifObject* buffer{};
	void* data{};
	int kind{};
	AlifUCS4 maxChar{};
	AlifSizeT size{};
	AlifSizeT pos{};
	AlifSizeT minLength{};
	AlifUCS4 minChar{};
	unsigned char overAllocate{};
	unsigned char readOnly{};
};



// 530
#define ALIFUSTRWRITER_PREPARE(_writer, _length, _maxChar)             \
    (((_maxChar) <= (_writer)->maxChar                                  \
      && (_length) <= (_writer)->size - (_writer)->pos)                  \
     ? 0                                                              \
     : (((_length) == 0)                                               \
        ? 0                                                           \
        : alifUStrWriter_prepareInternal((_writer), (_length), (_maxChar))))


AlifIntT alifUStrWriter_prepareInternal(AlifUStrWriter*, AlifSizeT, AlifUCS4);



// 549
#define ALIFUSTRWRITER_PREPAREKIND(_writer, _kind)                    \
    ((_kind) <= (_writer)->kind                                         \
     ? 0                                                              \
     : alifUStrWriter_prepareKindInternal((_writer), (_kind)))

AlifIntT alifUStrWriter_prepareKindInternal(AlifUStrWriter*, AlifIntT); // 557



AlifObject* alifUStrWriter_finish(AlifUStrWriter*); // 602


void alifUStrWriter_dealloc(AlifUStrWriter*); // 607