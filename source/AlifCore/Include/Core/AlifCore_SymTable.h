#pragma once


class Module; // 11

enum AlifBlockType { // 13
	Function_Block, Class_Block, Module_Block,

	Annotation_Block,

	Type_Alias_Block,

	Type_Parameters_Block,

	Type_Variable_Block,
};

enum AlifComprehensionType { // 38
	No_Comprehension = 0,
	List_Comprehension = 1,
	Dict_Comprehension = 2,
	Set_Comprehension = 3,
	Generator_Expression = 4
};


class AlifSourceLocation { // 46
public:
	AlifIntT lineno{};
	AlifIntT endLineno{};
	AlifIntT colOffset{};
	AlifIntT endColOffset{};
};


// 53
#define SRC_LOCATION_FROM_AST(_n) { \
               .lineno = (_n)->lineno, \
               .endLineno = (_n)->endLineno, \
               .colOffset = (_n)->colOffset, \
               .endColOffset = (_n)->endColOffset }


static const AlifSourceLocation _noLocation_ = { -1, -1, -1, -1 }; // 60

class AlifFutureFeatures { // 63
public:
	AlifIntT features{};
	AlifSourceLocation location{};
};

class AlifSTEntryObject; // 68

class AlifSymTable { // 70
public:
	AlifObject* fileName{};
	AlifSTEntryObject* cur{}; 
	AlifSTEntryObject* top{};
	AlifObject* blocks{};
	AlifObject* stack{};
	AlifObject* global{};
	AlifIntT nBlocks{};
	AlifObject* private_{};
	AlifFutureFeatures* future{};
	AlifIntT recursionDepth{};
	AlifIntT recursionLimit{};
};

class AlifSTEntryObject { // 89
public:
	ALIFOBJECT_HEAD;
	AlifObject* id{};
	AlifObject* symbols{};
	AlifObject* name{};
	AlifObject* varNames{};
	AlifObject* children{};
	AlifObject* directives{};
	AlifObject* mangledNames{};

	AlifBlockType type{};

	const char* scopeInfo{};

	AlifIntT nested{};
	unsigned generator : 1;   
	unsigned coroutine : 1;  
	unsigned annotationsUsed : 1;  
	AlifComprehensionType comprehension{};
	unsigned varArgs : 1;     
	unsigned varKeywords : 1;
	unsigned returnsValue : 1;
	unsigned needsClassClosure : 1;

	unsigned needsClassDict : 1; 
	unsigned compInlined : 1; 
	unsigned compIterTarget : 1; 
	unsigned canSeeClassScope : 1;

	AlifIntT compIterExpr{};
	AlifSourceLocation loc{};
	AlifSTEntryObject* annotationBlock{};
	AlifSymTable* table{};
};


extern void alifSymtable_free(AlifSymTable*); // 147

// 154
#define DEF_GLOBAL 1            
#define DEF_LOCAL 2           
#define DEF_PARAM (2<<1)       
#define DEF_NONLOCAL (2<<2)      
#define USE (2<<3)              
#define DEF_FREE_CLASS (2<<5)  
#define DEF_IMPORT (2<<6)        
#define DEF_ANNOT (2<<7)        
#define DEF_COMP_ITER (2<<8)    
#define DEF_TYPE_PARAM (2<<9)    
#define DEF_COMP_CELL (2<<10)    

#define DEF_BOUND (DEF_LOCAL | DEF_PARAM | DEF_IMPORT)

#define SCOPE_OFFSET 12
#define SCOPE_MASK (DEF_GLOBAL | DEF_LOCAL | DEF_PARAM | DEF_NONLOCAL)
#define SYMBOL_TO_SCOPE(S) (((S) >> SCOPE_OFFSET) & SCOPE_MASK)

#define LOCAL 1
#define GLOBAL_EXPLICIT 2
#define GLOBAL_IMPLICIT 3
#define FREE 4
#define CELL 5
// 181
