#pragma once


class GroupGenerator {
public:
	const char* grouping{};
	char previous{};
	AlifSizeT i{}; /* Where we're currently pointing in grouping. */
};


static void groupGenerator_init(GroupGenerator* _self, const char* _grouping) {
	_self->grouping = _grouping;
	_self->i = 0;
	_self->previous = 0;
}


/* Returns the next grouping, or 0 to signify end. */
static AlifSizeT groupGenerator_next(GroupGenerator* _self) {
	/* Note that we don't really do much error checking here. If a
	grouping string contains just CHAR_MAX, for example, then just
	terminate the generator. That shouldn't happen, but at least we
	fail gracefully. */
	switch (_self->grouping[_self->i]) {
	case 0:
		return _self->previous;
	case CHAR_MAX:
		/* Stop the generator. */
		return 0;
	default: {
		char ch = _self->grouping[_self->i];
		_self->previous = ch;
		_self->i++;
		return (AlifSizeT)ch;
	}
	}
}


/* Fill in some digits, leading zeros, and thousands separator. All
are optional, depending on when we're called. */
static void insertThousandsGrouping_fill(AlifUStrWriter* _writer,
	AlifSizeT* _bufferPos, AlifObject* _digits, AlifSizeT* _digitsPos,
	AlifSizeT _nChars, AlifSizeT _nZeros,
	AlifObject* _thousandsSep, AlifSizeT _thousandsSepLen,
	AlifUCS4* _maxchar) {
	if (!_writer) {
		/* if maxchar > 127, maxchar is already set */
		if (*_maxchar == 127 and _thousandsSep) {
			AlifUCS4 maxchar2 = ALIFUSTR_MAX_CHAR_VALUE(_thousandsSep);
			*_maxchar = ALIF_MAX(*_maxchar, maxchar2);
		}
		return;
	}

	if (_thousandsSep) {
		*_bufferPos -= _thousandsSepLen;

		/* Copy the thousandsSep chars into the buffer. */
		_alifUStr_fastCopyCharacters(_writer->buffer, *_bufferPos,
			_thousandsSep, 0,
			_thousandsSepLen);
	}

	*_bufferPos -= _nChars;
	*_digitsPos -= _nChars;
	_alifUStr_fastCopyCharacters(_writer->buffer, *_bufferPos,
		_digits, *_digitsPos,
		_nChars);

	if (_nZeros) {
		*_bufferPos -= _nZeros;
		AlifIntT kind = ALIFUSTR_KIND(_writer->buffer);
		void *data = ALIFUSTR_DATA(_writer->buffer);
		uStr_fill(kind, data, '0', *_bufferPos, _nZeros);
	}
}
