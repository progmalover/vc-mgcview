// PropertyMessage.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "PropertyMessage.h"
#include "afxdialogex.h"
#include "PropertyCtrl.h"
#include "Localization/Localization.h"

// CPropertyMessage dialog
extern UINT WM_ON_TEXTSETTING_CHANGE;

UINT WM_MEDIASETTING_UPDATE_BEGIN = ::RegisterWindowMessage(_T("WM_MEDIASETTING_UPDATE_BEGIN"));
UINT WM_MEDIASETTING_UPDATE_END   = ::RegisterWindowMessage(_T("WM_MEDIASETTING_UPDATE_END"));
IMPLEMENT_DYNAMIC(CPropertyMessage, CDialogEx)

    CPropertyMessage::CPropertyMessage(CWnd* pParent /*=NULL*/)
    : CDialogEx(CPropertyMessage::IDD, pParent)
{
    m_pMessage = NULL;
    m_bFontBold = FALSE;
    m_bFontItalic = FALSE;
    m_bFontUnderline = FALSE;
    m_nSpeed = 0;
	m_bVerticalText  = FALSE;
//     m_nTransparency = 50;
}

CPropertyMessage::~CPropertyMessage()
{
    m_pMessage = NULL;
}

void CPropertyMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_strMessage);
	//  DDX_CBIndex(pDX, IDC_COMBO_SIZE, m_nFontSize);
	//  DDV_MinMaxInt(pDX, m_nFontSize, 1, 1000);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_TEXT, m_wndFontColor);
	DDX_Check(pDX, IDC_CHECK_BOLD, m_bFontBold);
	DDX_Check(pDX, IDC_CHECK_ITALIC, m_bFontItalic);
	DDX_Check(pDX, IDC_CHECK_UNDERLINE, m_bFontUnderline);
	DDX_Control(pDX, IDC_MFCFONTCOMBO_TEXT, m_wndFont);
	DDX_Slider(pDX, IDC_SLIDER_SPEED, m_nSpeed);
	DDV_MinMaxInt(pDX, m_nSpeed, 0, 400);
	//     DDX_Slider(pDX, IDC_SLIDER_TRANSPARENCY, m_nTransparency);
	//     DDV_MinMaxInt(pDX, m_nTransparency, 0, 100);
	DDX_CBString(pDX, IDC_COMBO_SIZE, m_strFontSize);
	DDX_Control(pDX, IDC_COMBO_DIRECTION, m_wndDirection);
	//DDX_Control(pDX, IDC_COMBO_ALIGNMENT, m_wndAlignment);
	DDX_Control(pDX, IDC_COMBO_LAYOUT, m_wndLayout);
	DDX_Control(pDX, IDC_COMBO_FONTDIRECTION, m_fontDirection);
	DDX_Control(pDX, IDC_SLIDER_SPEED, m_ctrlSpeed);
}


