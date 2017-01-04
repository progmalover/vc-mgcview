#include "StdAfx.h"
#include "SchedulePackageHandler.h"
#include "Controller.h"
#include "CommonLib/base64.h"

SchedulePackageHandler::SchedulePackageHandler(void)
    : m_pCurDoc (NULL)
{
}


SchedulePackageHandler::~SchedulePackageHandler(void)
{
    if (m_pCurDoc)
    {
        delete m_pCurDoc;
    }
}

BOOL SchedulePackageHandler::ImportPackage(CString &GroupName, AppointmentArray& AppArray, AppointmentArray& DefaultAppArray,
    const CString& PackagePathName, ScheduleCtrl* pScheduleCtrl)
{
    CString PathName = PackagePathName;
    PathName.MakeLower();
    if (PathName.Right(SCHEDULEPACKAGE_POSTFIX_SIZE) != CString(SCHEDULEPACKAGE_POSTFIX))
    {
        return FALSE;
    }


    AppArray.RemoveAll();
    DefaultAppArray.RemoveAll();


    std::shared_ptr<FILE> fp(_tfopen(PathName.GetString(), _T("rb")),
        [](FILE* fp) {if (fp) fclose(fp);});
    if (!fp)
    {
		ShowXMessageBox(NULL,
				Translate(_T("Failed to open file: ")) + PathName,
				Translate(_T("Error:Import package")),
				MB_OK|MB_ICONERROR);

        //MessageBox(NULL,Translate(_T("Failed to open file: ")) + PathName, Translate(_T("Error:Import package")), MB_OK|MB_ICONERROR);
        return FALSE;
    }

    if (m_pCurDoc)
    {
        delete m_pCurDoc;
        m_pCurDoc = NULL;
    }

    m_pCurDoc = new TiXmlDocument;
    if (!m_pCurDoc->LoadFile(fp.get(), TIXML_ENCODING_UTF8))
    {
        return FALSE;
    }

    TiXmlElement* pSchedule = m_pCurDoc->FirstChildElement(CW2A(_T("Schedule"), CP_UTF8));
    if (NULL == pSchedule)
    {
        ASSERT(true);
        return FALSE;
    }

	TiXmlElement* pGroupName = pSchedule->FirstChildElement(CW2A(_T("GroupName"), CP_UTF8));
	if(NULL != pGroupName)
	{
		GroupName = CA2W(pGroupName->GetText(), CP_UTF8);
	}

    TiXmlElement* pEntry = pSchedule->FirstChildElement(CW2A(_T("DefaultLayout"), CP_UTF8));
    if(NULL != pEntry)
    {
        TiXmlElement* pID   = pEntry->FirstChildElement(CW2A(_T("ID"), CP_UTF8));
		TiXmlElement* pDesc = pEntry->FirstChildElement(CW2A(_T("Description"), CP_UTF8));
        TiXmlElement* pPath = pEntry->FirstChildElement(CW2A(_T("Path"), CP_UTF8));
        TiXmlElement* pST = pEntry->FirstChildElement(CW2A(_T("StartTime"), CP_UTF8));
        TiXmlElement* pET = pEntry->FirstChildElement(CW2A(_T("EndTime"), CP_UTF8));
        if (NULL == pID || NULL == pPath || NULL == pST || NULL == pET || NULL == pDesc)
        {
            ASSERT(true);
            return FALSE;
        }

		CString szDes = CA2W(pDesc->GetText(), CP_UTF8);
        CString STime = CA2W(pST->GetText(), CP_UTF8);
        CString ETime = CA2W(pET->GetText(), CP_UTF8);
        COleDateTime StartTime = S3Time::CreateTime(STime.GetBuffer(0)).ToCOleDateTime();
        COleDateTime EndTime = S3Time::CreateTime(ETime.GetBuffer(0)).ToCOleDateTime();
		Appointment* pApp = NULL;
        if (StartTime <= EndTime)
        {
            pApp = new Appointment();

            CString ID = CA2W(pID->GetText(), CP_UTF8);

            pApp->SetDetailID(_ttoi(ID));
            pApp->SetInterval(StartTime, EndTime);
			pApp->SetDescription(szDes);
            DefaultAppArray.Add(pApp);
        }

		ImportAppointmentThumbnail(pEntry, *pApp, pScheduleCtrl);

    }

    pEntry = pSchedule->FirstChildElement(CW2A(_T("Layout"), CP_UTF8));
    while(NULL != pEntry)
    {
        TiXmlElement* pID = pEntry->FirstChildElement(CW2A(_T("ID"), CP_UTF8));
        TiXmlElement* pType = pEntry->FirstChildElement(CW2A(_T("Type"), CP_UTF8));
		TiXmlElement* pDesc = pEntry->FirstChildElement(CW2A(_T("Description"), CP_UTF8));
        TiXmlElement* pST = pEntry->FirstChildElement(CW2A(_T("StartTime"), CP_UTF8));
        TiXmlElement* pET = pEntry->FirstChildElement(CW2A(_T("EndTime"), CP_UTF8));
        if (NULL == pID || NULL == pST || NULL == pET || NULL == pType || NULL == pDesc)
        {
            ASSERT(true);
            return FALSE;
        }

        Appointment* pApp = new Appointment();

        CString ID = CA2W(pID->GetText(), CP_UTF8);
        CString Type = CA2W(pType->GetText(), CP_UTF8);

        pApp->SetDetailID(_ttoi(ID));
        pApp->SetDetailType((Appointment::APPOINTMENT_TYPE)_ttoi(Type));

        CString STime = CA2W(pST->GetText(), CP_UTF8);
        CString ETime = CA2W(pET->GetText(), CP_UTF8);
        COleDateTime StartTime = S3Time::CreateTime(STime.GetBuffer(0)).ToCOleDateTime();
        COleDateTime EndTime = S3Time::CreateTime(ETime.GetBuffer(0)).ToCOleDateTime();
		CString szDes = CA2W(pDesc->GetText(), CP_UTF8);
		pApp->SetDescription(szDes);
        if (StartTime <= EndTime)
        {
            pApp->SetInterval(StartTime, EndTime);

            AppArray.Add(pApp);
        }
        else
        {
            delete pApp;
        }

		ImportAppointmentThumbnail(pEntry, *pApp, pScheduleCtrl);

        pEntry = pEntry->NextSiblingElement(CW2A(_T("Layout"), CP_UTF8));
    }

    return TRUE;
}

