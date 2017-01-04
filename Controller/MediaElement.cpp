#include "StdAfx.h"
#include "S3Types.h"
#include "LayoutSettings.h"
#include "XmlParserCommon.h"
#include "CommonLib/base64.h"
#include "MediaElement.h"
#include <strsafe.h>

static CString GetFileTitleFromFileName(CString FileName) 
{ 
	int Where; 
	Where = FileName.ReverseFind('\\'); 
	if (Where == -1) 
		Where = FileName.ReverseFind('/'); 
	CString FileTitle = FileName.Right(FileName.GetLength() - 1 - Where); 
	return FileTitle; 
} 

static CString GetFileTitleFromFileNameWithoutExt(CString FileName) 
{ 
	int Where; 
	Where = FileName.ReverseFind('\\'); 
	if (Where == -1) 
		Where = FileName.ReverseFind('/'); 
	CString FileTitle = FileName.Right(FileName.GetLength() - 1 - Where); 
	
	{ 
		int Which = FileTitle.ReverseFind('.'); 
		if (Which != -1) 
			FileTitle = FileTitle.Left(Which); 
	} 
	return FileTitle; 
} 

static CString GetFileExtensionFromFileName(CString FileName) 
{ 
	CString FileTitle = FileName;
	{ 
		int Which = FileTitle.ReverseFind('.'); 
		if (Which != -1) 
			FileTitle = FileTitle.Right(FileName.GetLength() - 1 - Which); 
	} 
	return FileTitle; 
}


MediaElement::MediaElement(void)
{
	m_nMediaID            = -1;
	m_MediaName           = _T("Media");
	m_MediaType           = _T("Movie");
	m_MediaFile           = _T("");
	m_MessageFile         = _T("");
	m_Position.SetRectEmpty();
	m_StartTime           = 0;
	m_Duration            = 30;
	m_RefreshInterval     = 600;
	m_LoopCount           = 0;
	m_VolumeCount         = 100;
	m_bKeepAspect         = FALSE;
	m_BGColor             = 0xFFFFFFFF;
	m_pTextSetting        = NULL;
	m_pDesigner2.reset();
	m_pDesigner.reset();
	m_bKeepAspect       = FALSE;
}



MediaElement::~MediaElement(void)
{
}

