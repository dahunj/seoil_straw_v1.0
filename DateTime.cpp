#include "StdAfx.h"
#include "DateTime.h"

DateTime::DateTime(void)
{
}

DateTime::~DateTime(void)
{
}

void DateTime::Check(void)
{
	m_time=CTime::GetCurrentTime();
}

CString DateTime::GetDateTime(void)
{
	CString str;
	str.Format("%04d-%02d-%02d %02d:%02d:%02d",m_time.GetYear(),m_time.GetMonth(),m_time.GetDay(),m_time.GetHour(),m_time.GetMinute(),m_time.GetSecond()); 
	return str;
}

CString DateTime::GetDate(void)
{
	CString str;
	str.Format("%04d-%02d-%02d",m_time.GetYear(),m_time.GetMonth(),m_time.GetDay()); 
	return str;
}

CString DateTime::GetTime(void)
{
	CString str;
	str.Format("%02d:%02d:%02d",m_time.GetHour(),m_time.GetMinute(),m_time.GetSecond()); 
	return str;
}
