#include "stdafx.h"
#include "Controller.h"
#include "EmergencyMessageDlg.h"
#include "afxdialogex.h"
#include "LayoutSettings.h"
#include "Localization/Localization.h"

IMPLEMENT_DYNAMIC(EmergencyMessageDlg, CDialogEx)

EmergencyMessageDlg::EmergencyMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(EmergencyMessageDlg::IDD, pParent)
{
    m_bInit = false;
}

EmergencyMessageDlg::~EmergencyMessageDlg()
{
}

void EmergencyMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EMGM_TEXT, m_MessageContent);
	DDX_DateTimeCtrl(pDX, IDC_EMGM_DATE_PICKER, m_Date);
	DDX_DateTimeCtrl(pDX, IDC_EMGM_STARTTIME, m_StartTime);
	DDX_DateTimeCtrl(pDX, IDC_EMGM_DURATION, m_Duration);
	DDX_Control(pDX, IDC_EMGM_FONT, m_ctrlFont);
	//     DDX_CBIndex(pDX, IDC_COMBO_SIZE, m_SizeIndex);
	DDX_Check(pDX, IDC_CHECK_BOLD, m_bBold);
	DDX_Check(pDX, IDC_CHECK_ITALIC, m_bItalic);
	DDX_Check(pDX, IDC_CHECK_UNDERLINE, m_bUnderline);
	DDX_Slider(pDX, IDC_SLIDER_SPEED, m_Speed);
	DDX_Slider(pDX, IDC_SLIDER_TRANS, m_Transparency);
	DDX_Control(pDX, IDC_COMBO_SIZE, m_ctrlFontSize);
	DDX_Control(pDX, IDC_COMBO_DIRECTION, m_ctrlDirection);
	DDX_Control(pDX, IDC_SLIDER_SPEED, m_ctrlSpeed);
	DDX_Control(pDX, IDC_SLIDER_TRANS, m_ctrlTransparency);
	DDX_Control(pDX, IDC_EMGM_COLOR, m_ColorControl);
	DDX_Control(pDX, IDC_LIST_MSG_HISTORY, m_ctrlListMsgHistory);
	DDX_Control(pDX, IDC_EMGM_DURATION, m_ctrlDuration);
	//  DDX_Text(pDX, IDC_TIMES, m_ShowTimes);
	//DDV_MinMaxInt(pDX, m_ShowTimes, 1, 9999);
	DDX_Text(pDX, IDC_TIMES, m_ShowTimes);
}


BEGIN_MESSAGE_MAP(EmergencyMessageDlg, CDialogEx)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDOK, &EmergencyMessageDlg::OnBnClickedOk)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MSG_HISTORY, &EmergencyMessageDlg::OnLvnItemchangedListMsgHistory)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_EMGM_DURATION, &EmergencyMessageDlg::OnDtnDatetimechangeEmgmDuration)
    ON_CBN_EDITCHANGE(IDC_COMBO_SIZE, &EmergencyMessageDlg::OnCbnEditchangeComboSize)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &EmergencyMessageDlg::OnBnClickedStop)
	ON_CBN_SELCHANGE(IDC_COMBO_DIRECTION, &EmergencyMessageDlg::OnSelchangeComboDirection)
	//	ON_EN_KILLFOCUS(IDC_TIMES, &EmergencyMessageDlg::OnEnKillfocusTimes)
	ON_EN_CHANGE(IDC_TIMES, &EmergencyMessageDlg::OnEnChangeTimes)
	ON_EN_CHANGE(IDC_EMGM_TEXT, &EmergencyMessageDlg::OnEnChangeEmgmText)
	ON_CBN_SELCHANGE(IDC_EMGM_FONT, &EmergencyMessageDlg::OnCbnSelchangeEmgmFont)
	ON_CBN_SELCHANGE(IDC_COMBO_SIZE, &EmergencyMessageDlg::OnCbnSelchangeComboSize)
	ON_BN_CLICKED(IDC_CHECK_BOLD, &EmergencyMessageDlg::OnBnClickedCheckBold)
	ON_BN_CLICKED(IDC_CHECK_ITALIC, &EmergencyMessageDlg::OnBnClickedCheckItalic)
	//ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_SPEED, &EmergencyMessageDlg::OnTRBNThumbPosChangingSliderSpeed)