VOID MediaElement::LoadMedias(TiXmlElement* pContentElement, MagicView::CommonLib::CPluginManager2  &plugMgr)
{
	DWORD											PropertyCount = 0;
    RO_PROPERTY_INFO								*pPropertyInfo = NULL;
    PLUGIN_SUPPORTFILES								*pSupportFiles = NULL;
	CMap<CString, LPCTSTR, CString, CString&>		AttribList;

    TiXmlAttribute* pAttributeOfContent = pContentElement->FirstAttribute();

    m_MediaType             = CA2W(pContentElement->Value(), CP_UTF8);
    m_MediaFile             = _T("");

    CString szFileName;

    if (m_MediaType.CompareNoCase( szTypeName_Text ) == 0 )
    {
        m_pTextSetting.reset(new S3SIGNAGE_TEXT_SETTING);

    }
    else if (m_MediaType.CompareNoCase( szTypeName_Audio ) == 0)
    {
		//do nothing
    }
    else
    {
        QueryDesignerWithName(m_MediaType, plugMgr, m_pDesigner2, m_pDesigner);
    }

    BOOL bMediaLosed = FALSE;

    // parser content attribute
    while(pAttributeOfContent)
    {
        CString ValueStr = CA2W(pAttributeOfContent->Value(), CP_UTF8);
        CString NameStr = CA2W(pAttributeOfContent->Name(), CP_UTF8);

        if(NameStr.CompareNoCase(_T("MediaID")) == 0)
        {
            m_nMediaID = _tstoi(ValueStr);
			/*
            if (m_nMediaID >= 0)
            {
                std::vector<int> MediaIds;
                MediaIds.push_back(m_nMediaID);
                std::vector<Media> Medias;
                HRESULT hr = GetControllerInterface->GetMedias(MediaIds, Medias);
                if(FAILED(hr) || Medias.size() < 1)
                {
                    bMediaLosed = TRUE;
                }
                else
                {
                    m_MediaName = Medias[0].GetName().c_str();
                }
            }*/
        }
        else if(NameStr.CompareNoCase(_T("MediaName")) == 0)
        {
            //if (m_nMediaID < 0)
            {
                m_MediaName = ValueStr;
            }
        }
        else if(NameStr.CompareNoCase(_T("StartTime")) == 0)
        {
            m_StartTime = ConvertStr2Duration(ValueStr);
        }
        else if(NameStr.CompareNoCase(_T("Duration")) == 0)
        {
            m_Duration  = ConvertStr2Duration(ValueStr);
        }
		else if (NameStr.CompareNoCase( _T("RefreshInterval")) == 0)
		{
			m_RefreshInterval  = ConvertStr2Duration(ValueStr);
		}
        else if(NameStr.CompareNoCase( _T("XPos")) == 0)
        {
            m_Position.left = _tstoi(ValueStr);
        }
        else if(NameStr.CompareNoCase(_T("YPos")) == 0)
        {
            m_Position.top = _tstoi(ValueStr);
        }
        else if(NameStr.CompareNoCase( _T("Width")) == 0)
        {
            m_Position.right = _tstoi(ValueStr);
        }
        else if(NameStr.CompareNoCase( _T("Height")) == 0)
        {
            m_Position.bottom = _tstoi(ValueStr);
        }
        else if(NameStr.CompareNoCase( _T("BgColor")) == 0)
        {
            m_BGColor = _tcstoul(ValueStr, NULL, 16);
        }
        else if (NameStr.CompareNoCase( _T("Speed")) == 0)
        {
            m_pTextSetting->FontSpeed = _tstoi(ValueStr);
        }
        else if (NameStr.CompareNoCase( _T("Direction")) == 0)
        {
            m_pTextSetting->TextDirection = ValueStr;
        }
		else if (NameStr.CompareNoCase( _T("FontDirection")) == 0)
		{
			m_pTextSetting->FontDirection = ValueStr;
		}
		else if (NameStr.CompareNoCase( _T("Transparency")) == 0)
		{
			m_pTextSetting->Transparency = _tstoi(ValueStr);
		}
        else if (NameStr.CompareNoCase( _T("Layout")) == 0)
        {
            m_pTextSetting->TextLayout = ValueStr;
        }
        else if (NameStr.CompareNoCase( _T("Alignment")) == 0)
        {
            m_pTextSetting->Alignment = ValueStr;
        }
        else if (NameStr.CompareNoCase( _T("Volume")) == 0 )
        {
            m_VolumeCount = _tstoi(ValueStr);
        }
        else if (NameStr.CompareNoCase( _T("KeepAspect")) == 0 )
        {
            m_bKeepAspect = ConvertStr2BOOL(ValueStr);
            // for S3ImageViewer and S3Flash, they have this attribute also and need to be synchronized.
            if (m_MediaType.CompareNoCase(_T("S3ImageViewer")) == 0 ||
                m_MediaType.CompareNoCase(_T("S3FlashPlayer")) == 0)
            {
                AttribList[_T("KeepAspect")] = ValueStr;
            }
        }
        else if (NameStr.CompareNoCase( _T("Loop")) == 0 )
        {
            m_LoopCount = _tstoi(ValueStr);
        }
        else if(NameStr.CompareNoCase( _T("FileName")) == 0)
        {
            BOOL bURL = TRUE;
            szFileName  = ValueStr;

            m_MediaFile = szFileName;

/*
            if(bMediaLosed && m_nMediaID >= 0)
			{
				m_arrLosedFiles.Add(szFileName);
			}*/
        }
        else if(NameStr.CompareNoCase( _T("URL")) == 0)
        {
            NameStr = _T("FileName");
            m_MediaFile = ValueStr;
        }
#if 1		
		else if(NameStr.CompareNoCase(_T("MediaHeight")) == 0)
		{
			m_MediaHeight = _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase(_T("MediaWidth")) == 0)
		{
			m_MediaWidth = _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase(_T("MediaDuration")) == 0)
		{
			m_MediaDuration = ConvertStr2Duration(ValueStr);
		}
		else if(NameStr.CompareNoCase(_T("MediaSize")) == 0)
		{
			m_MediaSize =  _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase(_T("MediaVersion")) == 0)
		{
			m_MediaVersion =  _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase(_T("MediaDigest")) == 0)
		{
			m_MediaDigest = ValueStr;
		}/*
		else if(NameStr.CompareNoCase(_T("MediaType")) == 0)
		{
			m_MediaType = ValueStr;
		}*/
		else if(NameStr.CompareNoCase(_T("MediaSignature")) == 0)
		{
			m_MediaSignature = ValueStr;
		}
		else if(NameStr.CompareNoCase(_T("MediaDescription")) == 0)
		{
			m_MediaDescription = ValueStr;
		}
#endif		
	    else
        {
            AttribList[NameStr] = ValueStr;
        }


        pAttributeOfContent = pAttributeOfContent->Next();
    }

    m_Position.right += m_Position.left;
    m_Position.bottom += m_Position.top;

    // Load renderable obj's property settings
    RO_PROPERTY_SETTING PropertySetting;
    RO_PROPERTY        *pProperties = new RO_PROPERTY[AttribList.GetCount()];
    CString szKey;
    CString szValue;
    int nCount = 0;
    PropertySetting.propertyCount = AttribList.GetCount();
    PropertySetting.pProperties = pProperties;
    POSITION pos = AttribList.GetStartPosition();
    while (pos != NULL)
    {
        AttribList.GetNextAssoc(pos, szKey, szValue);
        StringCchCopy(pProperties[nCount].propertyName, 256, szKey);
        StringCchCopy(pProperties[nCount].propertyValue, 256, szValue);

        nCount ++;
    }

    if (m_pDesigner2)
    {
        m_pDesigner2->CreateDesigner(m_MediaFile, &PropertySetting);
    }
    else if (m_pDesigner)
    {
        m_pDesigner->CreateDesigner(m_MediaFile, &PropertySetting);
    }

    TiXmlElement* pSubContentElement = pContentElement->FirstChildElement();

    while(pSubContentElement)
    {
        CString SubContentName = CA2W(pSubContentElement->Value(), CP_UTF8);
        if(SubContentName.CompareNoCase( _T("TransitionIn")) == 0 )
        {
            ParserTranstion(&m_TransIn, pSubContentElement);
        }

        if(SubContentName.CompareNoCase( _T("TransitionOut")) == 0 )
        {
            ParserTranstion(&m_TransOut, pSubContentElement);
        }

        if (SubContentName.CompareNoCase( szTypeName_TextFile ) == 0 )
        {
            ParserTextFile(pSubContentElement);
        }

        pSubContentElement = pSubContentElement->NextSiblingElement();
    }

    AttribList.RemoveAll();
    if (pProperties)
    {
        delete[] pProperties;
        pProperties = NULL;
    }
}

