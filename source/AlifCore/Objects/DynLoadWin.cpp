#include "alif.h"
#include "AlifCore_FileUtils.h"
#include "AlifCore_ImportDL.h"

#include <windows.h>















const char* _alifImportDynLoadFiletab_[] = { // 12
	//ALIFD_TAGGED_SUFFIX,
	ALIFD_UNTAGGED_SUFFIX,
	nullptr
};






















DLFuncPtr _alifImport_findSharedFuncptrWindows(const char* _prefix,
	const char* _shortname, AlifObject* _pathname, FILE* _fp) { // 203
	DLFuncPtr p{};
	char funcname[258]{}, * import_python{};

#ifdef ALIF_ENABLE_SHARED
	_alif_checkPython3();
#endif 

	wchar_t* wpathname = alifUStr_asWideCharString(_pathname, nullptr);
	if (wpathname == nullptr)
		return nullptr;

	alifOS_snprintf(funcname, sizeof(funcname), "%.20s_%.200s", _prefix, _shortname);

	{
		HINSTANCE hDLL = nullptr;
#ifdef MS_WINDOWS_DESKTOP
		AlifUIntT old_mode{};

		/* Don't display a message box when Python can't load a DLL */
		old_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

		ALIF_BEGIN_ALLOW_THREADS
			hDLL = LoadLibraryExW(wpathname, NULL,
				LOAD_LIBRARY_SEARCH_DEFAULT_DIRS |
				LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
		ALIF_END_ALLOW_THREADS
			alifMem_dataFree(wpathname);

#ifdef MS_WINDOWS_DESKTOP
		/* restore old error mode settings */
		SetErrorMode(old_mode);
#endif

		if (hDLL == nullptr) {
			AlifObject* message{};
			AlifUIntT errorCode{};

			/* Get an error string from Win32 error code */
			wchar_t theInfo[256]{}; /* Pointer to error text
								  from system */
			int theLength; /* Length of error text */

			errorCode = GetLastError();

			theLength = FormatMessageW(
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS, /* flags */
				nullptr, /* message source */
				errorCode, /* the message (error) ID */
				MAKELANGID(LANG_NEUTRAL,
					SUBLANG_DEFAULT),
				/* Default language */
				theInfo, /* the buffer */
				sizeof(theInfo) / sizeof(wchar_t), /* size in wchars */
				nullptr); /* no additional format args. */

			/* Problem: could not get the error message.
			   This should not happen if called correctly. */
			if (theLength == 0) {
				message = alifUStr_fromFormat(
					"فشل تحميل DLL مع إرجاع رمز الخطأ %u بينما يتم استيراد %s",
					errorCode, _shortname);
			}
			else {
				/* For some reason a \r\n
				   is appended to the text */
				if (theLength >= 2 and
					theInfo[theLength - 2] == '\r' and
					theInfo[theLength - 1] == '\n') {
					theLength -= 2;
					theInfo[theLength] = '\0';
				}
				message = alifUStr_fromFormat(
					"فشل تحميل DLL بينما يتم استيراد %s: ", _shortname);

				alifUStr_appendAndDel(&message,
					alifUStr_fromWideChar(
						theInfo,
						theLength));
			}
			if (message != nullptr) {
				AlifObject* shortname_obj = alifUStr_fromString(_shortname);
				//alifErr_setImportError(message, shortname_obj, _pathname);
				ALIF_XDECREF(shortname_obj);
				ALIF_DECREF(message);
			}
			return nullptr;
		}
		else {
			char buffer[256]{};

			alifOS_snprintf(buffer, sizeof(buffer),
#ifdef _DEBUG
				"alif%d%d_d.dll",
#else
				"alif%d%d.dll",
#endif
				ALIF_MAJOR_VERSION, ALIF_MINOR_VERSION);
			//import_python = get_alifImport(hDLL);
			printf("حالة غير مكتملة هنا\n");
			//if (import_python and
			//	_stricmp(buffer, import_python)) {
			//	alifErr_format(_alifExcImportError_,
			//		"تضارب الوحدة %.150s "
			//		"مع هذه النسخة من ألف.",
			//		import_python);
			//	ALIF_BEGIN_ALLOW_THREADS
			//		FreeLibrary(hDLL);
			//	ALIF_END_ALLOW_THREADS
			//		return nullptr;
			//}
		}
		ALIF_BEGIN_ALLOW_THREADS
		p = GetProcAddress(hDLL, funcname);
		ALIF_END_ALLOW_THREADS
	}

	return p;
}
