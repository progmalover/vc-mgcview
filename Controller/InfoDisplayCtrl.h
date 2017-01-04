#pragma once


class InfoDisplayItem
{
public:
    CString InfoName;
    CString InfoValue;
};
// CInfoDisplayCtrl

class CInfoDisplayCtrl : public CStatic
{
	DECLARE_DYNAMIC(CInfoDisplayCtrl)

public:
	CInfoDisplayCtrl();
	virtual ~CInfoDisplayCtrl();
    void ClearInfo();
    void AddInfo(InfoDisplayItem &DisplayItem);
    void AddAuditInfo(Audit audit);

protected:
	DECLARE_MESSAGE_MAP()

    CArray<InfoDisplayItem, InfoDisplayItem&> m_Infos;
public:
    afx_msg void OnPaint();
};


