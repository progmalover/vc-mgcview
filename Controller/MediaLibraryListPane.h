#pragma once

#include "MediaLibraryChartPane.h"
#include <set>
#include "ListCtrlEx.h"
#include "AsyncGrid.h"

extern UINT MEDIA_UPLOAD_LIST;
extern UINT MEDIA_DOWNLOAD_LIST;
extern UINT MEDIA_SHOW_INFO;
extern UINT MEDIA_CLEAR_INFO;
extern UINT MEDIA_LIST_UPDATE;
extern UINT BCGM_GRID_BEGINDRAG;

struct SMediaFilter {
	int categoryId;
	unsigned long count;
	unsigned long maxId;
	SMediaFilter() : categoryId(-1),count(0),maxId(0)
	{

	}
	void setCount(unsigned long cnt)
	{
		count = cnt;
	}
	void setCategoryId(int cgrId)
	{
		categoryId = cgrId;
	}
	void setMaxId(int md)
	{
		maxId = md;
	}
	std::tstring ToString() const
	{
		std::tstring str;
		str = _T(" and MediaFilterId = ");
		str += StringUtility::itos(categoryId).c_str();
		return str;
	}
};


class CMediaGrid : public CAsyncGrid2
{
public:
	CMediaGrid();
	virtual ~CMediaGrid();

	virtual void GetData(int start, int end, const sort_and_filter_t& sf, std::map<int, std::vector<_variant_t> >& data);
	virtual int GetVirtualRowCount();
	virtual sort_and_filter_t BuildSort();
	void		RefreshMediaGrid();
	unsigned long GetMaxRowId();
	int		GetSelectRowMediaID();
	virtual void OnSelChanged(const CBCGPGridRange &range, BOOL bSelect);
	int	JudgeReferCount();
	void OnTimerRefresh();
	void	handle_timeout(const boost::system::error_code& ec, boost::shared_ptr<boost::asio::deadline_timer> timer);

	void SetFilter(const SMediaFilter& filter);
	SMediaFilter  GetFilter();

private:
	virtual void Refresh(int count, unsigned long maxId);
	boost::recursive_mutex	m_filterMutex;
	SMediaFilter	m_mediaFilter;
};

class CMediaLibraryListTab : public CMFCTabCtrl
{
    DECLARE_DYNAMIC(CMediaLibraryListTab)

public:
    CMediaLibraryListTab();
    virtual ~CMediaLibraryListTab();

    HRESULT GetSelectedMediaIDList(std::vector<INT> &SelectedMediaIDList);

	std::map<INT, Media> * GetMediaMap()
	{
		m_Section.Lock();
		return &m_MediaMap;
	}

	void ReleaseMediaMap(std::map<INT, Media> *)
	{
		m_Section.Unlock();
	}
//	CListCtrlEx*       m_wndMainList;
	std::vector<CListCtrlEx*>  m_MainList;
	std::map<CWnd*, int>        m_wndCharts;
    std::set<int>               m_setMediaLibraryChartTabIds;
    std::map<INT, Media>        m_MediaMap;
    std::map<INT, Media>        m_SearchMediaMap;

	//add by sophia
	CMediaGrid					m_mediaGrid;

	LRESULT	OnBCGBeginDrag(WPARAM wp, LPARAM lp);
//	void	handle_timeout(const boost::system::error_code& ec);

    virtual BOOL RemoveTab(int iTab, BOOL bRecalcLayout = TRUE);

    CString FormatSizeString(DWORD64 Size);


    enum{
        MEDIA_INDEX = 0,
        MEDIA_NAME,
        MEDIA_TYPE,
        MEDIA_SIZE,
       // MEDIA_CATEGORY,
        MEDIA_DIMENSION,
        MEDIA_DURATION,
#ifdef SERVER_EDITION
        MEDIA_SYNCSTATUS,
        MEDIA_AUDIT,
#endif
        MEDIA_REFERS,
        MEDIA_SERVR_PATH,
    };

    enum{
        MEDIA_TASK_INDEX = 0,
        MEDIA_TASK_STATS,
        MEDIA_TASK_NAME,
        MEDIA_TASK_TYPE,
        MEDIA_TASK_SIZE,
        //MEDIA_TASK_GENERATE_INFO_PERCENT,
        MEDIA_TASK_PERCENT,
        //MEDIA_TASK_VERIFY_PERCENT,
       // MEDIA_TASK_CATEGORY,
        MEDIA_TASK_DIMENSION,
        MEDIA_TASK_DURATION,
        MEDIA_TASK_AVALIABLE,
        MEDIA_TASK_SYNCSTATUS,
        MEDIA_TASK_SERVR_PATH,
    };


protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLvnMediaItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginDragMediaList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNmDblClick(NMHDR *pNMHDR, LRESULT *pResult);

	

    virtual void FireChangeActiveTab(int nNewTab);

    virtual void AddChartPane(int nMediaLibraryId);


    DECLARE_MESSAGE_MAP()


    COleDataSource          m_OleDataSource;
	CCriticalSection		m_Section;
public:
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
    BOOL         SerializeTo (CFile& file, const std::vector<INT>& MediaIDList, const std::map<INT, Media>* mediaMap);
};

// CMediaLibraryListPane

class CToolBarEx : public CToolBar
{
    DECLARE_DYNAMIC(CToolBarEx) 
public:       
    CToolBarEx();
    virtual ~CToolBarEx();
public:
    void SetImageList(UINT nBitmapID);
	void SetDisableImageList(UINT nBitmapID);
    void SetImageSize(int nImageWidth,int nImageHeight);
    void AddBitmap(CBitmap* pbmImage, COLORREF crMask);
    void AddBitmap(CBitmap* pbmImage, CBitmap* pbmMask);
    void AddIcon(HICON hIcon);
    void SetDefaultSize();
    void AddSeparator(int nButton);
    void SetSize(int nWidth = 40,int nHeight = 40);
    void SetButton(int nButton,int nID, int nImage,LPTSTR lpText);
    void SetButton(int nButton,int nID, int nImage);
    void SetCheckBox(int nButton,int nID, int nImage,LPTSTR lpText);
    void SetButtonNumber(int n);
    void SetImage();

protected:
    CImageList img;
    CSize sizeImage;
	CToolTipCtrl   m_ToolTip;
    DECLARE_MESSAGE_MAP()
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
};


class CMediaLibraryListPane : public CBaseControllerPane
{
    DECLARE_DYNAMIC(CMediaLibraryListPane)

public:
    CMediaLibraryListPane();
    virtual ~CMediaLibraryListPane();

    CMediaLibraryListTab         m_wndStateTab;

    CToolBarEx   m_wndListToolBar; 

    void   RecalToolBarPos();

    void   InitUploadBar();
    void   InitDownloadBar();
    void   InitSearchResultBar();
    void   InitCategoryBar();

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);

    afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);


    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

    BOOL UpdateOptions();

    UINT_PTR                m_nTimer;
    CString                 m_refreshDuration;
};


