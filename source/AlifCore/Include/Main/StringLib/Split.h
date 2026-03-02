



// 14
#define MAX_PREALLOC 12

/* 5 splits gives 6 elements */
#define PREALLOC_SIZE(maxsplit) (maxsplit >= MAX_PREALLOC ? MAX_PREALLOC : maxsplit+1)

// 32
#define SPLIT_ADD(data, left, right) {          \
    sub = STRINGLIB_NEW((data) + (left),        \
                        (right) - (left));      \
    if (sub == nullptr)                            \
        goto onError;                           \
    if (count < MAX_PREALLOC) {                 \
        ALIFLIST_SET_ITEM(list, count, sub);      \
    } else {                                    \
        if (alifList_append(list, sub)) {         \
            ALIF_DECREF(sub);                     \
            goto onError;                       \
        }                                       \
        else                                    \
            ALIF_DECREF(sub);                    \
    }                                           \
    count++; }


// 51
#define FIX_PREALLOC_SIZE(list) ALIF_SET_SIZE(list, count)

ALIF_LOCAL_INLINE(AlifObject*) STRINGLIB(splitWhitespace)(AlifObject* str_obj,
	const STRINGLIB_CHAR* str, AlifSizeT str_len,
	AlifSizeT maxcount) {
	AlifSizeT i{}, j{}, count = 0;
	AlifObject* list = alifList_new(PREALLOC_SIZE(maxcount));
	AlifObject* sub{};

	if (list == nullptr)
		return nullptr;

	i = j = 0;
	while (maxcount-- > 0) {
		while (i < str_len and STRINGLIB_ISSPACE(str[i]))
			i++;
		if (i == str_len) break;
		j = i; i++;
		while (i < str_len and !STRINGLIB_ISSPACE(str[i]))
			i++;
	#if !STRINGLIB_MUTABLE
		if (j == 0 and i == str_len and STRINGLIB_CHECK_EXACT(str_obj)) {
			/* No whitespace in strObj, so just use it as list[0] */
			ALIF_INCREF(str_obj);
			ALIFLIST_SET_ITEM(list, 0, (AlifObject*)str_obj);
			count++;
			break;
		}
	#endif
		SPLIT_ADD(str, j, i);
	}

	if (i < str_len) {
		/* Only occurs when maxcount was reached */
		/* Skip any remaining whitespace and copy to end of string */
		while (i < str_len and STRINGLIB_ISSPACE(str[i]))
			i++;
		if (i != str_len)
			SPLIT_ADD(str, i, str_len);
	}
	FIX_PREALLOC_SIZE(list);
	return list;

onError:
	ALIF_DECREF(list);
	return nullptr;
}

// 192
ALIF_LOCAL_INLINE(AlifObject*)
STRINGLIB(rsplitWhitespace)(AlifObject* strObj,
	const STRINGLIB_CHAR* str, AlifSizeT strLen,
	AlifSizeT maxcount) {
	AlifSizeT i{}, j{}, count = 0;
	AlifObject* list = alifList_new(PREALLOC_SIZE(maxcount));
	AlifObject* sub;

	if (list == nullptr)
		return nullptr;

	i = j = strLen - 1;
	while (maxcount-- > 0) {
		while (i >= 0 and STRINGLIB_ISSPACE(str[i]))
			i--;
		if (i < 0) break;
		j = i; i--;
		while (i >= 0 and !STRINGLIB_ISSPACE(str[i]))
			i--;
	#if !STRINGLIB_MUTABLE
		if (j == strLen - 1 and i < 0 and STRINGLIB_CHECK_EXACT(strObj)) {
			/* No whitespace in strObj, so just use it as list[0] */
			ALIF_INCREF(strObj);
			ALIFLIST_SET_ITEM(list, 0, (AlifObject *)strObj);
			count++;
			break;
		}
	#endif
		SPLIT_ADD(str, i + 1, j + 1);
	}

	if (i >= 0) {
		/* Only occurs when maxcount was reached */
		/* Skip any remaining whitespace and copy to beginning of string */
		while (i >= 0 and STRINGLIB_ISSPACE(str[i]))
			i--;
		if (i >= 0)
			SPLIT_ADD(str, 0, i + 1);
	}
	FIX_PREALLOC_SIZE(list);
	if (alifList_reverse(list) < 0)
		goto onError;
	return list;

onError:
	ALIF_DECREF(list);
	return nullptr;
}


// 242
ALIF_LOCAL_INLINE(AlifObject*)
STRINGLIB(rsplitChar)(AlifObject* strObj,
	const STRINGLIB_CHAR* str, AlifSizeT strLen,
	const STRINGLIB_CHAR ch,
	AlifSizeT maxcount) {
	AlifSizeT i{}, j{}, count = 0;
	AlifObject* list = alifList_new(PREALLOC_SIZE(maxcount));
	AlifObject* sub{};

	if (list == nullptr)
		return nullptr;

	i = j = strLen - 1;
	while ((i >= 0) and (maxcount-- > 0)) {
		for(; i >= 0; i--) {
			if (str[i] == ch) {
				SPLIT_ADD(str, i + 1, j + 1);
				j = i = i - 1;
				break;
			}
		}
	}
#if !STRINGLIB_MUTABLE
	if (count == 0 and STRINGLIB_CHECK_EXACT(strObj)) {
		/* ch not in strObj, so just use strObj as list[0] */
		ALIF_INCREF(strObj);
		ALIFLIST_SET_ITEM(list, 0, (AlifObject*)strObj);
		count++;
	} else
	#endif
		if (j >= -1) {
			SPLIT_ADD(str, 0, j + 1);
		}
	FIX_PREALLOC_SIZE(list);
	if (alifList_reverse(list) < 0)
		goto onError;
	return list;

onError:
	ALIF_DECREF(list);
	return nullptr;
}