//	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_SPEED, &EmergencyMessageDlg::OnNMReleasedcaptureSliderSpeed)
	ON_BN_CLICKED(IDC_CHECK_UNDERLINE, &EmergencyMessageDlg::OnBnClickedCheckUnderline)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(EmergencyMessageDlg)
	/*EASYSIZE(IDC_STATIC_INFO_GROUP,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_INFO,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_START_TIME,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_END_TIME,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_PLAY_DURATION,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_DATETIMEPICKER_START,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_DATETIMEPICKER_END,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_DATETIMEPICKER_DURATION,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)*/
END_EASYSIZE_MAP

static int nFontSizes[] = {8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};
// CScheduleItemCtrl message handlers
BOOL EmergencyMessageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
    INIT_EASYSIZE;
    m_bInit = true;
	
    // Update the history
    m_ctrlListMsgHistory.ModifyStyle(LVS_LIST, LVS_REPORT);
    //m_ctrlListMsgHistory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    
    m_ctrlListMsgHistory.InsertColumn(0, Translate(_T("Time")), LVCFMT_LEFT, 200);
    m_ctrlListMsgHistory.InsertColumn(1, Translate(_T("Message")), LVCFMT_LEFT, 300);
    // the new one
    INT tempIndex = m_ctrlListMsgHistory.InsertItem(0, Translate(_T("New Message")));
    m_ctrlListMsgHistory.SetItemData(tempIndex, NEW_MSG_INDEX);
//     m_ctrlListMsgHistory.SetItemText(tempIndex, 1,  StringUtility::itos(NEW_MSG_INDEX).c_str());
    m_curMsgIndex = NEW_MSG_INDEX;

    for (UINT i = 0; i < m_MsgHistory.size(); i++)
    {
        S3Time End = m_MsgHistory[i].GetEndTime();
        S3Time Start = m_MsgHistory[i].GetStartTime();
        COleDateTimeSpan Span = End.ToCOleDateTime() - Start.ToCOleDateTime();
        CString Duration;
        INT minutes = static_cast<int>(Span.GetTotalMinutes());
        INT seconds = static_cast<int>(Span.GetTotalSeconds()) % 60;
        Duration.Format(_T("%d m %d s"), minutes, seconds);

        INT tempIndex = m_ctrlListMsgHistory.InsertItem(i+1, (m_MsgHistory[i].GetStartTime().ToString() 
            + _T(": ")).c_str() + Duration);
        m_ctrlListMsgHistory.SetItemData(tempIndex, i);

        m_ctrlListMsgHistory.SetItemText(i+1, 1, m_MsgHistory[i].GetCustomizedSetting(_T("MessageContent")).c_str());
        //m_ctrlListMsgHistory.SetItemText(tempIndex, 1,  StringUtility::itos(i).c_str());
    }
    
    m_ctrlListMsgHistory.SetFocus();
    m_ctrlListMsgHistory.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
    GetDlgItem(IDC_EMGM_TEXT)->SetFocus();

    // update ctrl

    for (INT i = 0; i < S3S_EMERGENCY_TEXT_DIRECTION_TOTAL; i++)
    {
        int index = m_ctrlDirection.AddString(Translate(szEmergencyTextDirection[i]));
		m_ctrlDirection.SetItemData(index, i);
    }

    TCHAR buf[25];
    INT FontSZcnt = sizeof(nFontSizes)/sizeof(INT);
    for (INT i = 0; i < FontSZcnt; i++)
    {
        _itot_s(nFontSizes[i], buf, 10);
        m_ctrlFontSize.AddString(buf);
    }
    m_ctrlFontSize.SetCurSel(10);
    m_ctrlFontSize.SetWindowText(_T("24"));

    m_Date = COleDateTime::GetCurrentTime();
    m_StartTime = COleDateTime::GetCurrentTime();
    m_Duration = COleDateTime(1983,3,17,0,10,0);

    m_ctrlSpeed.SetRange(0, 400);
    m_ctrlSpeed.SetTicFreq(10);

    m_ctrlTransparency.SetRange(0, 100);
    m_ctrlTransparency.SetTicFreq(10);

    m_ctrlDirection.SetCurSel(2);
	
	DWORD TextDirIndex = m_ctrlDirection.GetItemData(m_ctrlDirection.GetCurSel());
	if (TextDirIndex == S3S_EMERGENCY_TEXT_DIRECTION_NONE)
	{  
		GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(TRUE);
		GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(FALSE);
		GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);
	}

    m_ctrlFont.SelectFont(_T("Times New Roman"));
    m_Color = RGB(0, 0, 0);
    m_ColorControl.SetColor(m_Color);
    m_Speed = 200;
    m_Transparency = 50;
	m_ShowTimes = _T("1");

    ((CDateTimeCtrl*)GetDlgItem(IDC_EMGM_STARTTIME))->SetFormat(_T("HH:mm:ss"));
    ((CDateTimeCtrl*)GetDlgItem(IDC_EMGM_DURATION))->SetFormat(_T("HH:mm:ss"));

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


    UpdateData(FALSE);

	Translate(this);

    m_bInit = false;
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void EmergencyMessageDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}


void EmergencyMessageDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_MessageContent.GetLength() == 0)
	{
		MessageBox(Translate(_T("Please input emergency message.")), Translate(_T("Warning:Edit emergency message")),MB_OK|MB_ICONEXCLAMATION);
		return;
	}
    CDialogEx::OnOK();
    UpdateNewMsg();
}


BOOL EmergencyMessageDlg::SetMsgHistory(std::vector<EmergencyMessage>& history)
{
    //m_MsgHistory = history;
	//not show stop message.
	m_MsgHistory.clear();
	std::vector<EmergencyMessage> :: iterator it    = history.begin();
	for (; it < history.end(); it++)
	{
		if (it->GetCustomizedSetting(_T("MessageContent")) == _T(""))
		{
			continue;
		}
		m_MsgHistory.push_back(*it);
	}
    return TRUE;
}

void EmergencyMessageDlg::OnLvnItemchangedListMsgHistory(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    int iCurrent = pNMLV->iItem;
    // TODO: Add your control notification handler code here
    //INT itemData = m_ctrlListMsgHistory.GetItemData(iCurrent);
    INT itemData = m_ctrlListMsgHistory.GetItemData(iCurrent);
    BOOL bEnableOK = (itemData == NEW_MSG_INDEX) ;
    GetDlgItem(IDOK)->EnableWindow(bEnableOK);

    if (m_curMsgIndex == NEW_MSG_INDEX)
    { 
        UpdateNewMsg();
    }

    UpdateCtrls(itemData);

    m_curMsgIndex = itemData;

    *pResult = 0;
}

