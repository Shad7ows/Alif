#include "alif.h"

#include "AlifCore_ModSupport.h"    
#include "AlifCore_LifeCycle.h"


#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfU    /* constant vector a */
#define UPPER_MASK 0x80000000U  /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffU  /* least significant r bits */

class RandomState { // 96
public:
	AlifObject* randomType{};
	AlifObject* long___abs__{};
};

static inline RandomState* get_randomState(AlifObject* _module) { // 102
	void* state = alifModule_getState(_module);
	return (RandomState*)state;
}

static class AlifModuleDef _randomModule;

#define _randomstate_type(type) \
    (get_randomState(alifType_getModuleByDef(type, &_randomModule)))


typedef class RandomObject{ // 114
public:
	ALIFOBJECT_HEAD;
	AlifIntT index{};
	uint32_t state[N];
};

#include "../clinic/_RandomModule.cpp.h"


static uint32_t genrand_uint32(RandomObject* _self) { // 33
	uint32_t y{};
	static const uint32_t mag01[2] = { 0x0U, MATRIX_A };
	/* mag01[x] = x * MATRIX_A  for x=0,1 */
	uint32_t* mt{};

	mt = _self->state;
	if (_self->index >= N) { /* generate N words at one time */
		AlifIntT kk{};

		for (kk = 0;kk < N - M;kk++) {
			y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
			mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1U];
		}
		for (;kk < N - 1;kk++) {
			y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
			mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1U];
		}
		y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
		mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1U];

		_self->index = 0;
	}

	y = mt[_self->index++];
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680U;
	y ^= (y << 15) & 0xefc60000U;
	y ^= (y >> 18);
	return y;
}

static AlifObject* randomRandom_randomImpl(RandomObject* _self) { // 187
	uint32_t a = genrand_uint32(_self) >> 5, b = genrand_uint32(_self) >> 6;
	return alifFloat_fromDouble((a * 67108864.0 + b) * (1.0 / 9007199254740992.0));
}

/* initializes mt[N] with a seed */
static void init_genrand(RandomObject* _self, uint32_t _s) { // 196
	AlifIntT mti{};
	uint32_t* mt{};

	mt = _self->state;
	mt[0] = _s;
	for (mti = 1; mti < N; mti++) {
		mt[mti] =
			(1812433253U * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array mt[].                                */
		/* 2002/01/09 modified by Makoto Matsumoto                     */
	}
	_self->index = mti;
	return;
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
static void init_byArray(RandomObject* _self, uint32_t _initKey[], AlifSizeT _keyLength) {// 
	AlifSizeT i{}, j{}, k{};       /* was signed in the original code. RDH 12/16/2002 */
	uint32_t* mt{};

	mt = _self->state;
	init_genrand(_self, 19650218U);
	i = 1; j = 0;
	k = (N > _keyLength ? N : _keyLength);
	for (; k; k--) {
		mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525U))
			+ _initKey[j] + (uint32_t)j; /* non linear */
		i++; j++;
		if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
		if (j >= _keyLength) j = 0;
	}
	for (k = N - 1; k; k--) {
		mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941U))
			- (uint32_t)i; /* non linear */
		i++;
		if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
	}

	mt[0] = 0x80000000U; /* MSB is 1; assuring non-zero initial array */
}

static AlifIntT random_seedUrandom(RandomObject* _self) { // 251
	uint32_t key[N]{};

	if (_alifOS_uRandomNonblock(key, sizeof(key)) < 0) {
		return -1;
	}
	init_byArray(_self, key, ALIF_ARRAY_LENGTH(key));
	return 0;
}

static AlifIntT
random_seed_time_pid(RandomObject* self) { // 263
	AlifTimeT now{};
	if (alifTime_time(&now) < 0) {
		return -1;
	}

	uint32_t key[5]{};
	key[0] = (uint32_t)(now & 0xffffffffU);
	key[1] = (uint32_t)(now >> 32);

#if defined(MS_WINDOWS) and !defined(MS_WINDOWS_DESKTOP) && !defined(MS_WINDOWS_SYSTEM)
	key[2] = (uint32_t)GetCurrentProcessId();
#elif defined(HAVE_GETPID)
	key[2] = (uint32_t)getpid();
#else
	key[2] = 0;
#endif

	if (alifTime_monotonic(&now) < 0) {
		return -1;
	}
	key[3] = (uint32_t)(now & 0xffffffffU);
	key[4] = (uint32_t)(now >> 32);

	init_byArray(self, key, ALIF_ARRAY_LENGTH(key));
	return 0;
}

