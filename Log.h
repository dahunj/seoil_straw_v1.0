#pragma once
#include "afx.h"
#include "DateTime.h"

class Log : public CObject
{
public:
	DateTime m_time; 
	Log(void);
	~Log(void);
	void SaveLog(void);
};
