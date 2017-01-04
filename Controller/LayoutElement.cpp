#include "StdAfx.h"
#include "LayoutElement.h"
#include "XmlParserCommon.h"

DWORD LayoutElement::CurrentLayerId = 0;	

LayoutElement::LayoutElement(void)
{
	m_LayoutName          = TranslateString(_T("Default"));
	COleDateTime createLayoutTime = COleDateTime::GetCurrentTime();
	CString tmp(createLayoutTime.Format(_T("_%Y%m%d_%H%M%S")));
	m_LayoutName	     += tmp;
	m_LayoutDuration      = 0;
	m_GroupID             = 1;
	m_ClientID            = 0;
	m_LayoutSize.SetSize(1920, 1080);
	m_bKeepAspect         = FALSE;
	m_BGColor             = 0x00000000;
	m_LayoutFile          = _T("");
	m_MediaContentSize    = 0;
	m_MediaCount          = 0;
    m_fRotateDegree       = 0.0f;
}


LayoutElement::~LayoutElement(void)
{
}

HRESULT LayoutElement::LoadLayout(CString Filename, MagicView::CommonLib::CPluginManager2  &plugMgr, const CString& LayoutName, CString ContentXML)
{
	HRESULT hr = S_OK;
	TiXmlDocument tDoc;

	if(Filename != _T(""))
	{
		std::shared_ptr<FILE> fp(_tfopen(Filename.GetString(), _T("rb")), 
			[](FILE *fp){if (fp){ fclose(fp);}});
		//FILE* fp = _tfopen(Filename.GetString(), _T("rb"));
		if(!fp || !tDoc.LoadFile(fp.get(), TIXML_ENCODING_UTF8))
		{
			//fclose(fp);
			TRACE1("Error: Can not open playlist file %s", Filename);
			return E_UNEXPECTED;
		}
		//fclose(fp);
	}else if (ContentXML != _T(""))
	{
		std::string xml = StringUtility::ConvertUnicodeToUTF8(ContentXML.GetBuffer(0));
		tDoc.Parse(reinterpret_cast<LPCSTR>(xml.c_str()), NULL, TIXML_ENCODING_UTF8);
	}else
	{
		return E_UNEXPECTED;
	}

	//ResetLayout();

	m_LayoutFile = Filename;

	TiXmlElement* pCanvasElement = tDoc.RootElement();
	ASSERT(CString(_T("Canvas")).CompareNoCase(CA2W(pCanvasElement->Value(), CP_UTF8)) == 0);
	TiXmlAttribute* pAttributeOfCanvas = pCanvasElement->FirstAttribute();

	// parser canvas attribute
	while(pAttributeOfCanvas)
	{
		CString ValueStr = CA2W(pAttributeOfCanvas->Value(), CP_UTF8);
		CString NameStr = CA2W(pAttributeOfCanvas->Name(), CP_UTF8);

		if(NameStr.CompareNoCase( _T("Width")) == 0) 
		{
			m_LayoutSize.cx = _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase( _T("Height")) == 0)   
		{
			m_LayoutSize.cy = _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase( _T("BgColor")) == 0)
		{
			m_BGColor = _tcstoul(ValueStr, NULL, 16);
			m_BGColor = (GetRValue(m_BGColor) << 16) | (GetGValue(m_BGColor) << 8) | GetBValue(m_BGColor);
		}
		else if(NameStr.CompareNoCase( _T("KeepAspect")) == 0 ) 
		{
			m_bKeepAspect = ConvertStr2BOOL(ValueStr);
		}
		else if(NameStr.CompareNoCase( _T("Name")) == 0 )
		{
			if (!LayoutName.IsEmpty())
			{
				m_LayoutName = LayoutName;
			}
			else
			{
				m_LayoutName = ValueStr;
			}
		}
		else if(NameStr.CompareNoCase( _T("Description")) == 0 ) 
		{
			m_Description = ValueStr;
		}
		else if(NameStr.CompareNoCase( _T("GroupID")) == 0 ) 
		{
			m_GroupID = _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase( _T("ClientID")) == 0 ) 
		{
			m_ClientID = _tstoi(ValueStr);
		}
		else if(NameStr.CompareNoCase( _T("Duration")) == 0 )   
		{
			m_LayoutDuration = ConvertStr2Duration(ValueStr);
		}

		pAttributeOfCanvas = pAttributeOfCanvas->Next();
	}

	//NewLayout(m_pLayoutSetting->LayoutSize.cx, m_pLayoutSetting->LayoutSize.cy);
	UINT m_nDepth = 0;
	TiXmlElement* pLayerElement = pCanvasElement->FirstChildElement();
	while(pLayerElement)
	{
		if(CString(_T("Layer")).CompareNoCase(CA2W(pLayerElement->Value(), CP_UTF8)) == 0)
		{
			std::shared_ptr<LayerElement> shpLayer = CreateLayer();
            AddHeadLayer(shpLayer);
			shpLayer->LoadLayer(pLayerElement, plugMgr);
			shpLayer->SetDepth(m_nDepth++);

			//ParserLayer(m_pLayoutSetting, pLayerElement);
		}

		pLayerElement = pLayerElement->NextSiblingElement();
	}

	return S_OK;
}


std::shared_ptr<LayerElement>  LayoutElement:: CreateLayer()
{
	DWORD layerId = GenerateLayerId();
	std::shared_ptr<LayerElement> ret(new LayerElement(layerId));
	return ret;
}