BEGIN_MESSAGE_MAP(CPropertyMessage, CDialogEx)
    ON_WM_SIZE()
    ON_EN_CHANGE(IDC_EDIT_MESSAGE, &CPropertyMessage::OnChangeEditMessage)
    //    ON_CBN_EDITCHANGE(IDC_MFCFONTCOMBO_TEXT, &CPropertyMessage::OnEditchangeMfcfontcomboText)
    ON_CBN_EDITCHANGE(IDC_COMBO_SIZE, &CPropertyMessage::OnEditchangeComboSize)
    //ON_CBN_SELCHANGE(IDC_COMBO_ALIGNMENT, &CPropertyMessage::OnSelchangeComboAlignment)
    ON_CBN_SELCHANGE(IDC_COMBO_DIRECTION, &CPropertyMessage::OnSelchangeComboDirection)
    ON_CBN_SELCHANGE(IDC_COMBO_LAYOUT, &CPropertyMessage::OnSelchangeComboLayout)
    ON_BN_CLICKED(IDC_CHECK_BOLD, &CPropertyMessage::OnClickedCheckBold)
    ON_BN_CLICKED(IDC_CHECK_ITALIC, &CPropertyMessage::OnClickedCheckItalic)
    ON_BN_CLICKED(IDC_CHECK_UNDERLINE, &CPropertyMessage::OnClickedCheckUnderline)
    ON_CBN_SELCHANGE(IDC_MFCFONTCOMBO_TEXT, &CPropertyMessage::OnSelchangeMfcfontcomboText)
    ON_BN_CLICKED(IDC_MFCCOLORBUTTON_TEXT, &CPropertyMessage::OnClickedMfccolorbuttonText)
    ON_CBN_SELCHANGE(IDC_COMBO_SIZE, &CPropertyMessage::OnSelchangeComboSize)
    ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SPEED, &CPropertyMessage::OnReleasedcaptureSliderSpeed)
	ON_NOTIFY(NM_CUSTOMDRAW,IDC_SLIDER_SPEED,&CPropertyMessage::OnReleasedcaptureSliderSpeed)
//     ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_TRANSPARENCY, &CPropertyMessage::OnReleasedcaptureSliderTransparency)
ON_CBN_SELCHANGE(IDC_COMBO_FONTDIRECTION, &CPropertyMessage::OnCbnSelchangeComboFontdirection)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CPropertyMessage)
    EASYSIZE(IDC_EDIT_MESSAGE,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_MFCFONTCOMBO_TEXT,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_MFCCOLORBUTTON_TEXT,IDC_MFCFONTCOMBO_TEXT,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_DIRECTION,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_SLIDER_SPEED,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_LAYOUT,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
    EASYSIZE(IDC_COMBO_FONTDIRECTION,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
//     EASYSIZE(IDC_SLIDER_TRANSPARENCY, ES_BORDER, ES_BORDER, ES_BORDER, ES_KEEPSIZE, 0)
    //EASYSIZE(IDC_COMBO_ALIGNMENT,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
END_EASYSIZE_MAP

static int nFontSizes[] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
// CPropertyMessage message handlers
BOOL CPropertyMessage::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    INIT_EASYSIZE;

    for (int i = 0; i < S3S_TEXT_DIRECTION_TOTAL-1; ++i)
    {
		int index;
        if (szTextDirection[i] == _T(""))
		{
            index = m_wndDirection.AddString(_T("N/A"));		
		}
        else
		{
            index = m_wndDirection.AddString(Translate(szTextDirection[i]));		
		}
		m_wndDirection.SetItemData(index, i);
    }

    //for (int i = 0; i < S3S_TEXT_ALIGNMENT_TOTAL; ++i)
    //{
    //    if (szTextDirection[i] == _T(""))
    //        m_wndAlignment.AddString(_T("N/A"));
    //    else
    //        m_wndAlignment.AddString(szTextAlignment[i]);
    //}

    for (int i = 0; i < S3S_TEXT_LAYOUT_TOTAL; ++i)
    {
        int index = m_wndLayout.AddString(Translate(szTextLayout[i]));
		m_wndLayout.SetItemData(index, i);
    }

	for (int i = 0; i < S3S_TEXT_LAYOUT_TOTAL; ++i)
	{
		int index = m_fontDirection.AddString(Translate(szTextLayout[i]));
		m_fontDirection.SetItemData(index, i);
	}

    CFont font;
    LOGFONT logFont;
    CWnd* checkbox;

    checkbox = (CWnd*)GetDlgItem(IDC_CHECK_BOLD);
    checkbox->GetFont()->GetLogFont(&logFont);
    logFont.lfWeight = FW_BOLD;
    font.CreateFontIndirect(&logFont);
    checkbox->SetFont(&font);
    font.Detach();

    checkbox = (CWnd*)GetDlgItem(IDC_CHECK_ITALIC);
    checkbox->GetFont()->GetLogFont(&logFont);
    logFont.lfItalic = 1;
    font.CreateFontIndirect(&logFont);
    checkbox->SetFont(&font);
    font.Detach();

    checkbox = (CWnd*)GetDlgItem(IDC_CHECK_UNDERLINE);
    checkbox->GetFont()->GetLogFont(&logFont);
    logFont.lfUnderline = 1;
    font.CreateFontIndirect(&logFont);
    checkbox->SetFont(&font);
    font.Detach();

    LOGFONT lf;
    CWnd* content = (CWnd*)GetDlgItem(IDC_STATIC_TEXT);
    content->GetFont()->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;
    font.CreateFontIndirect(&lf);
    content->SetFont(&font);
    font.Detach();

	m_strMsg.SubclassDlgItem (IDC_EDIT_MESSAGE, this);
	m_strMsg.SetBackgroundColourFocus (RGB (150, 200, 255));
	m_strMsg.SetRegEx(_T("[^\s]*"));
	m_strMsg.LimitText(200);

	m_ctrlSpeed.SetRange(0, 400);
	Translate(this);
    return FALSE;  // return TRUE  unless you set the focus to a control
}


void CPropertyMessage::OnSize(UINT nType, int cx, int cy) 
{
    CDialogEx::OnSize(nType, cx, cy);
    UPDATE_EASYSIZE;
}

BOOL CPropertyMessage::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Add your specialized code here and/or call the base class

    CDialogEx::PreCreateWindow(cs);

    cs.style |= WS_CLIPCHILDREN;
    return TRUE;
}

