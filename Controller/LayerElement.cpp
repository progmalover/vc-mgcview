#include "StdAfx.h"
#include "LayerElement.h"
#include "XmlParserCommon.h"


LayerElement::LayerElement(void) : m_nCountType(-1), m_bLoopSync(FALSE), m_bKeepAspect(FALSE),m_Type(LAYER),m_nDepth(0)
{
}

LayerElement::LayerElement(DWORD layerId) : m_dwLayerId(layerId), m_nCountType(-1), m_bLoopSync(FALSE), m_bKeepAspect(FALSE),m_Type(LAYER),m_nDepth(0)
{
}

LayerElement::~LayerElement(void)
{
}

VOID LayerElement::LoadLayer(TiXmlElement* pLayerElement, MagicView::CommonLib::CPluginManager2  &plugMgr)
{
	HRESULT hr = S_OK;

    TiXmlAttribute* pAttributeOfLayer = pLayerElement->FirstAttribute();
	CRect rectLayer;
	rectLayer.SetRectEmpty();
    //PROPERTY_TYPE propertyType = LAYER;
    //CS3DrawRectObj* pObj = NULL;
    //LAYERINFO* pLayerInfo = new LAYERINFO;

    DWORD durationLayer = 0;
    DWORD duration = 0;
 	COLORREF fillColor = RGB(167, 224, 180);
	
    // parser layer attribute
    while(pAttributeOfLayer)
    {
        CString ValueStr = CA2W(pAttributeOfLayer->Value(), CP_UTF8);
        CString NameStr = CA2W(pAttributeOfLayer->Name(), CP_UTF8);

        if (NameStr.CompareNoCase( _T("XPos")) == 0 )           
			rectLayer.left = _tstoi(ValueStr);
        else if (NameStr.CompareNoCase( _T("YPos")) == 0 )      
			rectLayer.top = _tstoi(ValueStr);
        else if (NameStr.CompareNoCase( _T("Width")) == 0 )     
			rectLayer.right = _tstoi(ValueStr);
        else if (NameStr.CompareNoCase( _T("Height")) == 0 )    
			rectLayer.bottom = _tstoi(ValueStr);
        else if (NameStr.CompareNoCase( _T("Type")) == 0 )      
			m_szTypeName = ValueStr;
        else if (NameStr.CompareNoCase( _T("TypeID")) == 0 )    
			m_nCountType = _tstoi(ValueStr);
        else if (NameStr.CompareNoCase( _T("Duration")) == 0 )  
			durationLayer = ConvertStr2Duration(ValueStr);
        else if (NameStr.CompareNoCase( _T("KeepAspect")) == 0) 
			m_bKeepAspect = ConvertStr2BOOL(ValueStr);
        else if (NameStr.CompareNoCase( _T("Name")) == 0)
            m_szSerialName = ValueStr;

        pAttributeOfLayer = pAttributeOfLayer->Next();
    }
    rectLayer.right += rectLayer.left;
    rectLayer.bottom += rectLayer.top;


	if (m_szTypeName.GetLength() > 0)
	{
		if (m_szTypeName.CompareNoCase(szTypeName_Layer) == 0)
		{
			m_Type = LAYER;
		}
		else if (m_szTypeName.CompareNoCase(szTypeName_Audio) == 0)
		{
			m_Type = AUDIO;
		}
		else if (m_szTypeName.CompareNoCase(szTypeName_Message) == 0)
		{
			m_Type = MESSAGE;
		}
		else
		{
			m_Type = USER_DEFINED_PLUGIN;
		}
	}

       // Parser Layer Info
    TiXmlElement* pContentElement = pLayerElement->FirstChildElement();

    int LastEndTime = 0;
    //COLORREF fillColor = RGB(167, 224, 180);
    //CString szName;
    while(pContentElement)
    {
		std::shared_ptr<MediaElement> shpMedia;
		CreateMedia(shpMedia);
		shpMedia->LoadMedias(pContentElement, plugMgr);
		shpMediaArray.push_back(shpMedia);
        //MEDIASETTING* pMediaSetting = new MEDIASETTING;
       // ParserContent(pMediaSetting, pContentElement, propertyType);
        duration += shpMedia->GetDuration();

        //pLayerInfo->AddTail(pMediaSetting);
        pContentElement = pContentElement->NextSiblingElement();
    }
	m_Position  = rectLayer;
	m_LogicPos  = rectLayer;

    if (duration < durationLayer)
    {
        m_bLoopSync = TRUE;
    }
}


