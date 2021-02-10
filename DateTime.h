#pragma once
#include "afx.h"

class DateTime : public CObject
{
public:
	CTime m_time; 
	DateTime(void);
	~DateTime(void);
	void Check(void);
	CString GetDateTime(void);
	CString GetDate(void);
	CString GetTime(void);
};