VOID CPropertyMessage::SetContent(std::shared_ptr<S3SIGNAGE_TEXT_SETTING> &MessageSetting)
{
    if (MessageSetting.get())
    {
        m_pMessage = MessageSetting;

        m_strMessage = m_pMessage->szMessage;
        m_wndFontColor.SetColor(m_pMessage->FontColor);

        LOGFONT logFont;
        m_pMessage->pFontInfo->GetLogFont(&logFont);
        m_wndFont.SelectFont(logFont.lfFaceName);
        m_bFontBold = logFont.lfWeight == FW_BOLD;
        m_bFontItalic = logFont.lfItalic;
        m_bFontUnderline = logFont.lfUnderline;

        m_nSpeed = m_pMessage->FontSpeed;

		int dirIndex = 0;
		for (int i = 0; i < S3S_TEXT_LAYOUT_TOTAL; ++i)
		{
			if (m_pMessage->TextLayout == szTextLayout[i])
			{
				dirIndex = i;
			}
		}

		int iTextDirIndex = 0;
		m_wndLayout.SetCurSel(dirIndex);

		if (dirIndex == S3S_TEXT_LAYOUT_V)
		{
			if (!m_bVerticalText)
			{
				m_bVerticalText = TRUE;
				m_wndDirection.ResetContent();
				for (int i = 0; i < S3S_VERTICAL_TEXT_DIRECTION_TOTAL; ++i)
				{
					int index;
					if (szVerticalTextDirection[i] == _T(""))
					{
						index = m_wndDirection.AddString(_T("N/A"));		
					}
					else
					{
						index = m_wndDirection.AddString(Translate(szVerticalTextDirection[i]));		
					}
					m_wndDirection.SetItemData(index, i);
				}
			}

			iTextDirIndex = 0;
			for (int i = 0; i < S3S_VERTICAL_TEXT_DIRECTION_TOTAL; ++i)
			{
				if (m_pMessage->TextDirection == szVerticalTextDirection[i])
				{
					iTextDirIndex = i;
				}
			}
			if (m_pMessage->TextDirection == _T(""))
			{
				iTextDirIndex = 0;
				GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(TRUE);
			}
			m_wndDirection.SetCurSel(iTextDirIndex);

		}
		else if (dirIndex == S3S_TEXT_LAYOUT_H)
		{
			if (m_bVerticalText)
			{
				m_bVerticalText = FALSE;
				m_wndDirection.ResetContent();
				for (int i = 0; i < S3S_TEXT_DIRECTION_TOTAL; ++i)
				{
					int index;
					if (szTextDirection[i] == _T(""))
					{
						index = m_wndDirection.AddString(_T("N/A"));		
					}
					else
					{
						index = m_wndDirection.AddString(Translate(szTextDirection[i]));		
					}
					m_wndDirection.SetItemData(index, i);
				}
			}

			iTextDirIndex = 0;
			for (int i = 0; i < S3S_TEXT_DIRECTION_TOTAL; ++i)
			{
				if (m_pMessage->TextDirection == szTextDirection[i])
				{
					iTextDirIndex = i;
				}
			}

			if (m_pMessage->TextDirection == _T("") || m_pMessage->TextDirection == _T("Center"))
			{
				iTextDirIndex = 0;
				GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(FALSE);
			}
			else
			{
				GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(TRUE);
			}
			m_wndDirection.SetCurSel(iTextDirIndex);

		}

		m_strFontSize = StringUtility::itos(logFont.lfHeight).c_str();

        //dirIndex = 0;
        //for (int i = 0; i < S3S_TEXT_ALIGNMENT_TOTAL; ++i)
        //{
        //    if (m_pMessage->Alignment == szTextAlignment[i])
        //    {
        //        dirIndex = i;
        //    }
        //}
        //if (m_pMessage->Alignment == _T(""))
        //{
        //    dirIndex = 0;
        //}
        //m_wndAlignment.SetCurSel(dirIndex);

		dirIndex = 0;
		for (int i = 0; i < S3S_TEXT_LAYOUT_TOTAL; ++i)
		{
			if (m_pMessage->FontDirection == szTextLayout[i])
			{
				dirIndex = i;
			}
		}
		m_fontDirection.SetCurSel(dirIndex);

        UpdateData(FALSE);
    }
}

