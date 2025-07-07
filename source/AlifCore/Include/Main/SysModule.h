#pragma once

AlifObject* alifSys_getObject(const char*); // 7

AlifIntT alifSys_audit(const char*, const char* , ...); // 25


/*----------------------------------------------------------------------------------------*/

typedef AlifIntT(*AlifAuditHookFunction)(const char*, AlifObject*, void*); // 5
