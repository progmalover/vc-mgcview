#ifndef LAYOUT_ELEMENT_H
#define LAYOUT_ELEMENT_H

#pragma once

#include "LayerElement.h"

#define  DWORD_MAX_VALUE 4294967295
class LayoutElement
{
public:
	LayoutElement(void);
	~LayoutElement(void);
public:
	HRESULT LoadLayout(CString Filename, MagicView::CommonLib::CPluginManager2  &plugMgr, const CString& LayoutName, CString ContentXML);
	std::shared_ptr<LayerElement> CreateLayer();
	VOID AddTailLayer(std::shared_ptr<LayerElement> &layer){shpLayerArray.push_back(layer);}
	VOID AddHeadLayer(std::shared_ptr<LayerElement> &layer){shpLayerArray.push_front(layer);}

	std::shared_ptr<LayerElement> GetLayerInfo(DWORD dwLayerId, BOOL &bFound);
	BOOL RemoveLayer(DWORD dwLayerId,BOOL bDel = TRUE);

	CString GetLayoutName(){return m_LayoutName;}
	VOID    SetLayoutName(CString layoutName){m_LayoutName = layoutName;}

	CString GetDescription(){return m_Description;}
    VOID    SetDescription(CString strDescrip){ m_Description = strDescrip;}

	DWORD   GetGroupID(){return m_GroupID;}
	VOID    SetGroupID(DWORD GroupID){m_GroupID = GroupID;}

	CSize   GetLayoutSize(){return m_LayoutSize;}
    VOID	SetLayoutSize(CSize layoutSize){m_LayoutSize = layoutSize;}     
    FLOAT   GetLayoutRotateDegree(){return m_fRotateDegree;}
    VOID	SetLayoutRotateDegree(FLOAT fRotateDegreee){m_fRotateDegree = fRotateDegreee;}

	COLORREF GetBGColor(){return m_BGColor;}
	VOID     SetBGColor(COLORREF BGColor){m_BGColor = BGColor;};

	DWORD   GetLayoutDuration(){return m_LayoutDuration;}
	VOID    SetLayoutDuration(const DWORD LayoutDuration){m_LayoutDuration = LayoutDuration;}

	BOOL    GetbKeepAspect(){return m_bKeepAspect;}
	VOID    SetbKeepAspect(BOOL bKeepAspect){m_bKeepAspect = bKeepAspect;}

	VOID  SetLayoutFile(const CString LayoutFile){m_LayoutFile = LayoutFile;}

	VOID  SetMediaContentSize(const DWORD64 MediaContentSize){m_MediaContentSize = MediaContentSize;}
	DWORD64 GetMediaContentSize(){return m_MediaContentSize;}

	DWORD GetMediaCount(){return m_MediaCount;}

	HRESULT Save(CString Filename, BOOL bExportToDir, CString &ContentXML);

	list<std::shared_ptr<LayerElement>> GetAllLayers(){return shpLayerArray;}
	VOID	RemoverAllLayers();

	DWORD GetClientID(){return m_ClientID;}
	VOID  SetClientID(DWORD ClientID){m_ClientID = ClientID;}

protected:
	//VOID	ResetLayout();
	DWORD   GenerateLayerId();

private:
	CString					m_LayoutName;
    DWORD					m_LayoutDuration;
    DWORD					m_GroupID;
    CString					m_Description;
    DWORD					m_ClientID;
    CSize					m_LayoutSize;
    FLOAT                   m_fRotateDegree;
    BOOL					m_bKeepAspect;
    COLORREF				m_BGColor;
    CString					m_LayoutFile;
    DWORD64					m_MediaContentSize;
    DWORD					m_MediaCount;
	int						m_TypeID;
	static DWORD            CurrentLayerId;					
	//Map<CS3DrawObj*, LAYERINFO*> S3DrawLayerMap;
	list<std::shared_ptr<LayerElement>> shpLayerArray;
    MagicView::CommonLib::ThreadSection   m_critSection;
};

#endif