void CPropertyMessage::Clear()
{
    m_pMessage = NULL;
}

void CPropertyMessage::UpdateContent()
{
    UpdateData(TRUE);
    if (m_pMessage)
    {
        GetParent()->SendMessage(WM_MEDIASETTING_UPDATE_BEGIN,0,0);
        m_pMessage->szMessage = m_strMessage;

		DWORD SelTextLayoutIndex = m_wndLayout.GetItemData(m_wndLayout.GetCurSel());
		m_pMessage->TextLayout = szTextLayout[SelTextLayoutIndex];

		DWORD SelDirIndex = m_wndDirection.GetItemData(m_wndDirection.GetCurSel());
		if (SelTextLayoutIndex == S3S_TEXT_LAYOUT_V)
		{
			m_pMessage->TextDirection = szVerticalTextDirection[SelDirIndex];
		}
		else
		{
			m_pMessage->TextDirection = szTextDirection[SelDirIndex];
		}

        if (m_pMessage->TextDirection == _T("")  || m_pMessage->TextDirection == _T("Center"))
        {
            GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(FALSE);
        }
        else
        {
            GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(TRUE);
        }
        //m_wndAlignment.GetLBText(m_wndAlignment.GetCurSel(), m_pMessage->Alignment);
        //if (m_pMessage->Alignment == _T("N/A"))
        //{
        //    m_pMessage->Alignment = _T("");
        //}
       


		DWORD SelFontDirectionIndex = m_fontDirection.GetItemData(m_fontDirection.GetCurSel());
		m_pMessage->FontDirection = szTextLayout[SelFontDirectionIndex];
		GetParent()->PostMessage(WM_ON_TEXTSETTING_CHANGE, 0, 0);
        GetParent()->SendMessage(WM_MEDIASETTING_UPDATE_END,0,0);

    }
}



void CPropertyMessage::OnChangeEditMessage()
{
    if (!IsMsgLenValid())
    {
        return;
    }
    
    UpdateContent();
}

