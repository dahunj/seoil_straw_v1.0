#pragma once

#include "hasp_api.h"

class AFX_EXT_CLASS CVisionWorks2_Dll
{
public:
	CVisionWorks2_Dll(void);
	~CVisionWorks2_Dll(void);

	static BOOL CheckLock();
	static CString GetAdminUser();
	static hasp_status_t _CheckLockKey();

};
