#ifndef MEDIA_ELEMENT_H
#define MEDIA_ELEMENT_H

#pragma once

#include <tinyxml/tinyxml.h>
#include "XmlParserCommon.h"

class MediaElement
{
public:
	MediaElement(void);
	~MediaElement(void);
public:
	VOID LoadMedias(TiXmlElement* pContentElement, MagicView::CommonLib::CPluginManager2  &plugMgr);
	VOID CreateTextSetting(){m_pTextSetting.reset(new S3SIGNAGE_TEXT_SETTING);}

	VOID	SetMediaName(const CString szMediaName){m_MediaName = szMediaName;}
	CString GetMediaName(){return m_MediaName;}

	VOID	SetMediaType(const CString szTypeName_Text){m_MediaType = szTypeName_Text;}
	CString GetMediaType(){return m_MediaType;}

	VOID	SetMediaFile(const CString &mediaFile){m_MediaFile = mediaFile;}
	CString GetMediaFile(){return m_MediaFile;}

	VOID SetDesigner2(const std::shared_ptr<IS3RODesigner2> pDesigner2){ (pDesigner2 == NULL) ? (m_pDesigner2.reset()) : (m_pDesigner2 = pDesigner2);}
	std::shared_ptr<IS3RODesigner2> GetDesigner2(){return m_pDesigner2;}

	VOID SetDesigner(const std::shared_ptr<IS3RODesigner> pDesigner){pDesigner == NULL ? m_pDesigner.reset(): m_pDesigner = pDesigner;}
	std::shared_ptr<IS3RODesigner> GetDesigner(){return m_pDesigner;}

	VOID SetKeepAspect(BOOL bKeepAspect = TRUE){m_bKeepAspect = bKeepAspect;}
	BOOL GetKeepAspect(){return m_bKeepAspect;}

	VOID SetTextSetting(std::shared_ptr<S3SIGNAGE_TEXT_SETTING> pTextSetting){pTextSetting == NULL? m_pTextSetting.reset():m_pTextSetting = pTextSetting;}
	std::shared_ptr<S3SIGNAGE_TEXT_SETTING> GetTextSetting(){return m_pTextSetting;}

	VOID	 SetBGColor(const COLORREF BGColor){m_BGColor = BGColor;}
	COLORREF GetBGColor(){return m_BGColor;}

	VOID SetMediaID(const INT nMediaID){m_nMediaID = nMediaID;}
	INT  GetMediaID(){return m_nMediaID;}

	VOID	SetDuration(DWORD Duration){m_Duration= Duration;}
	DWORD	GetDuration(){return m_Duration;}

	//todo:add setting
	DWORD GetRefreshInterval(){return m_RefreshInterval;}
	VOID  SetRefreshInterval(DWORD RefreshInterval){m_RefreshInterval = RefreshInterval;}

	CRect GetPosition(){return m_Position;}
	VOID  SetPosition(const CRect Position){m_Position = Position;}

	DWORD GetVolumeCount(){return m_VolumeCount;}
	VOID  SetVolumeCount(const DWORD VolumeCount){m_VolumeCount = VolumeCount;}

	DWORD GetLoopCount(){return m_LoopCount;}
	VOID SetLoopCount(const DWORD LoopCount){m_LoopCount = LoopCount;}

	S3SIGNAGE_TRANSIION &GetTransIn(){return m_TransIn;}
	S3SIGNAGE_TRANSIION &GetTransOut(){return m_TransOut;}

	CString GetMessageFile(){return m_MessageFile;}
	VOID    SetMessageFile(const CString MessageFile){m_MessageFile = MessageFile;}

	HRESULT Save(TiXmlElement* pLayer, CString &dirPath, PROPERTY_TYPE propertyType, 
		CRect &rectObj, BOOL bExportToDir, DWORD &nMediaCount, DWORD &dwLastStartTime);

	HRESULT GenerateTextFile(IStream *pMemStream);
	void    SetXForm(INT degree, XFORM *ptXForm);

