#pragma once

#include "AlifCore_Interpreter.h"
#include "AlifCore_Parser.h"
#include "AlifCore_Thread.h"
#include "AlifCore_Import.h"
#include "AlifCore_UStrObject.h"


struct GILStateDureRunState { // 34
public:
	AlifIntT checkEnabled{};
	AlifInterpreter* autoInterpreterState{};
};




class RefTracerDureRunState { // 198
public:
	AlifRefTracer tracerFunc{};
	void* tracerData{};
};

class AlifDureRun { // 159
public:
	AlifIntT selfInitialized{};

	AlifIntT coreInitialized{};

	AlifIntT initialized{};

	AlifThread* finalizing_{};

	unsigned long finalizingID{};

	class AlifInterpreters {
	public:
		AlifInterpreter* head{};
		AlifInterpreter* main{};
		AlifIntT nextID{};
	} interpreters;

	AlifIntT mainThreadID{};
	AlifThread* mainThread{};

	AlifThreadDureRunState threads{};

	AlifTssT autoTSSKey{};
	AlifTssT trashTSSKey{};

	AlifWStringList origArgv{};

	AlifParserDureRunState parser{};
	ImportDureRunState imports{};
	EvalDureRunState eval{};
	GILStateDureRunState gilState{};
	RefTracerDureRunState refTracer{};

	AlifRWMutex stopTheWorldMutex{};
	StopTheWorldState stopTheWorld{};

	AlifStaticObjects staticObjects{};

	AlifInterpreter mainInterpreter{};
};

extern AlifDureRun _alifDureRun_; // 318

extern AlifIntT alifDureRunState_init(AlifDureRun*); // 320
extern void alifDureRunState_fini(AlifDureRun*); // 321


extern AlifIntT alifDureRun_initialize(); // 329






static inline AlifThread* alifDureRunState_getFinalizing(AlifDureRun* _dureRun) { // 383
	return (AlifThread*)alifAtomic_loadPtrRelaxed(&_dureRun->finalizing_);
}

static inline unsigned long alifDureRunState_getFinalizingID(AlifDureRun* _dureRun) { // 388
	return ALIFATOMIC_LOAD_ULONG_RELAXED(&_dureRun->finalizingID);
}
