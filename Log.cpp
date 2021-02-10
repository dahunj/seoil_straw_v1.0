#include "StdAfx.h"
#include "Log.h"

Log::Log(void)
{
}

Log::~Log(void)
{
}

void Log::SaveLog(void)
{
	FILE *f;
	m_time.GetTime(); 
	f=fopen("c:\\Log\\"+m_time.GetDate()+".txt","a"); 
	fprintf(f,"%s\n",m_time.GetTime()); 
	fclose(f); 
}