static AlifIntT random_seed(RandomObject* _self, AlifObject* _arg) { // 239
	AlifIntT result = -1;  /* guilty until proved innocent */
	AlifObject* n = nullptr;
	uint32_t* key = nullptr;
	int64_t bits{};
	size_t keyUsed{};
	AlifIntT res{};

	if (_arg == nullptr or _arg == ALIF_NONE) {
		if (random_seedUrandom(_self) < 0) {
			alifErr_clear();
			if (random_seed_time_pid(_self) < 0) {
				return -1;
			}
		}
		return 0;
	}

	if (ALIFLONG_CHECKEXACT(_arg)) {
		n = alifNumber_absolute(_arg);
	}
	else if (ALIFLONG_CHECK(_arg)) {

		RandomState* state = _randomstate_type(ALIF_TYPE(_self));
		n = alifObject_callOneArg(state->long___abs__, _arg);
	}
	else {
		AlifHashT hash = alifObject_hash(_arg);
		if (hash == -1)
			goto Done;
		n = alifLong_fromSizeT((size_t)hash);
	}
	if (n == nullptr)
		goto Done;

	/* Now split n into 32-bit chunks, from the right. */
	bits = _alifLong_numBits(n);

	keyUsed = bits == 0 ? 1 : (size_t)((bits - 1) / 32 + 1);

	/* Convert seed to byte sequence. */
	key = (uint32_t*)alifMem_dataAlloc((size_t)4 * keyUsed);
	if (key == nullptr) {
		//alifErr_noMemory();
		goto Done;
	}
	res = _alifLong_asByteArray((AlifLongObject*)n,
		(unsigned char*)key, keyUsed * 4,
		ALIF_LITTLE_ENDIAN,
		0, /* unsigned */
		1); /* with exceptions */
	if (res == -1) {
		goto Done;
	}

#if ALIF_BIG_ENDIAN
	{
		AlifSizeT i{}, j{};
		for (i = 0, j = keyUsed - 1; i < j; i++, j--) {
			uint32_t tmp = key[i];
			key[i] = key[j];
			key[j] = tmp;
		}
	}
#endif
	init_byArray(_self, key, keyUsed);

	result = 0;

Done:
	ALIF_XDECREF(n);
	alifMem_dataFree(key);
	return result;
}

static AlifIntT random_init(RandomObject* _self, AlifObject* _args, AlifObject* _kwDs) { // 554
	AlifObject* arg = nullptr;
	RandomState* state = _randomstate_type(ALIF_TYPE(_self));

	if ((ALIF_IS_TYPE(_self, (AlifTypeObject*)state->randomType) or
		ALIF_TYPE(_self)->init == ((AlifTypeObject*)state->randomType)->init) and
		!_ALIFARG_NOKEYWORDS("عشوائي", _kwDs)) {
		return -1;
	}

	if (ALIFTUPLE_GET_SIZE(_args) > 1) {
		alifErr_setString(_alifExcTypeError_, "Random() requires 0 or 1 argument");
		return -1;
	}

	if (ALIFTUPLE_GET_SIZE(_args) == 1)
		arg = ALIFTUPLE_GET_ITEM(_args, 0);

	return random_seed(_self, arg);
}

static AlifMethodDef _randomMethods_[] = { // 577
	_RANDOM_RANDOM_RANDOM_METHODDEF
	{nullptr,              nullptr}           /* sentinel */
};

static AlifTypeSlot _randomTypeSlots_[] = { // 589
	{},
	{ALIF_TP_METHODS, _randomMethods_},
	{},
	{ALIF_TP_INIT, random_init},
	{0, 0},
};

static AlifTypeSpec _randomTypeSpec_ = { // 598
	"_random.Random",
	sizeof(RandomObject),
	0,
	ALIF_TPFLAGS_DEFAULT | ALIF_TPFLAGS_BASETYPE,
	_randomTypeSlots_
};


static AlifIntT _random_exec(AlifObject* _module) { // 610
	RandomState* state = get_randomState(_module);

	state->randomType = alifType_fromModuleAndSpec(
		_module, &_randomTypeSpec_, nullptr);
	if (state->randomType == nullptr) {
		return -1;
	}
	if (alifModule_addType(_module, (AlifTypeObject*)state->randomType) < 0) {
		return -1;
	}

	/* Look up and save AlifIntT.__abs__, which is needed in random_seed(). */
	AlifObject* longval = alifLong_fromLong(0);
	if (longval == nullptr) {
		return -1;
	}

	AlifObject* longtype = alifObject_type(longval);
	ALIF_DECREF(longval);
	if (longtype == nullptr) {
		return -1;
	}

	state->long___abs__ = alifObject_getAttrString(longtype, "__abs__");
	ALIF_DECREF(longtype);
	if (state->long___abs__ == nullptr) {
		return -1;
	}
	return 0;
}

static AlifModuleDefSlot _randomSlots_[] = {
	{ALIF_MOD_EXEC, _random_exec},
	{0, nullptr}
};

static AlifIntT _random_clear(AlifObject* _module) { // 658
	ALIF_CLEAR(get_randomState(_module)->randomType);
	ALIF_CLEAR(get_randomState(_module)->long___abs__);
	return 0;
}

static AlifIntT _random_traverse(AlifObject* _module, VisitProc visit, void* arg) { // 651
	ALIF_VISIT(get_randomState(_module)->randomType);
	return 0;
}

static void _random_free(void* _module) { // 666
	_random_clear((AlifObject*)_module);
}

static class AlifModuleDef _randomModule_ = { // 671
	ALIFMODULEDEF_HEAD_INIT,
	"عشوائي",
	nullptr,
	sizeof(RandomState),
	nullptr,
	_randomSlots_,
	_random_traverse,
	_random_clear,
	_random_free,
};

AlifObject* alifInit__random(void)
{
	return alifModuleDef_init(&_randomModule_);
}
