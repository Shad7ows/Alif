#include "alif.h"
#include "alifcore_code.h"
#include "AlifCore_frame.h"
#include "AlifCore_DureRun.h"




// 7
#define MAX_CANDIDATE_ITEMS 750
#define MAX_STRING_SIZE 40

#define MOVE_COST 2
#define CASE_COST 1

#define LEAST_FIVE_BITS(n) ((n) & 31)

static inline AlifIntT substitution_cost(char _a, char _b) { // 16
	if (LEAST_FIVE_BITS(_a) != LEAST_FIVE_BITS(_b)) {
		// Not the same, not a case flip.
		return MOVE_COST;
	}
	if (_a == _b) {
		return 0;
	}
	if ('A' <= _a and _a <= 'Z') {
		_a += ('a' - 'A');
	}
	if ('A' <= _b and _b <= 'Z') {
		_b += ('a' - 'A');
	}
	if (_a == _b) {
		return CASE_COST;
	}
	return MOVE_COST;
}

static AlifSizeT levenshtein_distance(const char* _a, AlifSizeT _aSize,
	const char* _b, AlifSizeT _bSize,
	AlifSizeT _maxCost, AlifSizeT* _buffer)
{
	// Both strings are the same (by identity)
	if (_a == _b) {
		return 0;
	}

	// Trim away common affixes.
	while (_aSize and _bSize and _a[0] == _b[0]) {
		_a++; _aSize--;
		_b++; _bSize--;
	}
	while (_aSize and _bSize and _a[_aSize - 1] == _b[_bSize - 1]) {
		_aSize--;
		_bSize--;
	}
	if (_aSize == 0 or _bSize == 0) {
		return (_aSize + _bSize) * MOVE_COST;
	}
	if (_aSize > MAX_STRING_SIZE or _bSize > MAX_STRING_SIZE) {
		return _maxCost + 1;
	}

	// Prefer shorter buffer
	if (_bSize < _aSize) {
		const char* t = _a; _a = _b; _b = t;
		AlifSizeT tSize = _aSize; _aSize = _bSize; _bSize = tSize;
	}

	// quick fail when a match is impossible.
	if ((_bSize - _aSize) * MOVE_COST > _maxCost) {
		return _maxCost + 1;
	}

	// Instead of producing the whole traditional len(a)-by-len(b)
	// matrix, we can update just one row in place.
	// Initialize the buffer row
	AlifSizeT tmp = MOVE_COST;
	for (AlifSizeT i = 0; i < _aSize; i++) {
		// cost from b[:0] to a[:i+1]
		_buffer[i] = tmp;
		tmp += MOVE_COST;
	}

	AlifSizeT result = 0;
	for (AlifSizeT bindex = 0; bindex < _bSize; bindex++) {
		char code = _b[bindex];
		// cost(b[:b_index], a[:0]) == b_index * MOVE_COST
		AlifSizeT distance = result = bindex * MOVE_COST;
		AlifSizeT minimum = SIZE_MAX;
		for (AlifSizeT index = 0; index < _aSize; index++) {

			// cost(b[:b_index+1], a[:index+1]) = min(
			//     // 1) substitute
			//     cost(b[:b_index], a[:index])
			//         + substitution_cost(b[b_index], a[index]),
			//     // 2) delete from b
			//     cost(b[:b_index], a[:index+1]) + MOVE_COST,
			//     // 3) delete from a
			//     cost(b[:b_index+1], a[index]) + MOVE_COST
			// )

			// 1) Previous distance in this row is cost(b[:b_index], a[:index])
			AlifSizeT substitute = distance + substitution_cost(code, _a[index]);
			// 2) cost(b[:b_index], a[:index+1]) from previous row
			distance = _buffer[index];
			// 3) existing result is cost(b[:b_index+1], a[index])

			AlifSizeT insertDelete = ALIF_MIN(result, distance) + MOVE_COST;
			result = ALIF_MIN(insertDelete, substitute);

			// cost(b[:b_index+1], a[:index+1])
			_buffer[index] = result;
			if (result < minimum) {
				minimum = result;
			}
		}
		if (minimum > _maxCost) {
			// Everything in this row is too big, so bail early.
			return _maxCost + 1;
		}
	}
	return result;
}

AlifObject* _alif_calculateSuggestions(AlifObject* _dir,
	AlifObject* _name) { // 126
	AlifSizeT dirSize = ALIFLIST_GET_SIZE(_dir);
	if (dirSize >= MAX_CANDIDATE_ITEMS) {
		return nullptr;
	}

	AlifSizeT suggestion_distance = ALIF_SIZET_MAX;
	AlifObject* suggestion = nullptr;
	AlifSizeT nameSize{};
	const char* nameStr = alifUStr_asUTF8AndSize(_name, &nameSize);
	if (nameStr == nullptr) {
		return nullptr;
	}
	AlifSizeT* buffer =  (AlifSizeT*)alifMem_objAlloc(MAX_STRING_SIZE);
	if (buffer == nullptr) {
		//return alifErr_noMemory();
	}
	for (AlifSizeT i = 0; i < dirSize; ++i) {
		AlifObject* item = ALIFLIST_GET_ITEM(_dir, i);
		if (_alifUStr_equal(_name, item)) {
			continue;
		}
		AlifSizeT itemSize{};
		const char* itemStr = alifUStr_asUTF8AndSize(item, &itemSize);
		if (itemStr == nullptr) {
			alifMem_dataFree(buffer);
			return nullptr;
		}
		// No more than 1/3 of the involved characters should need changed.
		AlifSizeT maxDistance = (nameSize + itemSize + 3) * MOVE_COST / 6;
		// Don't take matches we've already beaten.
		maxDistance = ALIF_MIN(maxDistance, suggestion_distance - 1);
		AlifSizeT currentDistance =
			levenshtein_distance(nameStr, nameSize, itemStr,
				itemSize, maxDistance, buffer);
		if (currentDistance > maxDistance) {
			continue;
		}
		if (!suggestion or currentDistance < suggestion_distance) {
			suggestion = item;
			suggestion_distance = currentDistance;
		}
	}
	alifMem_dataFree(buffer);
	return ALIF_XNEWREF(suggestion);
}