BOOL EmergencyMessageDlg::UpdateNewMsg()
{
    if (m_bInit)
    {
        return TRUE;
    }
    // update ctrl data
    UpdateData(TRUE);

    if (m_ctrlFont.GetSelFont())
    {
        //m_Font = m_ctrlFont.GetSelFont()->GetFullName();
        m_Font = m_ctrlFont.GetSelFont()->m_strName.GetString();
    }

    DWORD TextDirIndex = m_ctrlDirection.GetItemData(m_ctrlDirection.GetCurSel());
    if (TextDirIndex >=0 && TextDirIndex < S3S_EMERGENCY_TEXT_DIRECTION_TOTAL)
    {  
        m_strDirection = szEmergencyTextDirection[TextDirIndex];
    }

/*
    if (m_SizeIndex == -1)
        m_SizeIndex = 10;

    m_Size = nFontSizes[m_SizeIndex];
*/


    CString strFontSize;
    GetDlgItem(IDC_COMBO_SIZE)->GetWindowText(strFontSize);


    m_Color = m_ColorControl.GetColor();



    //
    TCHAR buf[25];

    m_newEmessage.AddCustomizedSetting(_T("MessageContent"), m_MessageContent.GetString());
    m_newEmessage.AddCustomizedSetting(_T("Direction"), m_strDirection.GetString());
    m_newEmessage.AddCustomizedSetting(_T("FontFamily"), m_Font.GetString());

//     _itot_s(m_Size, buf, 10);
    m_newEmessage.AddCustomizedSetting(_T("FontSize"), strFontSize.GetString());

    _itot_s(m_Color, buf, 10);
    m_newEmessage.AddCustomizedSetting(_T("FontColor"), buf);

    CString T_F;
    T_F = m_bBold ? _T("TRUE") : _T("FALSE");
    m_newEmessage.AddCustomizedSetting(_T("Bold"), T_F.GetString());

    T_F = m_bItalic ? _T("TRUE") : _T("FALSE");
    m_newEmessage.AddCustomizedSetting(_T("Italic"), T_F.GetString());

    T_F = m_bUnderline ? _T("TRUE") : _T("FALSE");
    m_newEmessage.AddCustomizedSetting(_T("Underline"), T_F.GetString());

    _itot_s(m_Speed, buf, 10);
    m_newEmessage.AddCustomizedSetting(_T("Speed"), buf);

    _itot_s(m_Transparency, buf, 10);
    m_newEmessage.AddCustomizedSetting(_T("Transparency"), buf);

	_itot_s(GetSystemMetrics(SM_CXSCREEN), buf, 10);
	m_newEmessage.AddCustomizedSetting(_T("CtrScnSize"), buf);

	//_itot_s(m_ShowTimes, buf, 10);
	m_newEmessage.AddCustomizedSetting(_T("ShowTimes"), m_ShowTimes.GetBuffer());

    COleDateTime StartTime = COleDateTime(m_Date.GetYear(), m_Date.GetMonth(), m_Date.GetDay(),
        m_StartTime.GetHour(), m_StartTime.GetMinute(), m_StartTime.GetSecond());

	COleDateTimeSpan Duration;
	if (TextDirIndex == S3S_EMERGENCY_TEXT_DIRECTION_NONE)
	{
		Duration.SetDateTimeSpan(0, m_Duration.GetHour(), m_Duration.GetMinute(), m_Duration.GetSecond());
	}
	else
	{
		CalculateDuration(Duration);
	}			
    m_newEmessage.AddStartTime(S3Time::CreateTime(StartTime));
    m_newEmessage.AddEndTime(S3Time::CreateTime(StartTime + Duration));
    return TRUE;
}

