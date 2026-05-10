#include "alif.h"

#include "Opcode.h"

#include "AlifCore_Code.h"
#include "AlifCore_Dict.h" 
#include "AlifCore_Function.h" 
#include "AlifCore_Long.h" 
#include "AlifCore_Moduleobject.h"
#include "AlifCore_Object.h"
#include "AlifCore_OpcodeMetadata.h"
#include "AlifCore_OpcodeUtils.h"
#include "AlifCore_LifeCycle.h"
#include "AlifCore_Runtime.h"

#include <stdlib.h> // rand()






// 27
#define SET_OPCODE_OR_RETURN(_instr, _opcode)                                \
    do {                                                                   \
        uint8_t old_op = alifAtomic_loadUint8Relaxed(&(_instr)->op.code); \
        if (old_op >= MIN_INSTRUMENTED_OPCODE) {                           \
            /* Lost race with instrumentation */                           \
            return;                                                        \
        }                                                                  \
        if (!alifAtomic_compareExchangeUint8(&(_instr)->op.code, &old_op, \
                                               (_opcode))) {                \
            /* Lost race with instrumentation */                           \
            return;                                                        \
        }                                                                  \
    } while (0)






// Initialize warmup counters and optimize instructions. This cannot fail.
void _alifCode_quicken(AlifCodeUnit* _instructions, AlifSizeT _size,
	AlifObject* _consts, AlifIntT _enableCounters) { // 459
#if ENABLE_SPECIALIZATION_FT
	AlifBackoffCounter jumpCounter{}, adaptiveCounter{};
	if (_enableCounters) {
		jumpCounter = initial_jumpBackoffCounter();
		adaptiveCounter = adaptive_counterWarmup();
	}
	else {
		jumpCounter = initial_unreachableBackoffCounter();
		adaptiveCounter = initial_unreachableBackoffCounter();
	}
	AlifIntT opcode = 0;
	AlifIntT oparg = 0;
	/* The last code unit cannot have a cache, so we don't need to check it */
	for (AlifSizeT i = 0; i < _size-1; i++) {
		opcode = _instructions[i].op.code;
		AlifIntT caches = _alifOpcodeCaches_[opcode];
		oparg = (oparg << 8) | _instructions[i].op.arg;
		if (caches) {
			// The initial value depends on the opcode
			switch (opcode) {
			case JUMP_BACKWARD:
				_instructions[i + 1].counter = jumpCounter;
				break;
			case POP_JUMP_IF_FALSE:
			case POP_JUMP_IF_TRUE:
			case POP_JUMP_IF_NONE:
			case POP_JUMP_IF_NOT_NONE:
				_instructions[i + 1].cache = 0x5555;  // Alternating 0, 1 bits
				break;
			default:
				_instructions[i + 1].counter = adaptiveCounter;
				break;
			}
			i += caches;
		}
		else if (opcode == LOAD_CONST) {
			/* We can't do this in the bytecode compiler as
			* marshalling can intern strings and make them immortal. */

			AlifObject *obj = ALIFTUPLE_GET_ITEM(_consts, oparg);
			if (ALIF_ISIMMORTAL(obj)) {
				_instructions[i].op.code = LOAD_CONST_IMMORTAL;
			}
		}
		if (opcode != EXTENDED_ARG) {
			oparg = 0;
		}
	}
#endif /* ENABLE_SPECIALIZATION_FT */
}













