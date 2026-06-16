#pragma once


AlifObject* alifFile_getLine(AlifObject*, AlifIntT); // 14
AlifIntT alifFile_writeObject(AlifObject*, AlifObject*, AlifIntT); // 15
AlifIntT alifFile_writeString(const char*, AlifObject*); // 16
AlifIntT alifObject_asFileDescriptor(AlifObject*); // 17







extern AlifIntT _alifUTF8Mode_;



/* ------------------------------------------------------------------------------------------------ */


AlifObject* alifFile_newStdPrinter(int);
extern AlifTypeObject _alifStdPrinterType_;

typedef AlifObject* (*AlifOpenCodeHookFunction)(AlifObject*, void*);
