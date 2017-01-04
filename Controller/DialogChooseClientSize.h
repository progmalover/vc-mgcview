#pragma once

#define PI 3.1415926535897932384626433832795
#define PI_2 1.5707963267948966192313216916398
#define PI_3 1.0471975511965977461542144610932

//  CDialogChooseClientSize 对话框
class CClientDesc :public CRect
{
public:
    CClientDesc(){
        m_clientID = -1;
        m_fRotateDegree = 0.0;
    };
    ~CClientDesc(){};
public:
  int m_clientID;
  std::vector<CRect>  m_MonitorRects; 
  FLOAT               m_fRotateDegree;
};

class CDialogChooseClientSize : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogChooseClientSize)

public:
	CDialogChooseClientSize(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogChooseClientSize();

// 对话框数据
	enum { IDD = IDD_SELECT_CLIENT_SIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void OnOK();
public:
    CListCtrl m_wndClientSize;
    virtual BOOL OnInitDialog();
    std::vector<CClientDesc> m_Clients;
    CClientDesc m_rectSelectedLayout;
};