void _alifSpecialize_binaryOp(AlifStackRef _lhsSt, AlifStackRef _rhsSt,
	AlifCodeUnit* _instr, AlifIntT _oparg, AlifStackRef* _locals) { // 2268
	AlifObject *lhs = alifStackRef_asAlifObjectBorrow(_lhsSt);
	AlifObject *rhs = alifStackRef_asAlifObjectBorrow(_rhsSt);
	AlifBinaryOpCache* cache = (AlifBinaryOpCache*)(_instr + 1);
	uint8_t specializedOp{};
	switch (_oparg) {
	case NB_ADD:
	case NB_INPLACE_ADD:
		if (!ALIF_IS_TYPE(lhs, ALIF_TYPE(rhs))) {
			break;
		}
		if (ALIFUSTR_CHECKEXACT(lhs)) {
			AlifCodeUnit next = _instr[INLINE_CACHE_ENTRIES_BINARY_OP + 1];
			bool toStore = (next.op.code == STORE_FAST);
			if (toStore and alifStackRef_asAlifObjectBorrow(_locals[next.op.arg]) == lhs) {
				specializedOp = BINARY_OP_INPLACE_ADD_UNICODE;
				goto success;
			}
			specializedOp = BINARY_OP_ADD_UNICODE;
			goto success;
		}
		if (ALIFLONG_CHECKEXACT(lhs)) {
			specializedOp = BINARY_OP_ADD_INT;
			goto success;
		}
		if (ALIFFLOAT_CHECKEXACT(lhs)) {
			specializedOp = BINARY_OP_ADD_FLOAT;
			goto success;
		}
		break;
	case NB_MULTIPLY:
	case NB_INPLACE_MULTIPLY:
		if (!ALIF_IS_TYPE(lhs, ALIF_TYPE(rhs))) {
			break;
		}
		if (ALIFLONG_CHECKEXACT(lhs)) {
			specializedOp = BINARY_OP_MULTIPLY_INT;
			goto success;
		}
		if (ALIFFLOAT_CHECKEXACT(lhs)) {
			specializedOp = BINARY_OP_MULTIPLY_FLOAT;
			goto success;
		}
		break;
	case NB_SUBTRACT:
	case NB_INPLACE_SUBTRACT:
		if (!ALIF_IS_TYPE(lhs, ALIF_TYPE(rhs))) {
			break;
		}
		if (ALIFLONG_CHECKEXACT(lhs)) {
			specializedOp = BINARY_OP_SUBTRACT_INT;
			goto success;
		}
		if (ALIFFLOAT_CHECKEXACT(lhs)) {
			specializedOp = BINARY_OP_SUBTRACT_FLOAT;
			goto success;
		}
		break;
	}
	//SPECIALIZATION_FAIL(BINARY_OP, _binaryOpFailKind_(oparg, lhs, rhs));
	//STAT_INC(BINARY_OP, failure);
	SET_OPCODE_OR_RETURN(_instr, BINARY_OP);
	cache->counter = adaptive_counterBackoff(cache->counter);
	return;
success:
	//STAT_INC(BINARY_OP, success);
	SET_OPCODE_OR_RETURN(_instr, specializedOp);
	cache->counter = adaptive_counterCooldown();
}




void _alifSpecialize_containsOp(AlifStackRef _valueSt, AlifCodeUnit* _instr) { // 2745
	AlifObject* value = alifStackRef_asAlifObjectBorrow(_valueSt);

	uint8_t specializedOp{};
	AlifContainsOpCache* cache = (AlifContainsOpCache*)(_instr + 1);
	if (ALIFDICT_CHECKEXACT(value)) {
		specializedOp = CONTAINS_OP_DICT;
		goto success;
	}
	if (ALIFSET_CHECKEXACT(value) or ALIFFROZENSET_CHECKEXACT(value)) {
		specializedOp = CONTAINS_OP_SET;
		goto success;
	}

	//SPECIALIZATION_FAIL(CONTAINS_OP, containsOp_failKind(value));
	//STAT_INC(CONTAINS_OP, failure);
	SET_OPCODE_OR_RETURN(_instr, CONTAINS_OP);
	cache->counter = adaptive_counterBackoff(cache->counter);
	return;
success:
	//STAT_INC(CONTAINS_OP, success);
	SET_OPCODE_OR_RETURN(_instr, specializedOp);
	cache->counter = adaptive_counterCooldown();
}