HRESULT MediaElement::Save(TiXmlElement* pLayer, CString &dirPath, PROPERTY_TYPE propertyType, 
	                       CRect &rectObj, BOOL bExportToDir, DWORD &nMediaCount, DWORD &dwLastStartTime)
{
	DWORD					startTime		= 0;
    PRO_PROPERTY			pProperty		= NULL;
    DWORD					propertyCount	= 0;
    PLUGIN_SUPPORTFILES		*pSupportFiles  = NULL;
	char					szValue[MAX_PATH];


    if (m_MediaFile.GetLength() == 0)
    {
        return S_OK;
    }

    DWORD duration = m_Duration;
    if (nMediaCount == 0)
    {
        startTime = 0;
    }
    else
    {
        startTime = dwLastStartTime;
    }
    nMediaCount += 1;

    if (dwLastStartTime < startTime + duration)
    {
        dwLastStartTime += duration;
    }

	TiXmlElement* pContent;
	std::vector<int> mediaIDList;
	std::vector<Media> mediaList;
	mediaIDList.push_back(m_nMediaID);
	HRESULT hr = GetControllerInterface->GetMedias(mediaIDList, mediaList);
	if (FAILED(hr))
	{
		return hr;
	}

    CString szTempMediaFile = m_MediaFile;
    szTempMediaFile.Trim();
    szTempMediaFile.MakeLower();

    if(mediaList.size() > 0 && 
        mediaList[0].GetMediaType() == _T("URL")  &&
        (mediaList[0].GetDesc() == _T("mms") || mediaList[0].GetDesc() == _T("rtsp")|| 
        mediaList[0].GetDesc() == _T("rtsp://") || mediaList[0].GetDesc() == _T("mms://")) ||
        ((-1 != szTempMediaFile.Find(_T("http:")) ||  -1 != szTempMediaFile.Find(_T("https:"))) && -1 != szTempMediaFile.Find(_T(".nsc")))
#ifdef STARTER_EDITION                                             
        || -1 != m_MediaFile.Find(_T("mms://")) ||
        -1 != m_MediaName.Find(_T("mms://")) || 
        -1 != m_MediaFile.Find(_T("rtsp://")) || 
        -1 != m_MediaName.Find(_T("rtsp://")) 
#endif  
        )
	{
		pContent = new TiXmlElement(CW2A(szTypeName_Video, CP_UTF8));
	}
	else
	{
		pContent = new TiXmlElement(CW2A(m_MediaType, CP_UTF8));
	}
    if (pContent == NULL)
    {
        return E_UNEXPECTED;
    }
    pLayer->LinkEndChild(pContent);


    sprintf_s(szValue, "%d", m_nMediaID);
    pContent->SetAttribute("MediaID", szValue);
#ifdef STARTER_EDITION
	if(    -1 != m_MediaFile.Find(_T("mms://")) 
		|| -1 != m_MediaFile.Find(_T("rtsp://")) )
	{
		pContent->SetAttribute("MediaName", CW2A(m_MediaFile, CP_UTF8));
	}
	else
	{
		pContent->SetAttribute("MediaName", CW2A(m_MediaName, CP_UTF8));
	}

#else
    pContent->SetAttribute("MediaName", CW2A(m_MediaName, CP_UTF8));
#endif 

    pSupportFiles = NULL;

	if( TRUE == bExportToDir && m_nMediaID)
	{
#ifndef SERVER_EDITION
		CString originFileName = g_MediaServer().GetServerHomeDir().c_str() + m_MediaFile;
#else
		CString originFileName = GetMediaLibraryInterface->GetMediaLibraryRoot().c_str();
		originFileName	+=  m_MediaFile;
#endif
		CString dstFileName;
		CString dstFilePathName;
		dstFileName.Format(_T("%s-%d.%s"),GetFileTitleFromFileNameWithoutExt(m_MediaName),m_nMediaID, GetFileExtensionFromFileName(m_MediaName));
		dstFilePathName = _T("\\media\\") + dstFileName;
		
		CopyFile(originFileName,dirPath + dstFileName,FALSE);
		//pContent->SetAttribute("FileName", CW2A(pMedia->MediaFile, CP_UTF8));
		pContent->SetAttribute("FileName", CW2A(dstFilePathName, CP_UTF8));
	}
	else
	{
		pContent->SetAttribute("FileName", CW2A(m_MediaFile, CP_UTF8));
	}


    Xml_Parser_Common::ConvertDuration2Str(startTime, szValue);
    pContent->SetAttribute("StartTime", szValue);
    Xml_Parser_Common::ConvertDuration2Str(m_Duration, szValue);
    pContent->SetAttribute("Duration", szValue);

	if (m_MediaType.CompareNoCase(_T("S3WebBrowser")) == 0)
	{
		Xml_Parser_Common::ConvertDuration2Str(m_RefreshInterval, szValue);
		pContent->SetAttribute("RefreshInterval", szValue);
	}

    if (m_MediaType.CompareNoCase(szTypeName_EmptyContent) == 0)
    {
        return S_OK;
    }
    CString MediaLibraryFilePath = GetMediaLibraryInterface->GetMediaLibraryRoot().c_str() + m_MediaFile;

#ifndef SERVER_EDITION
    MediaLibraryFilePath = g_MediaServer().GetServerHomeDir().c_str() + m_MediaFile;
#endif
    // Check if the media file exists
    if (!(::PathFileExists(m_MediaFile) || ::PathFileExists(MediaLibraryFilePath))
        && propertyType != MESSAGE
        && (m_MediaType.CompareNoCase(_T("S3WebBrowser")) != 0))
    {
        if ((m_MediaType.CompareNoCase(szTypeName_Video) == 0)
            || (m_MediaType.CompareNoCase(szTypeName_Audio) == 0)
            || (GetControllerApp->FindPluginSupportFile(m_MediaType, pSupportFiles) != 0))
        {
            CString MediaFileName;
            vector<INT> MediaID;
            vector<Media> Medias;
            MediaID.push_back(m_nMediaID);
            HRESULT hr = GetControllerInterface->GetMedias(MediaID, Medias);
            if (SUCCEEDED(hr) && Medias.size() > 0)
            {
                MediaFileName = Medias[0].GetName().c_str();
            }
            else
            {
                MediaFileName = m_MediaFile;
            }

            TString SuspicionFile = GetMediaLibraryInterface->GetMediaLibraryRoot() +
                MediaLibrary::GetSuspicionFileName(m_MediaFile.GetString());
#ifndef SERVER_EDITION
            SuspicionFile = g_MediaServer().GetServerHomeDir().c_str()  +
                MediaLibrary::GetSuspicionFileName(m_MediaFile.GetString());
#endif
            if (::PathFileExists(SuspicionFile.c_str()))
            {
                //m_arrSuspicionFiles.Add(m_MediaName);
            }
            else
            {
                //m_arrLosedFiles.Add(m_MediaName);
            }
        }
    }

	CRect frectPosition = m_Position;
    if (!frectPosition.EqualRect(rectObj))
    {
        frectPosition.CopyRect(rectObj);
    }
    sprintf_s(szValue, "%d", frectPosition.left);
    pContent->SetAttribute("XPos", szValue);
    sprintf_s(szValue, "%d", frectPosition.top);
    pContent->SetAttribute("YPos", szValue);
    sprintf_s(szValue, "%d", frectPosition.Width());
    pContent->SetAttribute("Width", szValue);
    sprintf_s(szValue, "%d", frectPosition.Height());
    pContent->SetAttribute("Height", szValue);

    sprintf_s(szValue, "0x%08x", m_BGColor);
    pContent->SetAttribute("BgColor", szValue);

    Xml_Parser_Common::ConvertBOOL2Str( m_bKeepAspect, szValue);
    pContent->SetAttribute("KeepAspect", szValue);

    sprintf_s(szValue, "%d", m_VolumeCount);
    pContent->SetAttribute("Volume", szValue);
    sprintf_s(szValue, "%d", m_LoopCount);
    pContent->SetAttribute("Loop", szValue);
#if 1
	//--begin by RaymondLIu
	if(mediaList.size())
	{
		std::string strTmp;
		//Media mediaTemp = mediaList[0];
		sprintf_s(szValue, "%d", mediaList[0].GetHeight());
		pContent->SetAttribute("MediaHeight", szValue);

		sprintf_s(szValue, "%d", mediaList[0].GetWidth());
		pContent->SetAttribute("MediaWidth", szValue);

		Xml_Parser_Common::ConvertDuration2Str(mediaList[0].GetDuration(), szValue);
		pContent->SetAttribute("MediaDuration", szValue);

		strTmp = StringUtility::ConvertUnicodeToUTF8(mediaList[0].GetDigest());
		pContent->SetAttribute("MediaDigest", strTmp.c_str());

		strTmp = StringUtility::ConvertUnicodeToUTF8(mediaList[0].GetMediaType());
		pContent->SetAttribute("MediaType", strTmp.c_str() );

		strTmp = StringUtility::ConvertUnicodeToUTF8(mediaList[0].GetSignature());
		pContent->SetAttribute("MediaSignature", strTmp.c_str());

		//strTmp = StringUtility::ConvertUnicodeToUTF8(mediaList[0].GetDesc());
		//pContent->SetAttribute("MediaDescription", strTmp.c_str());

		sprintf_s(szValue, "%d", mediaList[0].GetFileSize());
		pContent->SetAttribute("MediaSize", szValue);

		sprintf_s(szValue, "%d", mediaList[0].GetVersion());
		pContent->SetAttribute("MediaVersion", szValue);
	}
	//--end by RaymondLiu
#endif
    // Write renderable obj's designer's property settings

    if (m_pDesigner2)
    {
        propertyCount = 0;
        pProperty = NULL;
        hr = m_pDesigner2->GetPropertySettings(&propertyCount, pProperty);
        if (SUCCEEDED(hr) && propertyCount > 0)
        {
            pProperty = new RO_PROPERTY[propertyCount];
            hr = m_pDesigner2->GetPropertySettings(&propertyCount, pProperty);
            if (SUCCEEDED(hr))
            {
                for (DWORD i = 0; i < propertyCount; i++)
                {
                    // for S3ImageViewer and S3Flash, we need to skip the KeepAspect argument because we use the media content's KeepAspect
                    if ((m_MediaType.CompareNoCase(_T("S3ImageViewer")) == 0 ||
                            m_MediaType.CompareNoCase(_T("S3FlashPlayer")) == 0) &&
                        CString(pProperty[i].propertyName).CompareNoCase(_T("KeepAspect")) == 0)
                    {
                        continue;
                    }
                    pContent->SetAttribute(CW2A(pProperty[i].propertyName), CW2A(pProperty[i].propertyValue));
                }
            }

            if (pProperty)
            {
                delete[] pProperty;
                pProperty = NULL;
            }
        }
    }
    else if (m_pDesigner)
    {
        propertyCount = 0;
        pProperty = NULL;

        hr = m_pDesigner->GetPropertySettings(&propertyCount, pProperty);
        if (SUCCEEDED(hr) && propertyCount > 0)
        {
            pProperty = new RO_PROPERTY[propertyCount];
            hr = m_pDesigner->GetPropertySettings(&propertyCount, pProperty);
            if (SUCCEEDED(hr))
            {
                for (DWORD i = 0; i < propertyCount; i++)
                {
                    pContent->SetAttribute(CW2A(pProperty[i].propertyName), CW2A(pProperty[i].propertyValue));
                }
            }

            if (pProperty)
            {
                delete[] pProperty;
                pProperty = NULL;
            }
        }
    }

    // Write text message property settings
    if (m_pTextSetting)
    {
        sprintf_s(szValue, "%d", m_pTextSetting->FontSpeed);
        pContent->SetAttribute("Speed", szValue);
        pContent->SetAttribute("Direction", CW2A(m_pTextSetting->TextDirection, CP_UTF8));
		pContent->SetAttribute("FontDirection", CW2A(m_pTextSetting->FontDirection, CP_UTF8));
        pContent->SetAttribute("Layout", CW2A(m_pTextSetting->TextLayout, CP_UTF8));
        pContent->SetAttribute("Alignment", CW2A(m_pTextSetting->Alignment, CP_UTF8));
					
		ZeroMemory(szValue, MAX_PATH);
		sprintf_s(szValue, "%d", m_pTextSetting->Transparency);
        pContent->SetAttribute("Transparency", szValue);

        if (m_pTextSetting->pFontInfo)
        {
			//split to multi-string by cr
			int index = -1, lastPosition = 0;
			CString szTmpMsg = m_pTextSetting->szMessage;
			vector<CString> strs;
			CString separate_str = _T("\x0D\x0A");
			while (-1 != (index = szTmpMsg.Find(separate_str, lastPosition)))
			{
				strs.push_back(szTmpMsg.Mid(lastPosition, index - lastPosition));
				lastPosition = index + separate_str.GetLength();
			}
			CString lastString = szTmpMsg.Mid(lastPosition);
			if (0 != lastString.GetLength())
			{
				strs.push_back(lastString);
			}

			for (vector<CString>:: iterator itt = strs.begin(); itt != strs.end(); itt++)
			{
				m_pTextSetting->szMultiLineMessage = *itt;
				TiXmlElement* pText = new TiXmlElement("TextFile");
				if (pText == NULL)
				{
					return E_UNEXPECTED;
				}
				pContent->LinkEndChild(pText);

				COleStreamFile  ImageStream;

				ImageStream.CreateMemoryStream(NULL);
				hr = GenerateTextFile(ImageStream.GetStream());
				if (FAILED(hr))
				{
					return hr;
				}

				std::vector<BYTE>    PngText;
				ImageStream.SeekToBegin();

				BYTE *pStreamBuf = new BYTE[ImageStream.GetLength()];

				int readsize = ImageStream.Read(pStreamBuf, ImageStream.GetLength());
				for(int i=0; i< readsize; i++)
				{
					PngText.push_back(pStreamBuf[i]);
				}
						
				delete [] pStreamBuf;

				std::tstring encodedThumbnail;
				MagicView::CommonLib::base64::encode(PngText.begin(), PngText.end(), std::back_inserter(encodedThumbnail));

				pText->SetAttribute("Content", CW2A(encodedThumbnail.c_str(), CP_UTF8));
		        pText->SetAttribute("Message", CW2A(m_pTextSetting->szMultiLineMessage, CP_UTF8));

				sprintf_s(szValue, "%.2f", m_pTextSetting->fScale);
				pText->SetAttribute("ScaleRate", szValue);

				LOGFONT logFont;
				m_pTextSetting->pFontInfo->GetLogFont(&logFont);
				pText->SetAttribute("Font", CW2A(logFont.lfFaceName, CP_UTF8));

				sprintf_s(szValue, "%d", logFont.lfHeight);
				pText->SetAttribute("FontSize", szValue);

				BOOL bBold = logFont.lfWeight == FW_BOLD ? TRUE : FALSE;
				Xml_Parser_Common::ConvertBOOL2Str(bBold, szValue);
				pText->SetAttribute("Bold", szValue);
				Xml_Parser_Common::ConvertBOOL2Str(logFont.lfItalic, szValue);
				pText->SetAttribute("Italic", szValue);
				Xml_Parser_Common::ConvertBOOL2Str(logFont.lfUnderline, szValue);
				pText->SetAttribute("Underline", szValue);

				sprintf_s(szValue, "0x%08x", m_pTextSetting->FontColor);
				pText->SetAttribute("FontColor", szValue);
			}
		}
    }

    // Write transition in/out settings
    if (m_TransIn.Duration > 0)
    {
        TiXmlElement* pTransition = new TiXmlElement("TransitionIn");
        if (pTransition == NULL)
        {
            return E_UNEXPECTED;
        }
        pContent->LinkEndChild(pTransition);

        pTransition->SetAttribute("Type", CW2A(m_TransIn.Name, CP_UTF8));
        pTransition->SetAttribute("Direction", CW2A(m_TransIn.Direction, CP_UTF8));
        Xml_Parser_Common::ConvertDuration2Str(m_TransIn.Duration, szValue);
        pTransition->SetAttribute("Duration", szValue);
    }
    if (m_TransOut.Duration > 0)
    {
        TiXmlElement* pTransition = new TiXmlElement("TransitionOut");
        if (pTransition == NULL)
        {
            return E_UNEXPECTED;
        }
        pContent->LinkEndChild(pTransition);

        pTransition->SetAttribute("Type", CW2A(m_TransOut.Name, CP_UTF8));
        pTransition->SetAttribute("Direction", CW2A(m_TransOut.Direction, CP_UTF8));
        Xml_Parser_Common::ConvertDuration2Str(m_TransOut.Duration, szValue);
        pTransition->SetAttribute("Duration", szValue);
    }

	return S_OK;
    
//end if
}