BOOL EmergencyMessageDlg::UpdateCtrls(INT index)
{
    if (index < 0 )
    {
        return FALSE;
    }

    if(m_MsgHistory.size() == 0) return FALSE;

    EmergencyMessage* pEmessage;
    if (index == NEW_MSG_INDEX)
    {
        pEmessage = &m_newEmessage;

        //GetDlgItem(IDC_EMGM_TEXT)->EnableWindow(TRUE);
		((CEdit *)(GetDlgItem(IDC_EMGM_TEXT)))->SetReadOnly(FALSE);
        GetDlgItem(IDC_EMGM_DATE_PICKER)->EnableWindow(TRUE);
        GetDlgItem(IDC_EMGM_STARTTIME)->EnableWindow(TRUE);
        //GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(TRUE);
        GetDlgItem(IDC_EMGM_FONT)->EnableWindow(TRUE);
        GetDlgItem(IDC_COMBO_DIRECTION)->EnableWindow(TRUE);
        //GetDlgItem(IDC_EMGM_COLOR)->EnableWindow(TRUE);
        GetDlgItem(IDC_COMBO_SIZE)->EnableWindow(TRUE);
        GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(TRUE);
        GetDlgItem(IDC_SLIDER_TRANS)->EnableWindow(TRUE);

        GetDlgItem(IDC_CHECK_BOLD)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_ITALIC)->EnableWindow(TRUE);
        GetDlgItem(IDC_CHECK_UNDERLINE)->EnableWindow(TRUE);
    }
    else
    {
        pEmessage = &m_MsgHistory[index];
        //GetDlgItem(IDC_EMGM_TEXT)->EnableWindow(FALSE);
		((CEdit *)(GetDlgItem(IDC_EMGM_TEXT)))->SetReadOnly();
        GetDlgItem(IDC_EMGM_DATE_PICKER)->EnableWindow(FALSE);
        GetDlgItem(IDC_EMGM_STARTTIME)->EnableWindow(FALSE);
        GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(FALSE);
        GetDlgItem(IDC_EMGM_FONT)->EnableWindow(FALSE);
        GetDlgItem(IDC_COMBO_DIRECTION)->EnableWindow(FALSE);
        //GetDlgItem(IDC_EMGM_COLOR)->EnableWindow(FALSE);
        GetDlgItem(IDC_COMBO_SIZE)->EnableWindow(FALSE);
        GetDlgItem(IDC_SLIDER_SPEED)->EnableWindow(FALSE);
        GetDlgItem(IDC_SLIDER_TRANS)->EnableWindow(FALSE);

        GetDlgItem(IDC_CHECK_BOLD)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_ITALIC)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_UNDERLINE)->EnableWindow(FALSE);
        GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
    }

    //

    m_MessageContent = pEmessage->GetCustomizedSetting(_T("MessageContent")).c_str();

    m_strDirection = pEmessage->GetCustomizedSetting(_T("Direction")).c_str();
	m_strDirection = Translate(m_strDirection);
    m_ctrlDirection.SetCurSel(m_ctrlDirection.FindString(0,m_strDirection));

    m_Font = pEmessage->GetCustomizedSetting(_T("FontFamily")).c_str();
    m_ctrlFont.SelectFont(m_Font);

	m_ShowTimes = (pEmessage->GetCustomizedSetting(_T("ShowTimes"))).c_str();
    m_Size = StringUtility::stoi(pEmessage->GetCustomizedSetting(_T("FontSize")));
    m_Color = StringUtility::stoi(pEmessage->GetCustomizedSetting(_T("FontColor")));
    m_ColorControl.SetColor(m_Color);
    
    if (pEmessage->GetCustomizedSetting(_T("Bold")) == _T("TRUE"))
    {
        m_bBold = TRUE;
    }
    else
    {
        m_bBold = FALSE;
    }
    
    if (pEmessage->GetCustomizedSetting(_T("Italic")) == _T("TRUE"))
    {
        m_bItalic = TRUE;
    }
    else
    {
        m_bItalic = FALSE;
    }

    if (pEmessage->GetCustomizedSetting(_T("Underline")) == _T("TRUE"))
    {
        m_bUnderline = TRUE;
    }
    else
    {
        m_bUnderline = FALSE;
    }

    m_Speed = StringUtility::stoi(pEmessage->GetCustomizedSetting(_T("Speed")));
    m_Transparency = StringUtility::stoi(pEmessage->GetCustomizedSetting(_T("Transparency")));

    S3Time End = pEmessage->GetEndTime();
    S3Time Start = pEmessage->GetStartTime();
    
    m_Duration = End.ToCOleDateTime() - Start.ToCOleDateTime();
	if (index == NEW_MSG_INDEX)
	{
		DWORD TextDirIndex = m_ctrlDirection.GetItemData(m_ctrlDirection.GetCurSel());
		if (TextDirIndex == S3S_EMERGENCY_TEXT_DIRECTION_NONE)
		{
			GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(TRUE);
			GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(FALSE);
			GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);	
		}
	}

    m_Date = Start.ToCOleDateTime();
    m_StartTime =  Start.ToCOleDateTime();
    
    CString strSize;
    strSize.Format(_T("%d"), m_Size);
    GetDlgItem(IDC_COMBO_SIZE)->SetWindowText(strSize);
    // update ctrl data
    UpdateData(FALSE);

    return TRUE;
}


