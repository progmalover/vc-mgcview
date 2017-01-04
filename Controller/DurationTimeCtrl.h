#pragma once


// CDurationTimeCtrl
#include <vector>
using namespace std;
#define HH_SECTION 0 // hour
#define MM_SECTION 1 // minute
#define SS_SECTION 2 // second
class CDurationTimeCtrl : public CWnd
{
	friend class CKeyBoardInputTrack;

	DECLARE_DYNAMIC(CDurationTimeCtrl)
	typedef struct DateSecStr{
		int startLocal;
		int endLocal;
		int startx;
		int endx;
		CString strSec;
		void operator=(const struct DateSecStr &_In)
		{
			startLocal = _In.startLocal;
			endLocal = _In.endLocal;
			startx = _In.startx;
			endx = _In.endx;
			strSec = _In.strSec;
		};
		DateSecStr(const struct DateSecStr &_In)
		{
			*this = _In;
		};
		DateSecStr()
		{
			 startLocal = 0;
			 endLocal = 0;
			 startx = 0 ;
			 endx = 0;
		};
	}DATE_SEC_STR;
	typedef vector<DATE_SEC_STR> DATE_SECS;

	// keyboard input tracker.
	class CKeyBoardInputTrack
	{
		public:
			CKeyBoardInputTrack(){};
			~CKeyBoardInputTrack(){};
		protected :
			//CEdit   m_InputEdit;
			CString m_OldString;
			bool	Filter(int &key);
		public:
			virtual bool Track(int key, CWnd *pOwner);
	};

	struct AFX_GLOBAL_DATA_PRIVATE :public AFX_GLOBAL_DATA
	{
		friend class CDurationTimeCtrl;
	};

	class CMFCVisualManagerOffice2007Private :public CMFCVisualManagerOffice2007
	{
		friend class CDurationTimeCtrl;
	};
public:
	CDurationTimeCtrl();
	virtual ~CDurationTimeCtrl();
	bool CreateFromStatic(CWnd *pOwner,int nID);
	void SetDurationTime( int h,int m,int s);
	void GetDurationTime( int &h,int &m,int &s);
	void PostDateTimeChangeNCMessage();
	 
private:
	CSpinButtonCtrl		m_SpinBtn;
	CString				m_DurationString;
	DATE_SECS			m_DateSecs;
	CRect				m_DrawArea;
	int					m_iFocusSec;
	CKeyBoardInputTrack  m_InputTracker;
	BOOL				m_bEnughSpace;
//	static CString			m_WndClass;
private :
	bool GetFoucusDateField(CString &str);
	bool SetFoucusDateField(CString str);
	bool GetDateFieldVal(int &val,int iSec);
	bool SetDateFieldVal(int val,int iSec);
	bool GetDateField(CString &str,int iSec);
	bool SetDateField(CString str ,int iSec);
	bool PtToDateSec(CPoint pt);
	void GetDrawArea(CRect &area);
	bool DecDateSec(int iSec);
	bool IncDateSec(int iSec);
	bool CanDec(int iSec);
	bool CanInc(int iSec);
	void ReBuildDataSecs(void);
	bool GetDateFieldRect(int iSec,CRect &rect);
	bool OnKeyDownSetFocusDateSec(CString str);
protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetTime(COleDateTimeSpan oleDate);
	void GetTime(COleDateTimeSpan &oleDate);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


 extern void AFXAPI DDX_DurationDateTimeCtrl(CDataExchange* pDX, int nIDC, COleDateTimeSpan& value);