HRESULT MediaElement:: GenerateTextFile(IStream *pMemStream)
{
	HRESULT hr = S_OK;
	BOOL    bScale = FALSE;
	FLOAT   fScale = 1.0f;
	DWORD m_TextureWidth = 0;
	DWORD m_TextureHeight = 0;
	DWORD CharPixelLength = 0;
	DWORD MaxHeight = 0;
	DWORD MaxWidth = 0;
	DWORD TextureWidth = 0;
	DWORD TextureHeight = 0;
	DWORD LineNumber = 0;
	DWORD CurLineWidth = 0;
	LOGFONT logFont;
	LOGFONT textFont;
	CFont fontInfo;
	int  iLineWidth = 0;
	int  iExtraWidth = 0;
	//std::shared_ptr<S3SIGNAGE_TEXT_SETTING> pTextSetting = NULL;
	// measure character size
	HDC  hDC       = CreateCompatibleDC( NULL );

	if (m_pTextSetting == NULL || m_pTextSetting->pFontInfo == NULL)
	{
		TRACE0("No font info exist, can not generate the text image.\n");
		return E_FAIL;
	}
	
	CString Message = m_pTextSetting->szMultiLineMessage;

	m_pTextSetting->pFontInfo->GetLogFont(&logFont);

	int degree = 0;
	if (m_pTextSetting->FontDirection == szTextLayout[S3S_TEXT_LAYOUT_V])
	{
		degree = 1;
	}
	BOOL bDrawLine = FALSE;
	if (1 == degree && 1 == logFont.lfUnderline)
	{
		logFont.lfUnderline = 0;
		bDrawLine = TRUE;
	}

	memcpy(&textFont, &logFont, sizeof(LOGFONT));
	fontInfo.CreateFontIndirect(&textFont);

	// Player can not load the text texture larger than 8k,
	// so If the width or height is larger than 4096, do stretch, and write the scale float value to the layout xml file


	int iTextWidth = 0, iTextHeight = 0;
	int iMaxHeight = 0, iMaxWidth   = 0;

	XFORM xForm;
	ZeroMemory(&xForm, sizeof(XFORM));
	SetXForm(degree, &xForm);

	do
	{
		if (TextureWidth != 0 || TextureHeight != 0)
		{
			textFont.lfHeight /= 2;
			MaxWidth = 0;
			MaxHeight = 0;
			CurLineWidth = 0;
			CharPixelLength = 0;
			LineNumber = 0;

			// Change the font size, attach the data to the font object
			fontInfo.DeleteObject();
			fontInfo.CreateFontIndirect(&textFont);
		}
		SetMapMode(hDC, MM_TEXT | MM_ANISOTROPIC);
		SelectObject(hDC, fontInfo.GetSafeHandle());

		if (textFont.lfItalic == 1)
		{
			iExtraWidth = textFont.lfHeight/8 + 1;
		}

		iLineWidth = textFont.lfHeight/18 + 1;
		iTextWidth = 0;
		for (int i = 0; i < Message.GetLength(); i++)
		{
			CString tmp = CString(Message.GetAt(i));
			SIZE size;
			GetTextExtentPoint32(hDC, tmp, tmp.GetLength(), &size);
			RECT rect;
			rect.left = 0;
			rect.top  = 0;
			rect.right = size.cx;
			rect.bottom = size.cy;
			int textureWidth = abs((rect.right * xForm.eM11 + rect.bottom * xForm.eM21 ) - (rect.left * xForm.eM11 + rect.top *xForm.eM21));
			int textureHeight = abs((rect.right * xForm.eM12 + rect.bottom * xForm.eM22) - (rect.left * xForm.eM12 + rect.top * xForm.eM22));
			iTextWidth  += textureWidth;
			//iTextHeight += textureHeight;

			iMaxWidth  =  (iMaxWidth < textureWidth) ? textureWidth : iMaxWidth;
			iMaxHeight =  (iMaxHeight < textureHeight) ? textureHeight : iMaxHeight;
		}

		//SIZE MessageSize;
		//GetTextExtentPoint32(hDC, pMessage->pTextSetting->szMultiLineMessage, pMessage->pTextSetting->szMultiLineMessage.GetLength(), &MessageSize);

		if (degree == 1)
		{
			TextureWidth = iTextWidth;
			TextureHeight = iMaxHeight + iExtraWidth;
		}
		else
		{
			TextureWidth = iTextWidth + iExtraWidth;
			TextureHeight = iMaxHeight;
		}

		if (m_TextureWidth == 0 && m_TextureHeight == 0)
		{
			m_TextureWidth = TextureWidth;
			m_TextureHeight = TextureHeight;
			fScale = 1.0f;
		}
		else
		{
			fScale = (FLOAT)m_TextureWidth / (FLOAT)TextureWidth;
		}

		//} while (TextureWidth > 4096 || TextureHeight > 4096);
	} while( (TextureWidth + 4096 - 1)/4096 * TextureHeight > 4096);

	m_TextureWidth = TextureWidth;
	m_TextureHeight = TextureHeight;

	m_pTextSetting->fScale = fScale;

	if (m_TextureWidth == 0 || m_TextureHeight == 0)
	{
		m_TextureWidth = m_TextureHeight = 1;
	}

	// Prepare to create a bitmap
	DWORD*      pBitmapBits;
	BITMAPINFO  bmi;

	ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth       =  (int)m_TextureWidth;
	bmi.bmiHeader.biHeight      = -(int)( (bDrawLine == TRUE) ? (m_TextureHeight+iLineWidth*2+1) : m_TextureHeight );
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount    = 32;
	HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
		(void**)&pBitmapBits, NULL, 0 );

	SelectObject( hDC, hbmBitmap );

	RECT FullRect;
	FullRect.left = 0;
	FullRect.top =  0;
	FullRect.right = m_TextureWidth;
	FullRect.bottom = m_TextureHeight;

	HBRUSH hBruch = CreateSolidBrush(0);
	FillRect(hDC, &FullRect, hBruch);
	DeleteObject( hBruch );
	ZeroMemory(&FullRect, sizeof(RECT));

	SelectObject(hDC, fontInfo.GetSafeHandle());
	SetMapMode(hDC, MM_TEXT | MM_ANISOTROPIC);
	// Set text properties
	SetTextColor( hDC, 0xFFFFFF );
	SetBkColor(   hDC, 0 );
	//SetTextAlign( hDC, TA_TOP );
	SetBkMode(hDC, TRANSPARENT);

	POINT LeftPoint;
	ZeroMemory(&LeftPoint, sizeof(POINT));

	int lastCharWidth = 0;
	XFORM  LeftMoveForm;
	XFORM  RightMoveForm;
	int oldGraphicsMode = SetGraphicsMode(hDC, GM_ADVANCED);
	//int oldGraphicsMode = SetGraphicsMode(hDC, GM_ADVANCED);
	for (int i = 0; i < Message.GetLength(); i++)
	{
		CString tmp = CString(Message.GetAt(i));

		SIZE size;
		GetTextExtentPoint32(hDC, tmp, tmp.GetLength(), &size);

		RECT rect;
		int textureWidth, textureHeight;
		//move left-top point to origin point
		switch (degree)
		{
		case 1:
			rect.left = 0;
			rect.top  = 0;
			rect.right  = (size.cx > iMaxHeight)? iMaxHeight : size.cx;
			rect.bottom = (size.cy > iMaxWidth) ? iMaxWidth: size.cy;

			textureWidth = abs((rect.right * xForm.eM11 + rect.bottom * xForm.eM21 ) - (rect.left * xForm.eM11 + rect.top *xForm.eM21));
			textureHeight = abs((rect.right * xForm.eM12 + rect.bottom * xForm.eM22) - (rect.left * xForm.eM12 + rect.top * xForm.eM22));

			//degree = 270;
			LeftMoveForm.eM11 = (FLOAT) 1; 
			LeftMoveForm.eM12 = (FLOAT) 0; 
			LeftMoveForm.eM21 = (FLOAT) 0; 
			LeftMoveForm.eM22 = (FLOAT) 1; 
			LeftMoveForm.eDx  = (FLOAT) -LeftPoint.x; 
			LeftMoveForm.eDy  = (FLOAT) 0;

			RightMoveForm.eM11 = (FLOAT) 1; 
			RightMoveForm.eM12 = (FLOAT) 0; 
			RightMoveForm.eM21 = (FLOAT) 0; 
			RightMoveForm.eM22 = (FLOAT) 1; 
			RightMoveForm.eDx  = (FLOAT) lastCharWidth; 
			RightMoveForm.eDy  = (FLOAT) ( ((iMaxHeight > rect.right) ? (rect.right + (iMaxHeight - rect.right)/2) : iMaxHeight) + iExtraWidth);

			LeftPoint.x  += rect.right;
			lastCharWidth += textureWidth;
			break;

		case 2:
			//degree = 180;
			break;
		case 3:
			//degree = 90;
			break;
		default:
			rect.left = 0;
			rect.top  = 0;
			rect.right  = size.cx;
			rect.bottom = size.cy;
			LeftMoveForm.eM11 = (FLOAT) 1; 
			LeftMoveForm.eM12 = (FLOAT) 0; 
			LeftMoveForm.eM21 = (FLOAT) 0; 
			LeftMoveForm.eM22 = (FLOAT) 1; 
			LeftMoveForm.eDx  = (FLOAT) 0; 
			LeftMoveForm.eDy  = (FLOAT) 0;
			RightMoveForm.eM11 = (FLOAT) 1; 
			RightMoveForm.eM12 = (FLOAT) 0; 
			RightMoveForm.eM21 = (FLOAT) 0; 
			RightMoveForm.eM22 = (FLOAT) 1; 
			RightMoveForm.eDx  = (FLOAT) 0; 
			RightMoveForm.eDy  = (FLOAT) 0;
			//degree = 0;
			break;
		}

		//FullRect.left = 0;
		FullRect.top  = 0;
		FullRect.right =  FullRect.left + rect.right;
		FullRect.bottom = rect.bottom + iExtraWidth;

		SetWorldTransform(hDC, &xForm);
		ModifyWorldTransform(hDC, &LeftMoveForm, MWT_LEFTMULTIPLY);
		ModifyWorldTransform(hDC, &RightMoveForm, MWT_RIGHTMULTIPLY);

		DrawText(hDC, tmp, tmp.GetLength(), &FullRect, DT_CENTER | DT_NOCLIP);

		FullRect.left += rect.right;

	}


	xForm.eM11 = (FLOAT) 1; 
	xForm.eM12 = (FLOAT) 0; 
	xForm.eM21 = (FLOAT) 0; 
	xForm.eM22 = (FLOAT) 1; 
	xForm.eDx  = (FLOAT) 0;
	xForm.eDy  = (FLOAT) 0;

	SetWorldTransform(hDC, &xForm); 
	SetGraphicsMode(hDC, oldGraphicsMode);	

	if(bDrawLine)
	{
		HPEN hPen = CreatePen(PS_SOLID, iLineWidth, RGB(255,255,255));
		SelectObject(hDC, hPen);
		MoveToEx(hDC, 0, RightMoveForm.eDy+1, NULL);
		LineTo(hDC, m_TextureWidth, RightMoveForm.eDy+1);
		m_TextureHeight = m_TextureHeight+iLineWidth*2+1;
	}
	//set m_TextureHeight = total map size

	// DrawText(hDC, pMessage->pTextSetting->szMultiLineMessage, pMessage->pTextSetting->szMultiLineMessage.GetLength(), &FullRect, DT_CENTER | DT_NOPREFIX);

	//    TCHAR PrintStr[2] = _T("x");
	//    PrintStr[0] = pCharInfo->Char;
	//    ExtTextOut( hDC, pCharInfo->XPos, pCharInfo->YPos, ETO_OPAQUE, NULL, PrintStr, 1, NULL );
	DWORD ColorConvertTable[256];

	DWORD FontB = (m_pTextSetting->FontColor & 0xFF0000) >> 16;
	DWORD FontG = (m_pTextSetting->FontColor & 0xFF00) >> 8;
	DWORD FontR = (m_pTextSetting->FontColor & 0xFF);

	DWORD BackA = ( m_BGColor & 0xFF000000) >> 24;
	DWORD BackR = (m_BGColor & 0xFF0000) >> 16;
	DWORD BackG = (m_BGColor & 0xFF00) >> 8;
	DWORD BackB = (m_BGColor & 0xFF);

	BackA = m_pTextSetting->Transparency * 255 / 100;


	for(int i=0; i< 256; i++)
	{
		DWORD BlendA = (i * 255 + (255 - i) * BackA + 127) / 255;
		DWORD BlendR = (i * FontR + (255 - i) * BackR + 127) / 255;
		DWORD BlendG = (i * FontG + (255 - i) * BackG + 127) / 255;
		DWORD BlendB = (i * FontB + (255 - i) * BackB + 127) / 255;

		ColorConvertTable[i] = (DWORD)((BlendA<<24) | (BlendR<<16) | (BlendG<<8) | BlendB);
	}


	BYTE bAlpha; // 4-bit measure of pixel intensity
	DWORD* pDst32;
	UINT nBitCount = 32;
	BYTE* pTextBitmapBits = new BYTE[m_TextureWidth * m_TextureHeight * nBitCount / 8];
	pDst32 = (DWORD*)pTextBitmapBits;

	CImage imageText;
	CImage imageDst;
	imageDst.Create(m_TextureWidth, m_TextureHeight, nBitCount, CImage::createAlphaChannel);

	for(DWORD y = 0; y < m_TextureHeight; y++ )
	{
		for(DWORD x=0; x < m_TextureWidth; x++ )
		{
			bAlpha = (BYTE)((pBitmapBits[m_TextureWidth * y + x] & 0xff));
			*pDst32++ = ColorConvertTable[bAlpha];
		}
	}

	CBitmap bitmapText;
	BOOL bBitmap = FALSE;
	bBitmap = bitmapText.CreateBitmap(m_TextureWidth, m_TextureHeight, 1, nBitCount, (void*)pTextBitmapBits);
	ASSERT(bBitmap == TRUE);

	imageText.Attach(bitmapText);
	HDC hDstDC = imageDst.GetDC();
	imageText.Draw(hDstDC, 0, 0);

	hr = imageDst.Save(pMemStream, Gdiplus::ImageFormatPNG);


	imageDst.ReleaseDC();
	imageDst.Destroy();
	imageText.Destroy();

	if (pTextBitmapBits)
	{
		delete [] pTextBitmapBits;
		pTextBitmapBits = NULL;
	}

	fontInfo.Detach();
	fontInfo.DeleteObject();
	DeleteObject( hbmBitmap );
	DeleteDC( hDC );

	return S_OK;
}

