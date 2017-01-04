#pragma once


// CRenameCategory �Ի���

class CRenameCategory : public CDialogEx
{
	DECLARE_DYNAMIC(CRenameCategory)

public:
	CRenameCategory(CWnd* pParent = NULL);   // ��׼���캯��
    CRenameCategory(BOOL isUrl, CWnd* pParent = NULL);
	virtual ~CRenameCategory();

// �Ի�������
	enum { IDD = IDD_MEDIA_RENAME_CATEGORYLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
public:
    virtual BOOL InitMedia(CString mediaName, CString urldesc = _T(""));


    CString m_OrigName;
    CString m_NewUrlName;
    CString m_header;
    CString m_NewMediaName;
    CComboBox m_urlType;
    BOOL        m_isUrl;
};
