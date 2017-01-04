// OldFileDetail.cpp : 实现文件
//

#include "stdafx.h"
#include "Controller.h"
#include "OldFileDetail.h"
#include "afxdialogex.h"
#include "MediaInfo/MediaInfo.h"
#include "Localization/Localization.h"

// COldFileDetail 对话框

IMPLEMENT_DYNAMIC(COldFileDetail, CDialogEx)

COldFileDetail::COldFileDetail(MediaInfoLib::MediaInfo& mi, CWnd* pParent /*=NULL*/)
	: CDialogEx(COldFileDetail::IDD, pParent), m_mi(mi)
{

}

COldFileDetail::~COldFileDetail()
{
}

void COldFileDetail::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
//     DDX_Control(pDX, IDC_STATIC_LOCATION, m_wndPropListLocation);
    DDX_Control(pDX, IDC_MFCPROPERTYGRID_DETAIL, m_wndPropList);
}


BEGIN_MESSAGE_MAP(COldFileDetail, CDialogEx)
END_MESSAGE_MAP()

BOOL COldFileDetail::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CRect rect;
    m_wndPropList.GetClientRect(rect);
    m_wndPropList.SetLeftColumnWidth(rect.Width()/2);

    m_wndPropList.EnableHeaderCtrl(FALSE);
    m_wndPropList.EnableDescriptionArea(TRUE);
    m_wndPropList.SetVSDotNetLook(TRUE);
    m_wndPropList.MarkModifiedProperties(TRUE);

    m_wndPropList.SetAlphabeticMode(FALSE);
    m_wndPropList.SetShowDragContext(FALSE);

    m_wndPropList.SetGroupNameFullWidth(TRUE);


    typedef struct {
        TCHAR*                  name;
        MediaInfoLib::stream_t  stream;
    } MediaInfo_t;
    static const MediaInfo_t MediaInfo[] = {
        { _T("General"),  MediaInfoLib::Stream_General },
        { _T("Video"),    MediaInfoLib::Stream_Video },
        { _T("Audio"),    MediaInfoLib::Stream_Audio },
        { _T("Text"),     MediaInfoLib::Stream_Text },
        { _T("Chapters"), MediaInfoLib::Stream_Chapters },
        { _T("Image"),    MediaInfoLib::Stream_Image },
        { _T("Menu"),     MediaInfoLib::Stream_Menu }
    };

    for (int mediaType = 0; mediaType < MediaInfoLib::Stream_Max; ++mediaType)
    {
        int paramCount = 0;
        std::auto_ptr<CMFCPropertyGridProperty> apGroup(new CMFCPropertyGridProperty(MediaInfo[mediaType].name));
        int n = m_mi.Count_Get(MediaInfoLib::Stream_General);
        for (int streamPos = 0; streamPos < n; ++streamPos)
        {
            CString streamName;
            streamName.Format(_T("Stream %d"), streamPos);
            CMFCPropertyGridProperty* stream = new CMFCPropertyGridProperty(streamName.GetString());

            int m = m_mi.Count_Get(MediaInfo[mediaType].stream, streamPos);
            for (int paramPos = 0; paramPos < m; ++paramPos)
            {
                CString name, value, desc;
                name  = m_mi.Get(MediaInfo[mediaType].stream, streamPos, paramPos, MediaInfoLib::Info_Name).c_str();
                value = m_mi.Get(MediaInfo[mediaType].stream, streamPos, paramPos, MediaInfoLib::Info_Text).c_str();
                desc  = m_mi.Get(MediaInfo[mediaType].stream, streamPos, paramPos, MediaInfoLib::Info_Info).c_str();
                if (!value.IsEmpty() && name != _T("Count") && name.Find(_T('/')) == -1 && value.Left(4) != _T("UTC ") &&
                    name != _T("StreamCount") && name != _T("StreamKind") && name != _T("StreamKindID"))
                {
                    CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(name.GetString(), value.GetString(), desc.GetString());
                    pProp->AllowEdit(FALSE);

                    stream->AddSubItem(pProp);
                    ++paramCount;
                }
            }
            apGroup->AddSubItem(stream);
        }
        if (paramCount > 0)
        {
            m_wndPropList.AddProperty(apGroup.release());
        }
    }

	Translate(this);
    return TRUE;


/*
    COleVariant var3DLook((short)VARIANT_FALSE, VT_BOOL);
    apGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), var3DLook, _T("Specifies the dialog's font will be nonbold and controls will have a 3D border")));

    CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
    pProp->AddOption(_T("None"));
    pProp->AddOption(_T("Thin"));
    pProp->AddOption(_T("Resizable"));
    pProp->AddOption(_T("Dialog Frame"));
    pProp->AllowEdit(FALSE);

    apGroup1->AddSubItem(pProp);
    apGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (COleVariant) _T("About NewControlsDemo"), _T("Specifies the text that will be displayed in the dialog's title bar")));
*/



#if 0
    std::auto_ptr<CMFCPropertyGridProperty> apGroup3(new CMFCPropertyGridProperty(_T("m_misc")));
    pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("IDD_ABOUT_BOX(dialog)"));
    pProp->Enable(FALSE);
    apGroup3->AddSubItem(pProp);

    CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), NULL, _T("Specifies the default dialog color"));
    pColorProp->EnableOtherButton(_T("Other..."));
    pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
    apGroup3->AddSubItem(pColorProp);

    static TCHAR BASED_CODE szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
    apGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the dialog icon")));

    apGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

    apGroup3->AddSubItem(new CMFCPropertyGridProperty(_T("Phone"), _T("(123) 123-12-12"), _T("Enter a phone number"), 0, _T(" ddd  ddd dd dd"), _T("(___) ___-__-__")));

    m_wndPropList.AddProperty(apGroup3.release());

    std::auto_ptr<CMFCPropertyGridProperty> apGroup4(new CMFCPropertyGridProperty(_T("Hierarchy")));

    CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
    apGroup4->AddSubItem(pGroup41);

    CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
    pGroup41->AddSubItem(pGroup411);

    pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (COleVariant) _T("Value 1"), _T("This is a description")));
    pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (COleVariant) _T("Value 2"), _T("This is a description")));
    pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (COleVariant) _T("Value 3"), _T("This is a description")));

    apGroup4->Expand(FALSE);
    m_wndPropList.AddProperty(apGroup4.release());
#endif // 0

    
}


// COldFileDetail 消息处理程序