void MediaElement:: SetXForm(INT degree, XFORM *ptXForm)
{
	switch (degree)
	{
	case 1:
		//degree = 270;
		ptXForm->eM11 = (FLOAT) 0; 
		ptXForm->eM12 = (FLOAT) -1; 
		ptXForm->eM21 = (FLOAT) 1; 
		ptXForm->eM22 = (FLOAT) 0; 
		break;
	case 2:
		//degree = 180;
		ptXForm->eM11 = (FLOAT) -1; 
		ptXForm->eM12 = (FLOAT) 0; 
		ptXForm->eM21 = (FLOAT) 0; 
		ptXForm->eM22 = (FLOAT) -1; 
		break;
	case 3:
		//degree = 90;
		ptXForm->eM11 = (FLOAT) 0; 
		ptXForm->eM12 = (FLOAT) 1; 
		ptXForm->eM21 = (FLOAT) -1; 
		ptXForm->eM22 = (FLOAT) 0; 
		break;
	default:
		//degree = 0;
		ptXForm->eM11 = (FLOAT) 1; 
		ptXForm->eM12 = (FLOAT) 0; 
		ptXForm->eM21 = (FLOAT) 0; 
		ptXForm->eM22 = (FLOAT) 1; 
		break;
	}

}


HRESULT MediaElement:: ParserTextFile(TiXmlElement *pTextFileElement)
{
	HRESULT hr = S_OK;

	LOGFONT logFont;
	logFont.lfHeight            = 20;
	logFont.lfWidth             = 0;
	logFont.lfEscapement        = 0;
	logFont.lfOrientation       = 0;
	logFont.lfWeight            = FW_NORMAL;
	logFont.lfItalic            = 0;
	logFont.lfUnderline         = 0;
	logFont.lfStrikeOut         = 0;
	logFont.lfCharSet           = DEFAULT_CHARSET;
	logFont.lfOutPrecision      = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision     = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality           = ANTIALIASED_QUALITY;
	logFont.lfPitchAndFamily    = VARIABLE_PITCH;
	_tcscpy_s(logFont.lfFaceName, LF_FACESIZE, _T("Arial"));

	TiXmlAttribute* pAttributeOfTransition = pTextFileElement->FirstAttribute();

	// parser canvas attribute
	while(pAttributeOfTransition)
	{
		CString ValueStr = CA2W(pAttributeOfTransition->Value(), CP_UTF8);
		CString NameStr = CA2W(pAttributeOfTransition->Name(), CP_UTF8);

		if(NameStr.CompareNoCase( _T("Message")) == 0)
		{
			if (!m_pTextSetting->szMessage.IsEmpty())
			{
				m_pTextSetting->szMessage += _T("\r\n") + ValueStr;
			}
			else
			{
				m_pTextSetting->szMessage = ValueStr;
			}
		}
		else if (NameStr.CompareNoCase( _T("Font")) == 0 )
		{
			_tcscpy_s(logFont.lfFaceName, LF_FACESIZE, ValueStr);
		}
		else if (NameStr.CompareNoCase( _T("FontSize")) == 0)
		{
			logFont.lfHeight = _tstoi(ValueStr);
		}
		else if (NameStr.CompareNoCase( _T("Italic")) == 0)
		{
			logFont.lfItalic = ConvertStr2BOOL(ValueStr);
		}
		else if (NameStr.CompareNoCase( _T("Underline")) == 0)
		{
			logFont.lfUnderline = ConvertStr2BOOL(ValueStr);
		}
		else if (NameStr.CompareNoCase( _T("Bold")) == 0)
		{
			BOOL bBold = ConvertStr2BOOL(ValueStr);
			if (bBold)
			{
				logFont.lfWeight = FW_BOLD;
			}
			else
			{
				logFont.lfWeight = FW_NORMAL;
			}
		}
		else if (NameStr.CompareNoCase( _T("FontColor")) == 0)
		{
			m_pTextSetting->FontColor = _tcstoul(ValueStr, NULL, 16);
		}

		pAttributeOfTransition = pAttributeOfTransition->Next();
	}

	m_pTextSetting->pFontInfo->Detach();
	m_pTextSetting->pFontInfo->CreateFontIndirect(&logFont);

	return hr;
}