bool CPropertyMessage::IsMsgLenValid()
{
    CString lastMsg = m_strMessage;
    
    UpdateData(TRUE);
	/*
    if (m_strMessage.GetLength() > 200)
    {
        MessageBox(Translate(_T("The length of the message can not be longer than 200!")),
            Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
        m_strMessage = lastMsg;
        UpdateData(FALSE);
        return FALSE;
    }
	*/
    return TRUE;
}

void CPropertyMessage::OnEditchangeComboSize()
{
    UpdateData(TRUE);
    int size = _tstoi(m_strFontSize.GetString());
    if (size < 1 || size > 500)
    {
        MessageBox(Translate(_T("Invalid font size, font size is between 1 to 500")), Translate(_T("Warning:Check value")), MB_OK|MB_ICONEXCLAMATION);
        if (size < 1)
        {
            size = 1;
        }
        else
        {
            size = 500;
        }
        m_strFontSize.SetString(StringUtility::itos(size).c_str());
        UpdateData(FALSE);
    }
    else
        UpdateFont();
}


void CPropertyMessage::OnSelchangeComboAlignment()
{
    UpdateContent();
}


void CPropertyMessage::OnSelchangeComboDirection()
{
    UpdateContent();
}


void CPropertyMessage::OnSelchangeComboLayout()
{
	UpdateTextDirCtrl();
    UpdateContent();
}


void CPropertyMessage::OnClickedCheckBold()
{
    UpdateFont();
}


void CPropertyMessage::OnClickedCheckItalic()
{
    UpdateFont();
}


void CPropertyMessage::OnClickedCheckUnderline()
{
    UpdateFont();
}


void CPropertyMessage::OnSelchangeMfcfontcomboText()
{
    UpdateFont();
}


void CPropertyMessage::OnClickedMfccolorbuttonText()
{
    UpdateFont();
}

void CPropertyMessage::UpdateFont(bool isSizeEdit)
{
    UpdateData(TRUE);
    if (m_pMessage)
    {
        GetParent()->SendMessage(WM_MEDIASETTING_UPDATE_BEGIN,0,0);
        LOGFONT logFont;
        m_pMessage->pFontInfo->GetLogFont(&logFont);

        wcscpy_s(logFont.lfFaceName, m_wndFont.GetSelFont()->m_strName.GetString());
        logFont.lfWeight = m_bFontBold ? FW_BOLD : FW_DONTCARE;
        logFont.lfItalic = m_bFontItalic;
        logFont.lfUnderline = m_bFontUnderline;

        if (isSizeEdit)
        {
            if (m_strFontSize.IsEmpty())
            {
                logFont.lfHeight = 12;
            }
            else
            {
                logFont.lfHeight = _tstoi(m_strFontSize.GetString());
            }
        }
        else
        {
            int index = ((CComboBox*)GetDlgItem(IDC_COMBO_SIZE))->GetCurSel();
            ((CComboBox*)GetDlgItem(IDC_COMBO_SIZE))->GetLBText(index, m_strFontSize);
            if (m_strFontSize.IsEmpty())
            {
                logFont.lfHeight = 12;
            }
            else
            {
                logFont.lfHeight = _tstoi(m_strFontSize.GetString());
            }
        }

        m_pMessage->pFontInfo->DeleteObject();
        m_pMessage->pFontInfo->CreateFontIndirect(&logFont);
        m_pMessage->FontColor = m_wndFontColor.GetColor();

		GetParent()->PostMessage(WM_ON_TEXTSETTING_CHANGE, 0, 0);
        GetParent()->SendMessage(WM_MEDIASETTING_UPDATE_END,0,0);
    }
}


void CPropertyMessage::OnSelchangeComboSize()
{
    UpdateFont(false);
}