DWORD LayoutElement::GenerateLayerId()
{
    MagicView::CommonLib::AutoThreadSection aSection(&m_critSection);
	CurrentLayerId = (CurrentLayerId++) % DWORD_MAX_VALUE;

	if (CurrentLayerId == 0)
	{
		CurrentLayerId = 1;
	}

	return CurrentLayerId;
}

std::shared_ptr<LayerElement> LayoutElement::GetLayerInfo(DWORD dwLayerId, BOOL &bFound)
{
	bFound = FALSE;
	std::shared_ptr<LayerElement> ret;
	list<std::shared_ptr<LayerElement>> ::iterator it; 
	for (it = shpLayerArray.begin(); it != shpLayerArray.end(); it++)
	{
		if ((*it)->GetLayerId() == dwLayerId)
		{
			ret = *it;
			bFound = TRUE;
			break;
		}
	}

	return ret;
}

HRESULT LayoutElement:: Save(CString Filename, BOOL bExportToDir, CString &ContentXML)
{    
    CString					szAttribute;
	CString					dirPath;
    CString					szExt;
	int                     nDepth          = 0;
	PRO_PROPERTY			pProperty		= NULL;
	char					szValue[MAX_PATH];

	if(TRUE == bExportToDir)
	{
		//dirPath = Filename + _T("\\media\\");
		//::CreateDirectory(dirPath,NULL);
		dirPath = Filename + _T("\\media\\");
		Filename += _T("index.xml");
	}

	szExt = ::PathFindExtension(Filename);
	if (szExt.CompareNoCase(_T(".xml")) != 0)
	{
		Filename.Append(_T(".xml"));
	}


	TiXmlDocument* pDoc = new TiXmlDocument;
    if (pDoc == NULL)
    {
        TRACE0("Error: Out of memory");
        return E_UNEXPECTED;
    }

    TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","UTF-8","");
    if (NULL==pDeclaration)
    {
        return E_UNEXPECTED;
    }
    pDoc->LinkEndChild(pDeclaration);

    // Create root node
    TiXmlElement *pRootEle = new TiXmlElement("Canvas");
    if (NULL==pRootEle)
    {
        return E_UNEXPECTED;
    }
    pDoc->LinkEndChild(pRootEle);

    pRootEle->SetAttribute("ID", "C1");

    DWORD color = (GetRValue(m_BGColor) << 16) | (GetGValue(m_BGColor) << 8) | GetBValue(m_BGColor);
    sprintf_s(szValue, "0x%08x", color);
    pRootEle->SetAttribute("BgColor", szValue);

    _itoa_s(m_LayoutSize.cx, szValue, 10);
    pRootEle->SetAttribute("Width", szValue);

    _itoa_s(m_LayoutSize.cy, szValue, 10);
    pRootEle->SetAttribute("Height", szValue);

    Xml_Parser_Common::ConvertBOOL2Str(m_bKeepAspect, szValue);
    pRootEle->SetAttribute("KeepAspect", szValue);

    pRootEle->SetAttribute("Name", CW2A(m_LayoutName, CP_UTF8));

    Xml_Parser_Common::ConvertDuration2Str(m_LayoutDuration, szValue);
    pRootEle->SetAttribute("Duration", szValue);

    szAttribute = m_Description;
    if (szAttribute.GetLength() > 0)
    {
        pRootEle->SetAttribute("Description", CW2A(szAttribute, CP_UTF8));
    }

    sprintf_s(szValue, "%d", m_GroupID);
    pRootEle->SetAttribute("GroupID", szValue);

    sprintf_s(szValue, "%d", m_ClientID);
    pRootEle->SetAttribute("ClientID", szValue);

    // Create Layers child nodes
    //pos = m_DrawObjList.GetTailPosition();
	list<std::shared_ptr<LayerElement>>::reverse_iterator it;
	for(it = shpLayerArray.rbegin(); it != shpLayerArray.rend(); it++)
    {
		HRESULT hr = (*it)->Save(pRootEle, bExportToDir, dirPath, nDepth);	
		if (FAILED(hr))
		{
			return hr;
		}
    }

    if (pProperty)
    {
        delete pProperty;
        pProperty = NULL;
    }

    FILE* fp = _tfopen(Filename.GetString(), _T("w"));
    if (!fp)
    {
        TRACE0("Error: Open save file failed");
        return E_UNEXPECTED;
    }
    pDoc->SaveFile(fp);
    // Declare a printer
    TiXmlPrinter printer;

    // attach it to the document you want to convert in to a std::string
    pDoc->Accept(&printer);

    // Create a std::string and copy your document data in to the string
    std::string str = printer.CStr();
    ContentXML = CA2W(str.c_str(), CP_UTF8);

    delete pDoc;
    fclose(fp);

	return S_OK;
}


BOOL LayoutElement:: RemoveLayer(DWORD dwLayerId,BOOL bDel)
{
	BOOL bSuccess = FALSE;
	list<std::shared_ptr<LayerElement>> ::iterator it; 
	for (it = shpLayerArray.begin(); it != shpLayerArray.end(); it++)
	{
		if ((*it)->GetLayerId() == dwLayerId)
		{
			if(bDel)
				(*it).reset();
			shpLayerArray.erase(it);
			bSuccess = TRUE;
			break;
		}
	}

	return bSuccess;
}

VOID LayoutElement:: RemoverAllLayers()
{
	list<std::shared_ptr<LayerElement>> ::iterator it; 
	for (it = shpLayerArray.begin(); it != shpLayerArray.end(); it++)
	{
		(*it).reset();
		//shpLayerArray.erase(it);
	}
    shpLayerArray.clear();
}