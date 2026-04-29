#include "alif.h"

#include "AlifCore_Code.h"
#include "AlifCore_Frame.h"
#include "AlifCore_Errors.h"
#include "AlifCore_Runtime.h"


#define MAX_CANDIDATE_ITEMS 750
#define MAX_STRING_SIZE 40

#define MOVE_COST 2
#define CASE_COST 1

#define LEAST_FIVE_BITS(n) ((n) & 31)



static inline AlifIntT substitution_cost(char a, char b) {
	if (LEAST_FIVE_BITS(a) != LEAST_FIVE_BITS(b)) {
		// Not the same, not a case flip.
		return MOVE_COST;
	}
	if (a == b) {
		return 0;
	}
	if ('A' <= a and a <= 'Z') {
		a += ('a' - 'A');
	}
	if ('A' <= b and b <= 'Z') {
		b += ('a' - 'A');
	}
	if (a == b) {
		return CASE_COST;
	}
	return MOVE_COST;
}


static AlifSizeT levenshtein_distance(const char *a, AlifUSizeT a_size,
	const char *b, AlifUSizeT b_size,
	AlifUSizeT max_cost, AlifUSizeT *buffer)
{
	// Both strings are the same (by identity)
	if (a == b) {
		return 0;
	}

	// Trim away common affixes.
	while (a_size && b_size && a[0] == b[0]) {
		a++; a_size--;
		b++; b_size--;
	}
	while (a_size && b_size && a[a_size-1] == b[b_size-1]) {
		a_size--;
		b_size--;
	}
	if (a_size == 0 || b_size == 0) {
		return (a_size + b_size) * MOVE_COST;
	}
	if (a_size > MAX_STRING_SIZE || b_size > MAX_STRING_SIZE) {
		return max_cost + 1;
	}

	// Prefer shorter buffer
	if (b_size < a_size) {
		const char *t = a; a = b; b = t;
		AlifUSizeT t_size = a_size; a_size = b_size; b_size = t_size;
	}

	// quick fail when a match is impossible.
	if ((b_size - a_size) * MOVE_COST > max_cost) {
		return max_cost + 1;
	}

	// Instead of producing the whole traditional len(a)-by-len(b)
	// matrix, we can update just one row in place.
	// Initialize the buffer row
	AlifUSizeT tmp = MOVE_COST;
	for (AlifUSizeT i = 0; i < a_size; i++) {
		// cost from b[:0] to a[:i+1]
		buffer[i] = tmp;
		tmp += MOVE_COST;
	}

	AlifUSizeT result = 0;
	for (AlifUSizeT b_index = 0; b_index < b_size; b_index++) {
		char code = b[b_index];
		// cost(b[:b_index], a[:0]) == b_index * MOVE_COST
		AlifUSizeT distance = result = b_index * MOVE_COST;
		AlifUSizeT minimum = SIZE_MAX;
		for (AlifUSizeT index = 0; index < a_size; index++) {

			// 1) Previous distance in this row is cost(b[:b_index], a[:index])
			AlifUSizeT substitute = distance + substitution_cost(code, a[index]);
			// 2) cost(b[:b_index], a[:index+1]) from previous row
			distance = buffer[index];
			// 3) existing result is cost(b[:b_index+1], a[index])

			AlifUSizeT insert_delete = ALIF_MIN(result, distance) + MOVE_COST;
			result = ALIF_MIN(insert_delete, substitute);

			// cost(b[:b_index+1], a[:index+1])
			buffer[index] = result;
			if (result < minimum) {
				minimum = result;
			}
		}
		if (minimum > max_cost) {
			// Everything in this row is too big, so bail early.
			return max_cost + 1;
		}
	}
	return result;
}



AlifObject* _alif_calculateSuggestions(AlifObject* _dir,
	AlifObject* _name) {
	AlifSizeT dir_size = ALIFLIST_GET_SIZE(_dir);
	if (dir_size >= MAX_CANDIDATE_ITEMS) {
		return nullptr;
	}

	AlifSizeT suggestion_distance = ALIF_SIZET_MAX;
	AlifObject* suggestion{};
	AlifSizeT nameSize{};
	const char *nameStr = alifUStr_asUTF8AndSize(_name, &nameSize);
	if (nameStr == nullptr) {
		return nullptr;
	}
	AlifUSizeT *buffer = ((AlifUSizeT)MAX_STRING_SIZE > ALIF_SIZET_MAX / sizeof(AlifUSizeT)) ? nullptr : \
		(AlifUSizeT*)alifMem_dataAlloc(MAX_STRING_SIZE * sizeof(AlifUSizeT)); //* alif
	if (buffer == nullptr) {
		//return alifErr_noMemory();
		return nullptr;
	}
	for (AlifSizeT i = 0; i < dir_size; ++i) {
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
		AlifSizeT max_distance = (nameSize + itemSize + 3) * MOVE_COST / 6;
		// Don't take matches we've already beaten.
		max_distance = ALIF_MIN(max_distance, suggestion_distance - 1);
		AlifSizeT current_distance =
			levenshtein_distance(nameStr, nameSize, itemStr,
				itemSize, max_distance, buffer);
		if (current_distance > max_distance) {
			continue;
		}
		if (!suggestion || current_distance < suggestion_distance) {
			suggestion = item;
			suggestion_distance = current_distance;
		}
	}
	alifMem_dataFree(buffer);
	return ALIF_XNEWREF(suggestion);
}