void EmergencyMessageDlg::OnDtnDatetimechangeEmgmDuration(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    // TODO: Add your control notification handler code here
    // update ctrl data

    COleDateTime DurationTime;

    if (m_ctrlDuration.GetTime(DurationTime))
    {
        INT Hours = DurationTime.GetHour();
        INT Minites = DurationTime.GetMinute();

        if (Hours == 0 && Minites == 0)
        {
            COleDateTime toTime(1983,3,17,0,1,0);
            m_ctrlDuration.SetTime(toTime);
        }
    }


    *pResult = 0;
}


void EmergencyMessageDlg::OnCbnEditchangeComboSize()
{
    CString strFontSize;
    GetDlgItem(IDC_COMBO_SIZE)->GetWindowText(strFontSize);

    int nFontSize = StringUtility::stoi(strFontSize.GetString());
    if (nFontSize <= 0 || nFontSize > 500)
    {
        MessageBox(Translate(_T("Wrong font size")), Translate(_T("Error:Emergency message")), MB_OK|MB_ICONERROR);
        GetDlgItem(IDC_COMBO_SIZE)->SetWindowText(_T("24"));
    }
	UpdateDuration();
}


void EmergencyMessageDlg::OnBnClickedStop()
{
	CDialogEx::OnOK();
	// TODO: Add your control notification handler code here
	if (m_bInit)
	{
		return;
	}
	// update ctrl data
	UpdateData(TRUE);

	if (m_ctrlFont.GetSelFont())
	{
		//m_Font = m_ctrlFont.GetSelFont()->GetFullName();
		m_Font = m_ctrlFont.GetSelFont()->m_strName.GetString();
	}

	DWORD TextDirIndex = m_ctrlDirection.GetItemData(m_ctrlDirection.GetCurSel());
	if (TextDirIndex >=0 && TextDirIndex < S3S_EMERGENCY_TEXT_DIRECTION_TOTAL)
	{  
		m_strDirection = szEmergencyTextDirection[TextDirIndex];
	
	}

	CString strFontSize;
	GetDlgItem(IDC_COMBO_SIZE)->GetWindowText(strFontSize);


	m_Color = m_ColorControl.GetColor();

	//
	TCHAR buf[25];

	m_newEmessage.AddCustomizedSetting(_T("MessageContent"), _T(""));
	m_newEmessage.AddCustomizedSetting(_T("Direction"), m_strDirection.GetString());
	m_newEmessage.AddCustomizedSetting(_T("FontFamily"), m_Font.GetString());

	//     _itot_s(m_Size, buf, 10);
	m_newEmessage.AddCustomizedSetting(_T("FontSize"), strFontSize.GetString());

	_itot_s(m_Color, buf, 10);
	m_newEmessage.AddCustomizedSetting(_T("FontColor"), buf);

	CString T_F;
	T_F = m_bBold ? _T("TRUE") : _T("FALSE");
	m_newEmessage.AddCustomizedSetting(_T("Bold"), T_F.GetString());

	T_F = m_bItalic ? _T("TRUE") : _T("FALSE");
	m_newEmessage.AddCustomizedSetting(_T("Italic"), T_F.GetString());

	T_F = m_bUnderline ? _T("TRUE") : _T("FALSE");
	m_newEmessage.AddCustomizedSetting(_T("Underline"), T_F.GetString());

	_itot_s(m_Speed, buf, 10);
	m_newEmessage.AddCustomizedSetting(_T("Speed"), buf);

	_itot_s(m_Transparency, buf, 10);
	m_newEmessage.AddCustomizedSetting(_T("Transparency"), buf);

	COleDateTime Date = COleDateTime::GetCurrentTime();

	COleDateTime StartTime = COleDateTime(Date.GetYear(), Date.GetMonth(), Date.GetDay(),
		Date.GetHour(), Date.GetMinute(), Date.GetSecond());
	COleDateTimeSpan Duration = COleDateTimeSpan(0, m_Duration.GetHour(), 
		m_Duration.GetMinute(), m_Duration.GetSecond());
	m_newEmessage.AddStartTime(S3Time::CreateTime(StartTime));
	m_newEmessage.AddEndTime(S3Time::CreateTime(StartTime + Duration));

	return;
}