VOID LayerElement :: CreateMedia(std::shared_ptr<MediaElement> &mediaElement)
{
	std::shared_ptr<MediaElement> ret(new MediaElement);
	//shpMediaArray.push_back(ret);
	mediaElement = ret;
	return;
}

DWORD LayerElement :: GetMediasTotalDuraiton(BOOL bCheckMediaFileLen)
{
	DWORD layerDuration = 0;
	vector<std::shared_ptr<MediaElement>>::iterator it;
	for (it = shpMediaArray.begin(); it != shpMediaArray.end(); it++)
	{
		if (bCheckMediaFileLen)
		{
			if ((*it)->GetMediaFile().GetLength() == 0)
			{
				continue;
			}
		}
		layerDuration += (*it)->GetDuration();
	}
	return layerDuration;
}

HRESULT LayerElement :: Save(TiXmlElement *pRootEle, BOOL bExportToDir, CString &dirPath, int &nDepth)
{
    int						TypeCount		= 0;
	CRect					rectObj;
    PLUGIN_SUPPORTFILES		*pSupportFiles	= NULL;
    CString					szAttribute;
	DWORD					duration		= 0;
	DWORD					layerDuration	= 0;
	DWORD					nMediaCount		= 0;
	char					szValue[MAX_PATH];
	HRESULT					hr;

	if (m_Type == BACKGROUND)
	{
		return S_OK;
	}

	TiXmlElement* pLayer = new TiXmlElement("Layer");
    if (pLayer == NULL)
    {
        return E_UNEXPECTED;
    }
    pRootEle->LinkEndChild(pLayer);

    sprintf_s(szValue, "L%d", nDepth);
    pLayer->SetAttribute("ID", szValue);
    pLayer->SetAttribute("Name", CW2A(m_szSerialName, CP_UTF8));

    m_LogicPos.ConvertToCRect(rectObj);
    sprintf_s(szValue, "%d", rectObj.left);
    pLayer->SetAttribute("XPos", szValue);
    sprintf_s(szValue, "%d", rectObj.top);
    pLayer->SetAttribute("YPos", szValue);
    sprintf_s(szValue, "%d", rectObj.Width());
    pLayer->SetAttribute("Width", szValue);
    sprintf_s(szValue, "%d", rectObj.Height());
    pLayer->SetAttribute("Height", szValue);

    Xml_Parser_Common::ConvertBOOL2Str(m_bKeepAspect, szValue);
    pLayer->SetAttribute("KeepAspect", szValue);

    sprintf_s(szValue, "%d", m_nCountType);
    pLayer->SetAttribute("TypeID", szValue);

    szAttribute = szTypeName_Layer;
    if (m_Type == LAYER)
    {
        szAttribute = szTypeName_Layer;
    }
    else if (m_Type == AUDIO)
    {
        szAttribute = szTypeName_Audio;
    }
    else if (m_Type == MESSAGE)
    {
        szAttribute = szTypeName_Message;
    }
	/* LAYOUT-DESIGNER-NEW-VER-RAYMONDLIU */
    else if (m_Type == USER_DEFINED_PLUGIN)
    {
		szAttribute = m_szTypeName;
        pSupportFiles = NULL;
/*
		if ((GetControllerApp->FindPluginSupportFile(m_szTypeName, pSupportFiles) == 0) 
            || m_szTypeName.CompareNoCase(_T("S3WebBrowser")) == 0)
        {
            szAttribute = m_szTypeName;
        }*/
    }

    if (szAttribute.GetLength() > 0)
    {
        pLayer->SetAttribute("Type", CW2A(szAttribute, CP_UTF8));
    }
    
	nDepth++;

    duration = 0;
    layerDuration = 0;
    nMediaCount = 0;

	vector<std::shared_ptr<MediaElement>>::iterator itM;
	for(itM = shpMediaArray.begin(); itM != shpMediaArray.end(); itM++)
	{
		hr = (*itM)->Save(pLayer, dirPath, m_Type, rectObj, bExportToDir, nMediaCount, layerDuration);
		if (FAILED(hr))
		{
			return hr;
		}
	}

    if (m_bLoopSync)
    {
		const char *szLayoutDuration;
		szLayoutDuration = pRootEle->Attribute("Duration");
		strcpy_s(szValue, strlen(szLayoutDuration) + 1, szLayoutDuration);
        //Xml_Parser_Common::ConvertDuration2Str(m_LayoutDuration, szValue);
        pLayer->SetAttribute("Duration", szValue);
    }
    else
    {
        Xml_Parser_Common:: ConvertDuration2Str(layerDuration, szValue);
        pLayer->SetAttribute("Duration", szValue);
    }

	return S_OK;
}