	VOID  SetStartTime(DWORD StartTime){m_StartTime = StartTime;}
	DWORD GetStartTime(){return m_StartTime;}
#if 1
	//--begin by RaymondLIu
	DWORD GetMediaHeight(){return m_MediaHeight;}
	DWORD GetMediaWidth() {return m_MediaWidth;}
	DWORD GetMediaDuration() {return m_MediaDuration;}
	DWORD64 GetMediaSize() {return m_MediaSize;}
	DWORD GetMediaVersion() {return m_MediaVersion;}
	CString GetMediaDigest() {return m_MediaDigest;}
	//CString m_MediaType;
	CString GetMediaSignature() {return m_MediaSignature;}
	CString GetMediaDescription() {return m_MediaDescription;}
	//--end by RaymondLiu
#endif
    VOID  operator=(class MediaElement &mediaElement)
    {
        m_nMediaID          = mediaElement.m_nMediaID;
        m_MediaName         = mediaElement.m_MediaName;
        m_MediaType         = mediaElement.m_MediaType;
        m_MediaFile         = mediaElement.m_MediaFile;
        m_MessageFile       = mediaElement.m_MessageFile;
        m_Position          = mediaElement.m_Position;
        m_StartTime         = mediaElement.m_StartTime;
        m_Duration          = mediaElement.m_Duration;
        m_RefreshInterval   = mediaElement.m_RefreshInterval;
        m_LoopCount         = mediaElement.m_LoopCount;
        m_VolumeCount       = mediaElement.m_VolumeCount;
        m_bKeepAspect       = mediaElement.m_bKeepAspect;
        m_BGColor           = mediaElement.m_BGColor;
        m_TransIn           = mediaElement.m_TransIn;
        m_TransOut          = mediaElement.m_TransOut;
        m_pTextSetting      = mediaElement.m_pTextSetting;
        m_pDesigner2        = mediaElement.m_pDesigner2;
        m_pDesigner         = mediaElement.m_pDesigner;
    }

    bool operator==(class MediaElement &mediaElement)
    {
        if(m_nMediaID == mediaElement.m_nMediaID &&
            m_MediaName == mediaElement.m_MediaName &&
            m_MediaType == mediaElement.m_MediaType &&
            m_MediaFile == mediaElement.m_MediaFile &&
            m_MessageFile == mediaElement.m_MessageFile &&
            m_Position == mediaElement.m_Position &&
            m_StartTime == mediaElement.m_StartTime &&
            m_Duration == mediaElement.m_Duration &&
            m_RefreshInterval == mediaElement.m_RefreshInterval &&
            m_LoopCount == mediaElement.m_LoopCount &&
            m_VolumeCount == mediaElement.m_VolumeCount &&
            m_bKeepAspect == mediaElement.m_bKeepAspect &&
            m_BGColor == mediaElement.m_BGColor &&
            m_TransIn == mediaElement.m_TransIn &&
            m_TransOut == mediaElement.m_TransOut &&
            m_pTextSetting == mediaElement.m_pTextSetting)
        {
            return true;
        }
        return false;
    }
protected:
	HRESULT ParserTextFile(TiXmlElement *pTextFileElement);
	HRESULT ParserTranstion(S3SIGNAGE_TRANSIION *pTransition, TiXmlElement *pTransitionElement);
	HRESULT QueryDesignerWithName(CString szName,  MagicView::CommonLib::CPluginManager2  &plugMgr,
		std::shared_ptr<IS3RODesigner2>& pDesigner2, std::shared_ptr<IS3RODesigner>& pDesigner);

private:
	INT												m_nMediaID;
	CString											m_MediaName;
	CString											m_MediaType;
    CString											m_MediaFile;
    CString											m_MessageFile;
    CRect											m_Position;
    DWORD											m_StartTime;
    DWORD											m_Duration;
	DWORD											m_RefreshInterval;
    DWORD											m_LoopCount;
    DWORD											m_VolumeCount;
    BOOL											m_bKeepAspect;
    COLORREF										m_BGColor;
    S3SIGNAGE_TRANSIION								m_TransIn;
    S3SIGNAGE_TRANSIION								m_TransOut;
    std::shared_ptr<S3SIGNAGE_TEXT_SETTING>         m_pTextSetting;
    std::shared_ptr<IS3RODesigner2>					m_pDesigner2;
    std::shared_ptr<IS3RODesigner>					m_pDesigner;
#if 1
private:
	//--begin by RaymondLIu
	DWORD m_MediaHeight;
	DWORD m_MediaWidth;
	DWORD m_MediaDuration;
	DWORD64 m_MediaSize;
	DWORD m_MediaVersion;
	CString m_MediaDigest;
	//CString m_MediaType;
	CString m_MediaSignature;
	CString m_MediaDescription;
	//--end by RaymondLiu
#endif
};

#endif