// 286
ALIF_LOCAL_INLINE(AlifObject*)
STRINGLIB(rsplit)(AlifObject* strObj,
	const STRINGLIB_CHAR* str, AlifSizeT strLen,
	const STRINGLIB_CHAR* sep, AlifSizeT sepLen,
	AlifSizeT maxcount) {
	AlifSizeT j{}, pos{}, count = 0;
	AlifObject* list{}, * sub{};

	if (sepLen == 0) {
		alifErr_setString(_alifExcValueError_, "لا يوجد فاصل");
		return nullptr;
	}
	else if (sepLen == 1)
		return STRINGLIB(rsplitChar)(strObj, str, strLen, sep[0], maxcount);

	list = alifList_new(PREALLOC_SIZE(maxcount));
	if (list == nullptr)
		return nullptr;

	j = strLen;
	while (maxcount-- > 0) {
		pos = FASTSEARCH(str, j, sep, sepLen, -1, FAST_RSEARCH);
		if (pos < 0)
			break;
		SPLIT_ADD(str, pos + sepLen, j);
		j = pos;
	}
#if !STRINGLIB_MUTABLE
	if (count == 0 and STRINGLIB_CHECK_EXACT(strObj)) {
		/* No match in strObj, so just use it as list[0] */
		ALIF_INCREF(strObj);
		ALIFLIST_SET_ITEM(list, 0, (AlifObject*)strObj);
		count++;
	} else
	#endif
	{
		SPLIT_ADD(str, 0, j);
	}
	FIX_PREALLOC_SIZE(list);
	if (alifList_reverse(list) < 0)
		goto onError;
	return list;

onError:
	ALIF_DECREF(list);
	return nullptr;
}




// 1101
ALIF_LOCAL_INLINE(AlifObject*)
STRINGLIB(splitChar)(AlifObject* strObj,
	const STRINGLIB_CHAR* str, AlifSizeT strLen,
	const STRINGLIB_CHAR ch,
	AlifSizeT maxcount) {
	AlifSizeT i{}, j{}, count = 0;
	AlifObject* list = alifList_new(PREALLOC_SIZE(maxcount));
	AlifObject* sub;

	if (list == nullptr)
		return nullptr;

	i = j = 0;
	while ((j < strLen) and (maxcount-- > 0)) {
		for (; j < strLen; j++) {
			/* I found that using memchr makes no difference */
			if (str[j] == ch) {
				SPLIT_ADD(str, i, j);
				i = j = j + 1;
				break;
			}
		}
	}
#if !STRINGLIB_MUTABLE
	if (count == 0 && STRINGLIB_CHECK_EXACT(strObj)) {
		/* ch not in strObj, so just use strObj as list[0] */
		ALIF_INCREF(strObj);
		ALIFLIST_SET_ITEM(list, 0, (AlifObject*)strObj);
		count++;
	}
	else
	#endif
		if (i <= strLen) {
			SPLIT_ADD(str, i, strLen);
		}
	FIX_PREALLOC_SIZE(list);
	return list;

onError:
	ALIF_DECREF(list);
	return nullptr;
}



// 144
ALIF_LOCAL_INLINE(AlifObject*) STRINGLIB(split)(AlifObject* str_obj,
	const STRINGLIB_CHAR* str, AlifSizeT str_len,
	const STRINGLIB_CHAR* sep, AlifSizeT sep_len,
	AlifSizeT maxcount) {
	AlifSizeT i{}, j{}, pos{}, count = 0;
	AlifObject* list{}, * sub{};

	if (sep_len == 0) {
		alifErr_setString(_alifExcValueError_, "لا يوجد فاصل");
		return nullptr;
	}
	else if (sep_len == 1)
		return STRINGLIB(splitChar)(str_obj, str, str_len, sep[0], maxcount);

	list = alifList_new(PREALLOC_SIZE(maxcount));
	if (list == nullptr)
		return nullptr;

	i = j = 0;
	while (maxcount-- > 0) {
		pos = FASTSEARCH(str + i, str_len - i, sep, sep_len, -1, FAST_SEARCH);
		if (pos < 0)
			break;
		j = i + pos;
		SPLIT_ADD(str, i, j);
		i = j + sep_len;
	}
#if !STRINGLIB_MUTABLE
	if (count == 0 and STRINGLIB_CHECK_EXACT(str_obj)) {
		/* No match in strObj, so just use it as list[0] */
		ALIF_INCREF(str_obj);
		ALIFLIST_SET_ITEM(list, 0, (AlifObject*)str_obj);
		count++;
	}
	else
	#endif
	{
		SPLIT_ADD(str, i, str_len);
	}
	FIX_PREALLOC_SIZE(list);
	return list;

onError:
	ALIF_DECREF(list);
	return nullptr;
}
