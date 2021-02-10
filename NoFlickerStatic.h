#pragma once


class CNoFlickerStatic :
	public CStatic
{
	DECLARE_DYNAMIC(CNoFlickerStatic)

public:
	CNoFlickerStatic(void);
	~CNoFlickerStatic(void);

protected:
	DECLARE_MESSAGE_MAP()

private:
	CString strText;
	CFont m_Font;

public:
	afx_msg void OnPaint();
	void SetText(const CString strText);
};
