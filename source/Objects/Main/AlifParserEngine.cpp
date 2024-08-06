#include "alif.h"

//#include "AlifCore_AST.h"
//#include "AlifCore_AlifTokenState.h"
#include "ErrorCode.h"

#include "AlifLexer.h"
#include "Tokenizer.h"
#include "AlifParserEngine.h"
//#include "AlifCore_Memory.h"

typedef AlifToken AlifToken; // temp

AlifIntT alifParserEngine_insertMemo(AlifParser* _p, AlifIntT _mark, AlifIntT _type, void* _node) { 
	Memo* m = (Memo*)alifASTMem_malloc(_p->astMem, sizeof(Memo));
	if (m == nullptr) return -1;
	m->type = _type;
	m->node = _node;
	m->mark_ = _p->mark_;
	m->next = _p->tokens[_mark]->memo;
	_p->tokens[_mark]->memo = m;
	return 0;
}

AlifIntT alifParserEngine_updateMemo(AlifParser* _p, AlifIntT _mark, AlifIntT _type, void* _node) { 
	for (Memo* m = _p->tokens[_mark]->memo; m != nullptr; m = m->next) {
		if (m->type == _type) {
			m->node = _node;
			m->mark_ = _p->mark_;
			return 0;
		}
	}
	return alifParserEngine_insertMemo(_p, _mark, _type, _node);
}

static AlifIntT get_keywordOrName(AlifParser* _p, AlifToken* _token) { 
	AlifIntT nameLen = _token->endColOffset - _token->colOffset;

	if (nameLen >= _p->nKeywordList or _p->keywords[nameLen] == nullptr
		or _p->keywords[nameLen]->type == -1) return NAME;
	for (KeywordToken* k = _p->keywords[nameLen]; k != nullptr and k->type != -1; k++) {
		if (strncmp(k->str, _token->start, nameLen) == 0) {
			return k->type;
		}
	}

	return NAME;
}


static AlifIntT initialize_token(AlifParser* _p, AlifPToken* _pToken, AlifToken* _token, AlifIntT _tokType) { 
	_pToken->type = (_tokType == NAME) ? get_keywordOrName(_p, _token) : _tokType;
	_pToken->bytes = alifBytes_fromStringAndSize(_token->start, (_token->end - _token->start));

	if (_pToken->bytes == nullptr) {
		return -1;
	}
	if (alifASTMem_listAddAlifObj(_p->astMem, _pToken->bytes) < 0) {
		ALIF_DECREF(_pToken->bytes);
		return -1;
	}

	_pToken->data = nullptr;
	if (_token->data != nullptr) {
		if (alifASTMem_listAddAlifObj(_p->astMem, _token->data) < 0) {
			ALIF_DECREF(_pToken->data);
			return -1;
		}
	}

	_pToken->level = _token->level;
	_pToken->lineNo = _token->lineNo;
	_pToken->colOffset = _p->tok->lineNo == _p->startingLineNo ? _p->startingColOffset + _token->colOffset : _token->colOffset;

	_pToken->endLineNo = _token->endLineNo;
	_pToken->endColOffset = _p->tok->lineNo == _p->startingLineNo ? _p->startingColOffset + _token->endColOffset : _token->endColOffset;

	_p->fill_ += 1;

	if (_tokType == ERRORTOKEN) {
		// error
	}

	//return (_tokType == ERRORTOKEN ? alifParserEngine_tokenizerError(_p) : 0);
	return 0;//
}

static AlifIntT resize_tokensArr(AlifParser* _p) { 
	AlifIntT newSize = _p->size_ * 2;
	AlifPToken** newTokens = (AlifPToken**)alifMem_dataRealloc(_p->tokens, newSize * sizeof(AlifPToken*)); // error when using realloc, need to review

	_p->tokens = newTokens;
	for (AlifIntT i = _p->size_; i < newSize; i++) {
		_p->tokens[i] = (AlifPToken*)calloc(1, sizeof(AlifPToken));
	}

	_p->size_ = newSize;
	return 0;
}

AlifIntT alifParserEngine_fillToken(AlifParser* _p) { 
	AlifToken newToken{};
	AlifIntT type = alifTokenizer_get(_p->tok, &newToken);

	AlifPToken* pT{};
	//while (type == TYPEIGNORE) {
	//	AlifSizeT len = newToken.endColOffset - newToken.colOffset;
	//	char* tag = (char*)malloc(len + 1);
	//	wcsncpy(tag, newToken.start, len);
	//	tag[len] = '0';
	//	if (!growableCommentArr_add(&_p->typeIgnoreComments, _p->tok->lineNo, tag)) {
	//		//goto error;
	//	}
	//	type = alifTokenizer_get(_p->tok, &newToken);
	//}

	if (_p->startRule == ALIFSINGLE_INPUT and type == ENDMARKER and _p->parsingStarted) {
		type = NEWLINE;
		_p->parsingStarted = 0;
		if (_p->tok->indent) {
			_p->tok->pendInd = -_p->tok->indent;
			_p->tok->indent = 0;
		}
		else {
			_p->parsingStarted = 1;
		}
	}

	if ((_p->fill_ == _p->size_) and (resize_tokensArr(_p)) != 0) {
		goto error;
	}

	pT = _p->tokens[_p->fill_];
	return initialize_token(_p, pT, &newToken, type);

error:
	//alifToken_free(&newToken);
	return -1;
}