LRESULT EmergencyMessageDlg::CalculateDuration(COleDateTimeSpan &timeSpan, BOOL bEditFont)
{
	CFont FontInfo;
	LOGFONT logFont;
	int durationTime = 0;
	int iShowTimes = _ttoi(m_ShowTimes.GetBuffer());

	if (m_ctrlFont.GetSelFont() == NULL )
	{
		return S_FALSE;
	}

	if (iShowTimes < 0)
	{
		iShowTimes = 0;
	}
	
	if (m_MessageContent.GetLength() == 0)
	{
		timeSpan = 0;
		return S_OK;
	}

	CString strFontSize;

	if (bEditFont)
	{
		GetDlgItem(IDC_COMBO_SIZE)->GetWindowText(strFontSize);
	}
	else
	{
		m_ctrlFontSize.GetLBText(m_ctrlFontSize.GetCurSel(), strFontSize);
	}

	ZeroMemory(&logFont, sizeof(LOGFONT));
	wcscpy_s(logFont.lfFaceName, m_ctrlFont.GetSelFont()->m_strName.GetString());
	logFont.lfWeight    = m_bBold ? FW_BOLD : FW_NORMAL;
	logFont.lfItalic    = m_bItalic;
	logFont.lfUnderline = m_bUnderline;
	logFont.lfHeight    = _tstoi(strFontSize.GetString());
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH;

	FontInfo.CreateFontIndirect(&logFont);

	HDC  hDC       = CreateCompatibleDC( NULL );
	SIZE MessageSize;
	SelectObject(hDC, FontInfo.GetSafeHandle());
	
	GetTextExtentPoint32(hDC, m_MessageContent, m_MessageContent.GetLength(), &MessageSize);

	int iExtraWidth = 0;
	if (logFont.lfItalic == 1)
	{
		iExtraWidth = logFont.lfHeight/6 + 1;
	}

	float width = GetSystemMetrics(SM_CXSCREEN);
	durationTime = ceil(iShowTimes*(width + MessageSize.cx + iExtraWidth) / (float)m_Speed);
	//durationTime = m_ShowTimes * durationTime;

	int d   =   durationTime / (3600*24);
	int h   =   (durationTime - d * 3600 * 24) / 3600; 
	int m   =   (durationTime - d * 3600 * 24 - h * 3600) / 60; 
	int s   =   durationTime - d * 3600 * 24 - h * 3600 - m * 60 ; 

	COleDateTimeSpan span(d, h, m, s);

	timeSpan = span;
	return S_OK;
}


void EmergencyMessageDlg::OnSelchangeComboDirection()
{
	// TODO: Add your control notification handler code here
	DWORD TextDirIndex = m_ctrlDirection.GetItemData(m_ctrlDirection.GetCurSel());
	if (TextDirIndex == S3S_EMERGENCY_TEXT_DIRECTION_NONE)
	{  
		GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(TRUE);
		GetDlgItem(IDC_TIMES)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EMGM_DURATION)->EnableWindow(FALSE);
		GetDlgItem(IDC_TIMES)->EnableWindow(TRUE);
	}
}

