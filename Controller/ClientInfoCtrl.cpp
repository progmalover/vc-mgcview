// MediaLibraryInfoCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Controller.h"
#include "ClientInfoCtrl.h"
#include "Localization/Localization.h"

// CClientInfoCtrl dialog

IMPLEMENT_DYNAMIC(CClientInfoCtrl, CDialogEx)

CClientInfoCtrl::CClientInfoCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientInfoCtrl::IDD, pParent)
{

}

CClientInfoCtrl::~CClientInfoCtrl()
{
}

void CClientInfoCtrl::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MEDIA_INFO_DESC, m_Info);
    DDX_Control(pDX, IDC_MEDIA_THUMBNAIL, m_thumbnail);
}


BEGIN_MESSAGE_MAP(CClientInfoCtrl, CDialogEx)
    ON_WM_SIZE()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CClientInfoCtrl)

  EASYSIZE(IDC_MEDIA_THUMBNAIL, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE,0)
  //EASYSIZE(IDC_STATIC_GROUP_INFO, IDC_STATIC_THUMBNAIL, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0)
  EASYSIZE(IDC_MEDIA_INFO_DESC, IDC_MEDIA_THUMBNAIL, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


// CClientInfoCtrl message handlers
BOOL CClientInfoCtrl::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    CRect ThumbnailRect;
    m_thumbnail.GetWindowRect(&ThumbnailRect);
    m_ThumbnailImage.Create(ThumbnailRect.Width(), ThumbnailRect.Height(), 32);

    GetDlgItem(IDC_BTN_REJECT_REASON)->ShowWindow(SW_HIDE);

    INIT_EASYSIZE;

	Translate(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CClientInfoCtrl::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	UPDATE_EASYSIZE;

}



BOOL CClientInfoCtrl::SetInfo(const ClientInfo& Info, CString szIP)
{

    InfoDisplayItem InfoItem;
    m_Info.ClearInfo();


    CString szInfo = szIP;

    InfoItem.InfoName = Translate(_T("Name: "));
    InfoItem.InfoValue = Info.GetClientDesc().c_str();
    m_Info.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Location: "));
    InfoItem.InfoValue = Info.GetLocation().c_str();
    m_Info.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Admin: "));
    InfoItem.InfoValue = Info.GetAdminName().c_str();
    m_Info.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Company: "));
    InfoItem.InfoValue = Info.GetCompany().c_str();
    m_Info.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Email: "));
    InfoItem.InfoValue = Info.GetEmail().c_str();
    m_Info.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("Tel: "));
    InfoItem.InfoValue = Info.GetTelPhone().c_str();
    m_Info.AddInfo(InfoItem);

    InfoItem.InfoName = Translate(_T("IP Address: "));
    InfoItem.InfoValue = szIP;
    m_Info.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Display Card Type: "));
    InfoItem.InfoValue = Info.GetDisplayCardType().c_str();
    m_Info.AddInfo(InfoItem);


    InfoItem.InfoName = Translate(_T("Software Info: "));
    InfoItem.InfoValue =  Info.GetSoftwareInfo().c_str();
    m_Info.AddInfo(InfoItem);

	CString szHardWardInfo = Info.GetHardwareInfo().c_str();
	CString separate_str = _T("Memory");
	CString cpu_str = _T("CPU:");
	CString szCPUinfo;
	CString szMemoryinfo;
	int index = 0;
	if(-1 != (index = szHardWardInfo.Find(separate_str)))
	{
		szCPUinfo = szHardWardInfo.Mid(cpu_str.GetLength(), index - cpu_str.GetLength());
		szCPUinfo.TrimLeft();
		szMemoryinfo = szHardWardInfo.Mid(index+separate_str.GetLength()+1, szHardWardInfo.GetLength()-index-separate_str.GetLength()-1) ;
		szMemoryinfo.TrimLeft();

		/*InfoItem.InfoName  = _T("CPU: ");
		InfoItem.InfoValue =  szCPUinfo;
		m_Info.AddInfo(InfoItem);

		InfoItem.InfoName   = Translate(_T("Memory: "));
		InfoItem.InfoValue  = szMemoryinfo;
		m_Info.AddInfo(InfoItem);*/

		InfoItem.InfoName = Translate(_T("Hardware Info: "));
		CString tmp = _T("CPU: ") + szCPUinfo + _T(" ") +Translate(_T("Memory: ")) + szMemoryinfo;
		InfoItem.InfoValue  = tmp;
		m_Info.AddInfo(InfoItem);
	}
	else
	{
		InfoItem.InfoName   = Translate(_T("Hardware Info: "));		
		InfoItem.InfoValue = Info.GetHardwareInfo().c_str();
		m_Info.AddInfo(InfoItem);
	}
   


    UpdateData(FALSE);

    return TRUE;
}



BOOL CClientInfoCtrl::SetThumbnail(const CImage& Thumbnail)
{
    if (Thumbnail.IsNull())
    {
        m_thumbnail.SetBitmap(NULL);
        return TRUE;
    }

    HDC hDstDC = NULL;

    CRect rc;
    // Obtain a DC to the destination image
    hDstDC = m_ThumbnailImage.GetDC();

    rc.left = 0;
    rc.top = 0;
    rc.right = m_ThumbnailImage.GetWidth();
    rc.bottom = m_ThumbnailImage.GetHeight();

    HBRUSH hSolidBrush = CreateSolidBrush(RGB(255,255,255));
    FillRect(hDstDC, &rc, hSolidBrush);
    DeleteObject(hSolidBrush);
    int OldStretchMode = GetStretchBltMode (hDstDC);
    SetStretchBltMode( hDstDC,  HALFTONE );

    FLOAT W2H = ((FLOAT)Thumbnail.GetWidth() / (FLOAT)Thumbnail.GetHeight());
    FLOAT w2h = ((FLOAT)rc.Width() / (FLOAT)rc.Height());
    if ( W2H > w2h)
    {
        INT DeflactY = (rc.Height() - rc.Width()/W2H) / 2;
        rc.DeflateRect(0, DeflactY, 0, DeflactY);
    }
    else
    {
        INT DeflactX = (rc.Width() - rc.Height()*W2H) / 2;
        rc.DeflateRect(DeflactX, 0, DeflactX, 0);
    }

    //Thumbnail.SetTransparentColor(RGB(123,456,789));
    Thumbnail.StretchBlt(hDstDC, rc);
    SetStretchBltMode(hDstDC, OldStretchMode);

    // Release the destination DC
    m_ThumbnailImage.ReleaseDC();

    m_thumbnail.SetBitmap(m_ThumbnailImage);

    m_thumbnail.ShowWindow(SW_SHOW);
    UpdateData(FALSE);

    return TRUE;
}


BOOL  CClientInfoCtrl::ClearInfo()
{
    m_thumbnail.SetBitmap(NULL);

    m_Info.ClearInfo();
    m_thumbnail.ShowWindow(SW_SHOW);
    UpdateData(FALSE);

    return TRUE;
}