HRESULT MediaElement::ParserTranstion(S3SIGNAGE_TRANSIION *pTransition, TiXmlElement *pTransitionElement)
{
	HRESULT hr = S_OK;

	TiXmlAttribute* pAttributeOfTransition = pTransitionElement->FirstAttribute();

	pTransition->Duration   = 0;
	pTransition->Name       = _T("");
	pTransition->Direction  = _T("");
	// parser canvas attribute
	while(pAttributeOfTransition)
	{
		CString ValueStr = CA2W(pAttributeOfTransition->Value(), CP_UTF8);
		CString NameStr = CA2W(pAttributeOfTransition->Name(), CP_UTF8);

		if(NameStr.CompareNoCase( _T("Type")) == 0)   pTransition->Name = ValueStr;
		if(NameStr.CompareNoCase( _T("Duration")) == 0)   pTransition->Duration = ConvertStr2Duration(ValueStr);
		if(NameStr.CompareNoCase( _T("Direction")) == 0)   pTransition->Direction = ValueStr;

		pAttributeOfTransition = pAttributeOfTransition->Next();
	}

	return hr;
}


HRESULT MediaElement::QueryDesignerWithName(CString szName,  MagicView::CommonLib::CPluginManager2  &plugMgr,
	std::shared_ptr<IS3RODesigner2>& pDesigner2, std::shared_ptr<IS3RODesigner>& pDesigner)
{
    HRESULT hr = S_OK;
    std::shared_ptr<IS3RODesigner2> pPluginDesigner2    = NULL;
    std::shared_ptr<IS3RODesigner>  pPluginDesigner     = NULL;
    //std::shared_ptr<IS3RenderableObject> pPluginObj = GetControllerApp->FindObj(szName);
    std::shared_ptr<IS3RenderableObject> pPluginObj = plugMgr.CreateObject(szName.GetString());
    if (!pPluginObj)
    {
        hr = E_FAIL;
    }
    else
    {
        //
        pPluginDesigner2 = std::dynamic_pointer_cast<IS3RODesigner2>(pPluginObj);
        if (!pPluginDesigner2)
        {
            pPluginDesigner = std::dynamic_pointer_cast<IS3RODesigner>(pPluginObj);
            if (!pPluginDesigner)
            {
                return E_FAIL;
            }
        }
    }


    pDesigner2   = pPluginDesigner2;
    pDesigner    = pPluginDesigner;

    return hr;
}