void EmergencyMessageDlg::UpdateDuration(BOOL bEditFont)
{
	static CString Last_MessageContent;
	static int Last_Speed;
	static CString Last_ShowTimes;

	Last_ShowTimes      = m_ShowTimes;
	Last_MessageContent = m_MessageContent;
	Last_Speed          = m_Speed;

	UpdateData(TRUE);

	DWORD TextDirIndex = m_ctrlDirection.GetItemData(m_ctrlDirection.GetCurSel());
	if (TextDirIndex == S3S_EMERGENCY_TEXT_DIRECTION_NONE)
	{
		return;
	}

	int iShowTimes = _ttoi(m_ShowTimes.GetBuffer());
	if (iShowTimes < 1 )
	{
		m_ShowTimes = _T("1");
		MessageBox(Translate(_T("An integer between 1 and 9999 is required for this field.")), Translate(_T("Warning:Emergency message")), MB_OK|MB_ICONEXCLAMATION);
	}

	if (iShowTimes > 9999)
	{
		m_ShowTimes = _T("9999");
		MessageBox(Translate(_T("An integer between 1 and 9999 is required for this field.")), Translate(_T("Warning:Emergency message")),MB_OK|MB_ICONEXCLAMATION);
	}

	COleDateTimeSpan Duration;
	CalculateDuration(Duration, bEditFont);
	COleDateTimeSpan MaxDuration(1,0,0,0);
	if(Duration > MaxDuration)
	{
		MessageBox(Translate(_T("The duration of emergency message could not be longer than 24h.")), Translate(_T("Warning:Emergency message")),MB_OK|MB_ICONEXCLAMATION);
		m_MessageContent    = Last_MessageContent ;
		m_ShowTimes         = Last_ShowTimes;
		m_Speed             = Last_Speed;
		UpdateData(FALSE);
		return;
	}

	m_Duration = Duration;
	UpdateData(FALSE);
}


void EmergencyMessageDlg::OnEnChangeTimes()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*UpdateData(TRUE);

	int iShowTimes = _ttoi(m_ShowTimes.GetBuffer());
	if (iShowTimes < 1 )
	{
		m_ShowTimes = _T("1");
		AfxMessageBox(Translate(_T("Enter an integer between 1 and 9999.")));
	}

	if (iShowTimes > 9999)
	{
		m_ShowTimes = _T("9999");
		AfxMessageBox(Translate(_T("Enter an integer between 1 and 9999.")));
	}

	UpdateData(FALSE);*/

	UpdateDuration();
}


void EmergencyMessageDlg::OnEnChangeEmgmText()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateDuration();
}


void EmergencyMessageDlg::OnCbnSelchangeEmgmFont()
{
	// TODO: Add your control notification handler code here
	UpdateDuration();
}


void EmergencyMessageDlg::OnCbnSelchangeComboSize()
{
	// TODO: Add your control notification handler code here
	UpdateDuration(FALSE);
}


void EmergencyMessageDlg::OnBnClickedCheckBold()
{
	// TODO: Add your control notification handler code here
	UpdateDuration();
}


void EmergencyMessageDlg::OnBnClickedCheckItalic()
{
	// TODO: Add your control notification handler code here
	UpdateDuration();
}

//void EmergencyMessageDlg::OnNMReleasedcaptureSliderSpeed(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	UpdateDuration();
//	*pResult = 0;
//}


void EmergencyMessageDlg::OnBnClickedCheckUnderline()
{
	// TODO: Add your control notification handler code here
	UpdateDuration();
}

BOOL EmergencyMessageDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(GetFocus() == GetDlgItem(IDC_TIMES))
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			if (isdigit(pMsg->wParam))
			{
				
			}
			else if (pMsg->wParam >= VK_NUMPAD0 &&  pMsg->wParam <= VK_NUMPAD9)
			{

			}
			else if(pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DELETE 
				|| pMsg->wParam == VK_BACK || pMsg->wParam == VK_TAB)
			{
				
			}
			else
			{
				CWnd * curFocus = GetFocus();
				MessageBox(Translate(_T("Please input integer!")), Translate(_T("Warning:Emergency message")),MB_OK|MB_ICONEXCLAMATION);
				curFocus->SetFocus();
				return TRUE;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void EmergencyMessageDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    UpdateDuration();
    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