void CPropertyMessage::OnReleasedcaptureSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
{
    UpdateData(TRUE);
    if (m_pMessage)
    {
        m_pMessage->FontSpeed = m_nSpeed;
    }
    *pResult = 0;
	GetParent()->PostMessage(WM_ON_TEXTSETTING_CHANGE, 0, 0);
}

/*
void CPropertyMessage::OnReleasedcaptureSliderTransparency(NMHDR *pNMHDR, LRESULT *pResult)
{
    UpdateData(TRUE);
    if (m_pMessage)
    {
        m_pMessage->Transparency = m_nTransparency;
    }
    *pResult = 0;
}
*/

BOOL CPropertyMessage::PreTranslateMessage(MSG* pMsg)
{
    m_focus.InitFocusEx(GetFocusableWindow, this);

    if (m_focus.ProcessKeyPressMessage(this, pMsg))
    {
        return TRUE;
    }
    if (pMsg->message == WM_MOUSEWHEEL)
    {
        GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
        return TRUE;
    }
    if (pMsg->message == WM_LBUTTONDOWN)
    {
        GetParent()->SetFocus();
    }

    return __super::PreTranslateMessage(pMsg);
}

HWND CPropertyMessage::GetFocusableWindow(int nPoisition, LPVOID lParam, BOOL shift)
{
    CPropertyMessage* pThis = (CPropertyMessage*)lParam;
    switch (nPoisition)
    {
    case FOCUSABLEWINDOW_POSITION_FIRST:
        return CFocusEx::GetFirstFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_LAST:
        return CFocusEx::GetLastFocusableWindow(pThis->GetSafeHwnd());
    case FOCUSABLEWINDOW_POSITION_FOCUSABLE:
        {
            CPropertyCtrl* pParent = (CPropertyCtrl*)(pThis->GetParent());
            return pParent->GetNextFocusWindow(pThis->GetSafeHwnd(), shift);
        }
    }
    return NULL;
}


void CPropertyMessage::OnCbnSelchangeComboFontdirection()
{
	// TODO: Add your control notification handler code here
	UpdateContent();
}

void CPropertyMessage::UpdateTextDirCtrl()
{
	UpdateData();

	DWORD SelTextLayoutIndex = m_wndLayout.GetItemData(m_wndLayout.GetCurSel());
	if (m_bVerticalText == FALSE && SelTextLayoutIndex == S3S_TEXT_LAYOUT_V)
	{
		m_bVerticalText = TRUE;
		m_wndDirection.ResetContent();
		for (int i = 0; i < S3S_VERTICAL_TEXT_DIRECTION_TOTAL; ++i)
		{
			int index;
			if (szVerticalTextDirection[i] == _T(""))
			{
				index = m_wndDirection.AddString(_T("N/A"));	
			}
			else
			{
				index = m_wndDirection.AddString(Translate(szVerticalTextDirection[i]));		
			}
			m_wndDirection.SetItemData(index, i);
		}
		m_wndDirection.SetCurSel(S3S_VERTICAL_TEXT_DIRECTION_NONE);
		UpdateData(FALSE);
	}
	else if (m_bVerticalText == TRUE && SelTextLayoutIndex == S3S_TEXT_LAYOUT_H)
	{
		m_bVerticalText = FALSE;
		m_wndDirection.ResetContent();
		for (int i = 0; i < S3S_TEXT_DIRECTION_TOTAL; ++i)
		{
			int index;
			if (szTextDirection[i] == _T(""))
			{
				index = m_wndDirection.AddString(_T("N/A"));		
			}
			else
			{
				index = m_wndDirection.AddString(Translate(szTextDirection[i]));		
			}
			m_wndDirection.SetItemData(index, i);
		}
		m_wndDirection.SetCurSel(S3S_VERTICAL_TEXT_DIRECTION_NONE);
		UpdateData(FALSE);
	}
}

void CPropertyMessage::SetActualRunTime(CString str)
{
	GetDlgItem(IDC_ACTUAL_RUNTIME)->SetWindowText(str);
}