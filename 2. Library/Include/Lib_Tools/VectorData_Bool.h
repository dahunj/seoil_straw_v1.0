#pragma once

#include <vector>
using namespace std;


class AFX_EXT_CLASS CVectorData_Bool
{
public:
	CVectorData_Bool(void);
	~CVectorData_Bool(void);

	BOOL Save(CFile *fp);
	BOOL Read(CFile *fp);

	void SetData( CString sName );
	void ResetData();
	int GetDataNum();
	void AddData( bool bData );
	bool GetData( int nIndex );
	CString GetDataName();

protected :
	char m_pBuffer[256];
	CString m_sName;
	int m_nElementNum;
	vector<bool>m_bData;
};
