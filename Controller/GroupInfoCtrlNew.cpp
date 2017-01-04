// CGroupInfoCtrlNew.cpp : implementation file
//

#include "stdafx.h"
#include "Controller.h"
#include "GroupInfoCtrlNew.h"
#include "afxdialogex.h"
#include "ControllerClient/PowerOffSchedule.h"

// CGroupInfoCtrlNew dialog

IMPLEMENT_DYNAMIC(CGroupInfoCtrlNew, CDialogEx)

CGroupInfoCtrlNew::CGroupInfoCtrlNew(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGroupInfoCtrlNew::IDD, pParent)
	, m_CurGroup (-1, _T(""), _T(""))
	, m_InitGroupID (-1)
	, m_bModified (FALSE)
{

}

CGroupInfoCtrlNew::~CGroupInfoCtrlNew()
{
}

void CGroupInfoCtrlNew::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_GNAME, m_SelectedGroupName);
	DDX_Text(pDX, IDC_EDIT_GDESC, m_SelectedGroupDesc);
	DDX_Slider(pDX, IDC_SLIDER_GROUP_VOLUME, m_volume);
	DDV_MinMaxInt(pDX, m_volume, 0, 100);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_FROM, m_startTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_TO, m_endTime);
	DDX_Check(pDX, IDC_CHECK_ALWAYS_WORK, m_bAlwaysWork);
}


BEGIN_MESSAGE_MAP(CGroupInfoCtrlNew, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGroupInfoCtrlNew::OnBnClickedOk)

	ON_EN_CHANGE(IDC_EDIT_GNAME, &CGroupInfoCtrlNew::OnEnChangeEditGname)
	ON_EN_CHANGE(IDC_EDIT_GDESC, &CGroupInfoCtrlNew::OnEnChangeEditGdesc)

	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_FROM, &CGroupInfoCtrlNew::OnDtnDatetimechangeDatetimepickerFrom)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_TO, &CGroupInfoCtrlNew::OnDtnDatetimechangeDatetimepickerTo)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_GROUP_VOLUME, &CGroupInfoCtrlNew::OnNMReleasedcaptureSliderGroupVolume)
	ON_BN_CLICKED(IDC_CHECK_ALWAYS_WORK, &CGroupInfoCtrlNew::OnBnClickedCheckAlwaysWork)

END_MESSAGE_MAP()


// CGroupInfoCtrlNew message handlers


void CGroupInfoCtrlNew::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();

	UpdateData(TRUE);
	
	m_CurGroup.SetGroupDesc(m_SelectedGroupDesc.GetString());
	m_CurGroup.SetGroupName(m_SelectedGroupName.GetString());
	m_CurGroup.SetVolume(m_volume);

	PowerOffSchedule powerOffSchedule;

	if (!m_bAlwaysWork)
	{
		S3Time startTime = S3Time::CreateTime(m_startTime);
		S3Time endTime   = S3Time::CreateTime(m_endTime);	
		powerOffSchedule.AddPowerOffDefine(std::make_pair(endTime, startTime));
	}	

	m_CurGroup.SetWorkTime(powerOffSchedule.ToXML());

	HRESULT hr = GetControllerInterface->EditClientGroup(m_CurGroup);
	if (!SUCCEEDED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_CONNECTION_INVALID))
		{
			GetControllerApp->OnControllerServerOffline();
		}
		else
		{
			TRACE0("Failed to Edit Client Group!\n");
		}
		MessageBox(Translate(_T("Failed to edit group info!")),Translate(_T("Error:Group info")), MB_OK|MB_ICONERROR);
		return ;
	}
	else
	{
		MessageBox(Translate(_T("Edit group info ok!")),Translate(_T("Tips:Group info")),MB_OK);
	}
}