AlifIntT alifParserEngine_isMemorized(AlifParser* _p, AlifIntT _type, void* _pres) { 
	if (_p->mark_ == _p->fill_) {
		if (alifParserEngine_fillToken(_p) < 0) {
			_p->errorIndicator = 1;
			return -1;
		}
	}

	AlifPToken* t_ = _p->tokens[_p->mark_];

	for (Memo* m_ = t_->memo; m_ != nullptr; m_ = m_->next) {
		if (m_->type == _type) {
			_p->mark_ = m_->mark_;
			*(void**)_pres = m_->node;
			return 1;
		}
	}
	return 0;
}

AlifIntT alifParserEngine_lookaheadWithInt(AlifIntT _positive, AlifPToken* (_func)(AlifParser*, AlifIntT), AlifParser* _p, AlifIntT _arg) { 
	AlifIntT mark_ = _p->mark_;
	void* res = _func(_p, _arg);
	_p->mark_ = mark_;
	return (res != nullptr) == _positive;
}

AlifIntT alifParserEngine_lookahead(AlifIntT _positive, void* (_func)(AlifParser*), AlifParser* _p) { 
	AlifIntT mark_ = _p->mark_;
	void* res = _func(_p);
	_p->mark_ = mark_;
	return (res != nullptr) == _positive;
}

AlifPToken* alifParserEngine_expectToken(AlifParser* _p, AlifIntT _type) {
	/*
		إذا وصل المؤشر mark
		الى مؤشر الملء fill
		هذا يعني أنه لم يعد هنالك رموز المأخوذة tokens
		قابلة للإستخدام وبالتالي يجب جلب رمز جديد
		-------------------------------------------
		الفرق بين المؤشر الحالي ومؤشر الملء يدل على عدد الرموز المأخوذة tokens
	*/
	if (_p->mark_ == _p->fill_) {
		if (alifParserEngine_fillToken(_p) < 0) {
			_p->errorIndicator = 1;
			return nullptr;
		}
	}
	AlifPToken* t = _p->tokens[_p->mark_];
	if (t->type != _type) {
		return nullptr;
	}
	_p->mark_ += 1;
	return t;
}

AlifPToken* alifParserEngine_expectTokenForced(AlifParser* _p, AlifIntT _type, const char* _expected) { 
	if (_p->errorIndicator == 1) return nullptr;

	if (_p->mark_ == _p->fill_) {
		if (alifParserEngine_fillToken(_p) < 0) {
			_p->errorIndicator = 1;
			return nullptr;
		}
	}

	AlifPToken* t_ = _p->tokens[_p->mark_];
	if (t_->type != _type) {
		// error
		return nullptr;
	}
	_p->mark_ += 1;
	return t_;
}

AlifPToken* alifParserEngine_getLastNonWhitespaceToken(AlifParser* _p) { 
	AlifPToken* token = nullptr;
	for (AlifIntT i = _p->mark_ - 1; i >= 0; i--) {
		token = _p->tokens[i];
		if (token->type != ENDMARKER and (token->type < NEWLINE or token->type > DEDENT)) {
			break;
		}
	}

	return token;
}

AlifObject* alifParserEngine_newIdentifier(AlifParser* _p, const char* _s) { 
	AlifObject* id = alifUStr_decodeStringToUTF8(_s);

	if (alifASTMem_listAddAlifObj(_p->astMem, id) < 0) {
		// error
	}
	return id;
}

static Expression* alifParserEngine_nameFromToken(AlifParser* _p, AlifPToken* _t) { 
	if (_t == nullptr) {
		return nullptr;
	}

	const char* s = _alifWBytes_asString(_t->bytes);

	if (!s) {
		_p->errorIndicator = 1;
		return nullptr;
	}
	AlifObject* id = alifParserEngine_newIdentifier(_p, s); // need work on it

	return alifAST_name(id, Load, _t->lineNo, _t->colOffset, _t->endLineNo, _t->endColOffset, _p->astMem);
}