BOOL SchedulePackageHandler::ExportPackage(AppointmentArray& AppArray, AppointmentArray& DefaultAppArray,
    std::vector<Timetable> TimetableArray, ScheduleCtrl* pScheduleCtrl,
    const CString& PackagePathName, CSchedulerPage* pPage)
{
    USES_CONVERSION;

    CString PathName = PackagePathName;
    CString TempPathName = PackagePathName;
    TempPathName.MakeLower();
    if (TempPathName.Right(SCHEDULEPACKAGE_POSTFIX_SIZE) != CString(SCHEDULEPACKAGE_POSTFIX))
    {
        PathName += SCHEDULEPACKAGE_POSTFIX;
    }

    if (!m_pCurDoc || (m_pCurDoc && (PathName != m_CurrentPathName)))
    {
        if (m_pCurDoc)
        {
            delete m_pCurDoc;
        }

        m_pCurDoc = new TiXmlDocument;

        m_CurrentPathName = PathName;
    }

    TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","UTF-8","");
    if (NULL==pDeclaration)
    {
        return FALSE;
    }
    m_pCurDoc->LinkEndChild(pDeclaration);
    TiXmlElement *pRootEle = new TiXmlElement("Schedule");
    if (NULL==pRootEle)
    {
        return FALSE;
    }
    m_pCurDoc->LinkEndChild(pRootEle);

#ifdef STANDALONE_EDITION
	TiXmlElement *pGroupName = new TiXmlElement("GroupName");
	if (NULL==pGroupName)
	{
		return FALSE;
	}
	pRootEle->LinkEndChild(pGroupName);

	TiXmlText *pGroupNameValue;
	ClientGroup clientGroupInfo = pScheduleCtrl->GetScheduleGroup();
	if(clientGroupInfo.GetGroupID() == S3NET_DEFAULT_CLIENTGROUP_ID)
	{
		pGroupNameValue = new TiXmlText("Default");
	}
	else
	{
		pGroupNameValue = new TiXmlText(StringUtility::ConvertUnicodeToUTF8(clientGroupInfo.GetGroupName()).c_str());
	}

	pGroupName->LinkEndChild(pGroupNameValue);
#endif


    for (int i = 0; i < TimetableArray.size(); i++)
    {
        // layout node
        TiXmlElement *pTimeTable = new TiXmlElement("TimeTable");
        if (NULL==pTimeTable)
        {
            return FALSE;
        }

        pRootEle->LinkEndChild(pTimeTable);


        // ID
        TiXmlElement *pID = new TiXmlElement("ID");
        if (NULL==pID)
        {
            return FALSE;
        }
        pTimeTable->LinkEndChild(pID);
        // Set ID
        char ID[25];
        _itoa_s(TimetableArray[i].GetTimetableID(), ID, 10);
        TiXmlText *pIDValue = new TiXmlText(ID);
        pID->LinkEndChild(pIDValue);

        auto it = TimetableArray[i].LayoutEntryList.begin();
        while(it != TimetableArray[i].LayoutEntryList.end())
        {
            // layout node
            TiXmlElement *pTimeTableEntry = new TiXmlElement("TimeTableEntry");
            if (NULL==pTimeTableEntry)
            {
                return FALSE;
            }

            pTimeTable->LinkEndChild(pTimeTableEntry);

            // ID
            TiXmlElement *pID = new TiXmlElement("LayoutID");
            if (NULL==pID)
            {
                return FALSE;
            }
            pTimeTableEntry->LinkEndChild(pID);
            // Set ID
            char ID[25];
            _itoa_s(it->GetLayoutID(), ID, 10);
            TiXmlText *pIDValue = new TiXmlText(ID);
            pID->LinkEndChild(pIDValue);


             // Path
            TiXmlElement *pPath = new TiXmlElement("Path");
            if (NULL==pPath)
            {
                return FALSE;
            }
            pTimeTableEntry->LinkEndChild(pPath);
            // Set Path
            TiXmlText *pPathValue;

            if (pPage)
            {
                pPathValue= new TiXmlText(StringUtility::ConvertUnicodeToUTF8(pPage->GetLayoutExportLocation(it->GetLayoutID()).GetString()).c_str());
            }
            else
            {
                pPathValue= new TiXmlText("Where is the Layout?");
            }
            pPath->LinkEndChild(pPathValue);


            // ID
            TiXmlElement *pDuration = new TiXmlElement("Duration");
            if (NULL==pID)
            {
                return FALSE;
            }
            pTimeTableEntry->LinkEndChild(pDuration);
            // Set ID
            char Duration[25];
            _itoa_s(it->GetDuration(), Duration, 10);
            TiXmlText *pDurationValue = new TiXmlText(Duration);
            pDuration->LinkEndChild(pDurationValue);

            it++;
        }
    }


    if (DefaultAppArray.GetSize() > 0)
    {
        // layout node
        TiXmlElement *pLayout = new TiXmlElement("DefaultLayout");
        if (NULL==pLayout)
        {
            return FALSE;
        }

        pRootEle->LinkEndChild(pLayout);

        // ID
        TiXmlElement *pID = new TiXmlElement("ID");
        if (NULL==pID)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pID);
        // Set ID
        char ID[25];
        _itoa_s(DefaultAppArray[0]->GetDetailID(), ID, 10);
        TiXmlText *pIDValue = new TiXmlText(ID);
        pID->LinkEndChild(pIDValue);

		// Description
		TiXmlElement *pDescription = new TiXmlElement("Description");
		if (NULL==pDescription)
		{
			return FALSE;
		}
		pLayout->LinkEndChild(pDescription);

		// Set Description
		TiXmlText *pDescriptionValue = new TiXmlText(StringUtility::ConvertUnicodeToUTF8(DefaultAppArray[0]->GetDescription().GetString()).c_str());
		pDescription->LinkEndChild(pDescriptionValue);

        // Type
        TiXmlElement *pType = new TiXmlElement("Type");
        if (NULL==pType)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pType);
        // Set Type
        char Type[25];
        _itoa_s(DefaultAppArray[0]->GetDetailType(), Type, 10);
        TiXmlText *pTypeValue = new TiXmlText(Type);
        pType->LinkEndChild(pTypeValue);


        if(DefaultAppArray[0]->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
        {

            // Path
            TiXmlElement *pPath = new TiXmlElement("Path");
            if (NULL==pPath)
            {
                return FALSE;
            }
            pLayout->LinkEndChild(pPath);
            // Set Path
            TiXmlText *pPathValue;

            if (pPage)
            {
                pPathValue= new TiXmlText(StringUtility::ConvertUnicodeToUTF8(pPage->GetLayoutExportLocation(DefaultAppArray[0]->GetDetailID()).GetString()).c_str());
            }
            else
            {
                pPathValue= new TiXmlText("Where is the Layout?");
            }
            pPath->LinkEndChild(pPathValue);
			// Thumbnail
			ExportAppointmentThumbnail(DefaultAppArray[0]->GetDetailID(), pLayout);
        }


        // Start Time
        TiXmlElement *pST = new TiXmlElement("StartTime");
        if (NULL==pST)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pST);
        // Set Start Time
        TiXmlText *pSTValue = new TiXmlText(T2CA(S3Time::CreateTime(DefaultAppArray[0]->GetStart()).ToString().c_str()));
        pST->LinkEndChild(pSTValue);


        // End Time
        TiXmlElement *pET = new TiXmlElement("EndTime");
        if (NULL==pET)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pET);
        // Set End Time
        TiXmlText *pETValue = new TiXmlText(T2CA(S3Time::CreateTime(DefaultAppArray[0]->GetFinish()).ToString().c_str()));
        pET->LinkEndChild(pETValue);
    }

    for (int i = 0; i < AppArray.GetSize(); i++)
    {
        // layout node
        TiXmlElement *pLayout = new TiXmlElement("Layout");
        if (NULL==pLayout)
        {
            return FALSE;
        }

        pRootEle->LinkEndChild(pLayout);

        // ID
        TiXmlElement *pID = new TiXmlElement("ID");
        if (NULL==pID)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pID);
        // Set Type
        char ID[25];
        _itoa_s(AppArray[i]->GetDetailID(), ID, 10);
        TiXmlText *pIDValue = new TiXmlText(ID);
        pID->LinkEndChild(pIDValue);

		// Description
		TiXmlElement *pDescription = new TiXmlElement("Description");
		if (NULL==pDescription)
		{
			return FALSE;
		}
		pLayout->LinkEndChild(pDescription);

		// Set Description
		TiXmlText *pDescriptionValue = new TiXmlText(StringUtility::ConvertUnicodeToUTF8(AppArray[i]->GetDescription().GetString()).c_str());
		pDescription->LinkEndChild(pDescriptionValue);

        // Type
        TiXmlElement *pType = new TiXmlElement("Type");
        if (NULL==pType)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pType);
        // Set ID
        char Type[25];
        _itoa_s(AppArray[i]->GetDetailType(), Type, 10);
        TiXmlText *pTypeValue = new TiXmlText(Type);
        pType->LinkEndChild(pTypeValue);

        if(AppArray[i]->GetDetailType() == Appointment::APPOINTMENT_TYPE_LAYOUT)
        {
            // Path
            TiXmlElement *pPath = new TiXmlElement("Path");
            if (NULL==pPath)
            {
                return FALSE;
            }
            pLayout->LinkEndChild(pPath);
            // Set Path
            TiXmlText *pPathValue;

            if (pPage)
            {
                pPathValue= new TiXmlText(StringUtility::ConvertUnicodeToUTF8(pPage->GetLayoutExportLocation(AppArray[i]->GetDetailID()).GetString()).c_str());
            }
            else
            {
                pPathValue= new TiXmlText("Where is the Layout?");
            }
            pPath->LinkEndChild(pPathValue);

			// Thumbnail
			ExportAppointmentThumbnail(AppArray[i]->GetDetailID(), pLayout);
        }

        // Start Time
        TiXmlElement *pST = new TiXmlElement("StartTime");
        if (NULL==pST)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pST);
        // Set Start Time
        TiXmlText *pSTValue = new TiXmlText(T2CA(S3Time::CreateTime(AppArray[i]->GetStart()).ToString().c_str()));
        pST->LinkEndChild(pSTValue);


        // End Time
        TiXmlElement *pET = new TiXmlElement("EndTime");
        if (NULL==pET)
        {
            return FALSE;
        }
        pLayout->LinkEndChild(pET);
        // Set End Time
        TiXmlText *pETValue = new TiXmlText(T2CA(S3Time::CreateTime(AppArray[i]->GetFinish()).ToString().c_str()));
        pET->LinkEndChild(pETValue);
    }

    FILE* fp = _tfopen(PathName.GetString(), _T("w"));
    if (!fp)
    {
        return FALSE;
    }

    if (!m_pCurDoc->SaveFile(fp))
    {
        fclose(fp);
        return FALSE;
    }

    fclose(fp);
    return TRUE;
}