void CGroupInfoCtrlNew::SetCurGroupID(int groupid) 
{
	m_InitGroupID = groupid;

	if(groupid == -1)
	{
		GetDlgItem( IDOK)->EnableWindow(FALSE);
		((CEdit *)GetDlgItem( IDC_EDIT_GNAME))->SetReadOnly(FALSE);
		((CEdit *)GetDlgItem( IDC_EDIT_GDESC))->SetReadOnly(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER_GROUP_VOLUME)->EnableWindow(FALSE);
		
	}
	else
	{
		GetDlgItem( IDOK)->EnableWindow(TRUE);
		((CEdit *)GetDlgItem( IDC_EDIT_GNAME))->SetReadOnly(TRUE);
		((CEdit *)GetDlgItem( IDC_EDIT_GDESC))->SetReadOnly(TRUE);
		GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER_GROUP_VOLUME)->EnableWindow(TRUE);
	}
}

BOOL CGroupInfoCtrlNew::SetCurGroup(const ClientGroup& group)
{
	m_CurGroup = group;

	m_SelectedGroupName = Translate(m_CurGroup.GetGroupName().c_str());
	m_SelectedGroupDesc = Translate(m_CurGroup.GetGroupDesc().c_str());
	m_volume            = m_CurGroup.GetVolume();

	m_bModified = FALSE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	((CEdit *)GetDlgItem( IDC_EDIT_GNAME))->SetReadOnly(FALSE);
	((CEdit *)GetDlgItem( IDC_EDIT_GDESC))->SetReadOnly(FALSE);

	UpdateData(FALSE);

	if (m_CurGroup.GetWorkTime() != _T(""))
	{
		std::string message = StringUtility::ConvertUnicodeToUTF8(m_CurGroup.GetWorkTime());

		TiXmlDocument xml;
		xml.Parse(reinterpret_cast<LPCSTR>(message.c_str()), NULL, TIXML_ENCODING_UTF8);

		TiXmlElement* pPowerOffSchedule = xml.RootElement();
		if (NULL == pPowerOffSchedule)
		{
			ASSERT(0);
			return FALSE;
		}

		PowerOffSchedule powerOffSchedule;
		if (!powerOffSchedule.LoadFromXML(pPowerOffSchedule))
		{
			ASSERT(0);
			return FALSE;
		}

		std::vector<std::pair<S3Time, S3Time>> poweroffDefine;
		powerOffSchedule.LoadPowerOffDefine(poweroffDefine);

		if (poweroffDefine.size() > 0)
		{
			S3Time endTime   = poweroffDefine[0].first;
			S3Time startTime = poweroffDefine[0].second;

			m_startTime  = startTime.ToCOleDateTime();
			m_endTime    = endTime.ToCOleDateTime();			
		}
		else
		{
			m_bAlwaysWork = TRUE;
			GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(FALSE);
			GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(FALSE);
		}

		UpdateData(FALSE);
	}   

	return TRUE;
}


void CGroupInfoCtrlNew::OnEnChangeEditGname()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

}


void CGroupInfoCtrlNew::OnEnChangeEditGdesc()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);
}


void CGroupInfoCtrlNew::OnDtnDatetimechangeDatetimepickerFrom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	*pResult = 0;
}


void CGroupInfoCtrlNew::OnDtnDatetimechangeDatetimepickerTo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	*pResult = 0;
}


void CGroupInfoCtrlNew::OnNMReleasedcaptureSliderGroupVolume(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	*pResult = 0;
}


void CGroupInfoCtrlNew::OnBnClickedCheckAlwaysWork()
{
	// TODO: Add your control notification handler code here
	m_bModified = TRUE;

	GetDlgItem( IDOK)->EnableWindow(m_bModified);

	UpdateData(TRUE);
	if (m_bAlwaysWork)
	{
		GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(FALSE);
		GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_DATETIMEPICKER_FROM)->EnableWindow(TRUE);
		GetDlgItem(IDC_DATETIMEPICKER_TO)->EnableWindow(TRUE);
	}
}
