#pragma once

class AlifDictValues {
public:
	size_t hash{};
	//uint8_t kind;

	AlifObject* key{};
	AlifObject* value{};

};

class AlifDictObject{
public:

	ALIFOBJECT_HEAD

	int64_t size_;
	int64_t capacity_;
	AlifDictValues* items_;
};

extern AlifInitObject typeDict;

AlifObject* alifNew_dict();
AlifDictObject* deleteItem_fromIndex(AlifDictObject*, int64_t);
AlifDictObject* dict_deleteItem(AlifDictObject*, AlifObject*);
bool dict_next(AlifObject*, int64_t*, AlifObject**, AlifObject**, size_t*);
AlifDictObject* dict_setItem(AlifDictObject* , AlifObject*, AlifObject* );
AlifDictObject* dict_ass_sub(AlifDictObject*, AlifObject*, AlifObject*);
int dict_lookupItem(AlifDictObject*, AlifObject*, size_t, AlifObject** );
AlifObject* dict_getItem(AlifObject* , AlifObject* );
bool dict_contain(AlifObject* , AlifObject* ); 
AlifDictObject* dict_resize(AlifDictObject* );
AlifDictObject* deleteItem_fromIndex(AlifDictObject* , int64_t );