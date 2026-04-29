#pragma once


static inline AlifIntT _config_dictGet(AlifObject* _dict,
	const char* _name, AlifObject** _pItem) {
	AlifObject* item{};
	if (alifDict_getItemStringRef(_dict, _name, &item) < 0) {
		return -1;
	}
	if (item == nullptr) {
		return -1;
	}
	*_pItem = item;
	return 0;
}

static AlifObject* config_dictGet(AlifObject* _dict, const char* _name) {
	AlifObject* item{};
	if (_config_dictGet(_dict, _name, &item) < 0) {
		if (!alifErr_occurred()) {
			alifErr_format(_alifExcValueError_, "مفتاح التكوين مفقود: %s", _name);
		}
		return nullptr;
	}
	return item;
}

static void config_dictInvalidType(const char* _name) {
	alifErr_format(_alifExcTypeError_, "نوع التكوين غير صالح: %s", _name);
}