Expression* alifParserEngine_nameToken(AlifParser* _p) { 
	AlifPToken* tok = alifParserEngine_expectToken(_p, NAME);
	return alifParserEngine_nameFromToken(_p, tok);
}

void* alifParserEngine_stringToken(AlifParser* _p) { 
	return alifParserEngine_expectToken(_p, STRING);
}

static AlifObject* parseNumber_raw(const char* _s) { 
	const char* end{};
	AlifSizeT x{};
	double dx{};
	//AlifIntT imflag{};

	errno = 0;
	end = _s + strlen(_s) - 1;
	if (_s[0] == '0') {

	}
	else {
		x = alifOS_strToLong(_s); // need edit
		end++; // temp
	}
	if (*end == '\0') {
		if (errno != 0) {

		}
		return alifInteger_fromLongLong(x);
	}

	return nullptr; //
}

static AlifObject* parse_number(const char* _s) { 
	char* dup{};
	char* end{};
	AlifObject* res{};

	if (strstr(_s, "_") == nullptr) {
		return parseNumber_raw(_s);
	}

	dup = (char*)alifMem_dataAlloc(strlen(_s));
	end = dup;
	for (; *_s; _s++) {
		if (*_s != '_') {
			*end++ = *_s;
		}
	}
	*end = '\0';
	res = parseNumber_raw(dup);
	free(dup);
	return res;
}

Expression* alifParserEngine_numberToken(AlifParser* _p) { 
	AlifPToken* tok = alifParserEngine_expectToken(_p, NUMBER);
	if (tok == nullptr) return nullptr;

	const char* rawNum = _alifWBytes_asString(tok->bytes);

	AlifObject* num = parse_number(rawNum);

	if (alifASTMem_listAddAlifObj(_p->astMem, num) < 0) {
		// error
		return nullptr;
	}

	return alifAST_constant(num, nullptr, tok->lineNo, tok->colOffset, tok->endLineNo, tok->endColOffset, _p->astMem); // type should be NUMBER not nullptr!
}


AlifParser* alifParserEngine_newParser(TokenInfo* _tokInfo, AlifIntT _startRule, AlifASTMem* _astMem)
{ 
	AlifParser* p = (AlifParser*)alifMem_dataAlloc(sizeof(AlifParser));
	if (p == nullptr) {
		// error
		return nullptr;
	}

	p->tok = _tokInfo;
	p->keywords = nullptr;
	p->nKeywordList = -1;
	p->softKeyword = nullptr;
	p->tokens = (AlifPToken**)alifMem_dataAlloc(sizeof(AlifPToken*));
	if (p->tokens == nullptr) {
		// error
		return nullptr;
	}
	p->tokens[0] = (AlifPToken*)alifMem_dataAlloc(sizeof(AlifPToken));
	if (p->tokens[0] == nullptr) {
		// error
		return nullptr;
	}

	p->mark_ = 0;
	p->fill_ = 0;
	p->size_ = 1;

	p->astMem = _astMem;
	p->startRule = _startRule;
	p->parsingStarted = 0;
	p->normalize = nullptr;
	p->errorIndicator = 0;
	p->startingLineNo = 0;
	p->startingColOffset = 0;
	p->level = 0;

	return p;
}


void alifParserEngine_parserFree(AlifParser* _p) { 
	ALIF_XDECREF(_p->normalize);
	for (AlifIntT i = 0; i < _p->size_; i++) {
		alifMem_dataFree(_p->tokens[i]);
	}
	alifMem_dataFree(_p->tokens);
	//growableCommentArr_dealloc(&_p->typeIgnoreComments);
	alifMem_dataFree(_p);
}


void* alifParserEngine_runParser(AlifParser* _p) { 

	void* res = alifParserEngine_parse(_p);

	if (res == nullptr) {
		AlifPToken* lastToken = _p->tokens[_p->fill_ - 1];
		//alifParserEngine_setSyntaxError(_p, lastToken);

		return nullptr;
	}

	return res;
}

Module* alifParser_astFromFile(FILE* _fp, AlifObject* _fn, AlifIntT _startRule, AlifASTMem* _astMem) { // _alifPegen_run_parser_from_file_pointer() 

	TokenInfo* tokInfo = alifTokenizerInfo_fromFile(_fp);
	if (tokInfo == nullptr) {
		// error
		return nullptr;
	}

	tokInfo->fn = ALIF_NEWREF(_fn);

	Module* result{};

	AlifParser* p = alifParserEngine_newParser(tokInfo, _startRule, _astMem);
	if (p == nullptr) goto end;

	result = (Module*)alifParserEngine_runParser(p);
	//alifParserEngine_parserFree(p); // يحتاج إختبار وتصحيح


end:
	//alifTokenizer_Free(tokInfo);
	return result;
}