VOID SchedulePackageHandler::ExportAppointmentThumbnail(INT LayoutID, TiXmlElement *pLayout)
{
	Layout::thumbnail_t thumbnail;
	GetControllerInterface->GetLayoutThumbnail(LayoutID, thumbnail);
	if (thumbnail.size() > 0)
	{	
		TiXmlElement *pThumbnail = new TiXmlElement("Thumbnail");
		if (NULL==pThumbnail)
		{
			return;
		}
		pLayout->LinkEndChild(pThumbnail);

		// Set Thumbnail
		std::tstring encodedThumbnail;
		MagicView::CommonLib::base64::encode(thumbnail.begin(), thumbnail.end(), std::back_inserter(encodedThumbnail));

		TiXmlText *pThumbnailValue = new TiXmlText(CW2A(encodedThumbnail.c_str(), CP_UTF8));
		pThumbnail->LinkEndChild(pThumbnailValue);

	}
}

VOID SchedulePackageHandler::ImportAppointmentThumbnail(TiXmlElement* pEntry, Appointment &App, ScheduleCtrl* pScheduleCtrl)
{
	CImageList *ptScheduleItemIconList = pScheduleCtrl->GetScheduleItemIconList();
	CMap<UINT, UINT, UINT, UINT>  *ptIconDetailIDMapper = pScheduleCtrl->GetIconDetailIDMapper();

	TiXmlElement* pThumbnail = pEntry->FirstChildElement(CW2A(_T("Thumbnail"), CP_UTF8));
	if (NULL != pThumbnail )
	{
		std::vector<BYTE> thumbnail;
		INT Index;

		CString valueStr = CA2W(pThumbnail->GetText(), CP_UTF8);
		thumbnail.clear();
		MagicView::CommonLib::base64::decode(valueStr.GetString(), valueStr.GetString()+valueStr.GetLength(), std::back_inserter(thumbnail));

		HGLOBAL hGlobal  =  GlobalAlloc(GMEM_MOVEABLE, thumbnail.size() * sizeof(thumbnail[0]));
		void *  pData  =  GlobalLock(hGlobal);
		memcpy(pData, &thumbnail[0], thumbnail.size() * sizeof(thumbnail[0]));
		GlobalUnlock(hGlobal);

		IStream *  pStream  =  NULL;
		if  (CreateStreamOnHGlobal(hGlobal, TRUE,  & pStream)  ==  S_OK)
		{
			CImage image;
			if (SUCCEEDED(image.Load(pStream)))
			{
				CImage ThumbImage;
				ThumbImage.Create(THUMBNAIL_W, THUMBNAIL_H, 32);

				HDC hDstDC = NULL;
				BOOL bResult;

				// Obtain a DC to the destination image
				hDstDC = ThumbImage.GetDC();

				CDC  cDC;
				cDC.Attach(hDstDC);
				cDC.FloodFill(0, 0, RGB(123,456,789));
				cDC.Detach();

				// Perform the blit
				bResult = image.BitBlt( hDstDC, max(0, (THUMBNAIL_W - image.GetWidth()))/2,
					max(0, (THUMBNAIL_H - image.GetHeight()))/2,
					image.GetWidth(), image.GetHeight(), 0, 0 );

				// Release the destination DC
				ThumbImage.ReleaseDC();

				HBITMAP hBitmap=ThumbImage.Detach();
				CBitmap bmp;
				bmp.Attach(hBitmap);

				Index = ptScheduleItemIconList->Add(&bmp, RGB(123,456,789));
				pStream -> Release();
			}
			else
			{
				pStream -> Release();
				GlobalFree(hGlobal);
				return;
			}
	
		}

		GlobalFree(hGlobal);
		ptIconDetailIDMapper->SetAt(App.GetDetailID(), Index);
		App.SetImage(ptScheduleItemIconList, Index);
	}
}