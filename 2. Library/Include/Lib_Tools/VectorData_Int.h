#pragma once
#include <vector>
using namespace std;

class AFX_EXT_CLASS CVectorData_Int
{
public:
	CVectorData_Int(void);
	~CVectorData_Int(void);

	BOOL Save(CFile *fp);
	BOOL Read(CFile *fp);

	void SetData( CString sName );
	void ResetData();
	int GetDataNum();
	void AddData( int nData );
	int GetData( int nIndex );
	CString GetDataName();

protected :
	char m_pBuffer[256];
	CString m_sName;
	int m_nElementNum;
	vector<int>m_nData;
};
