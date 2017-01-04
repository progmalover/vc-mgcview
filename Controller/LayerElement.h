#ifndef LAYER_ELEMENT_H
#define LAYER_ELEMENT_H
#pragma once
#include "S3Types.h"
#include "MediaElement.h"

using namespace std;

class LayerElement
{
public:
	LayerElement(void);
	LayerElement(DWORD layerId);
	~LayerElement(void);

public:
	VOID LoadLayer(TiXmlElement* pLayerElement, MagicView::CommonLib::CPluginManager2  &plugMgr);
	VOID CreateMedia(std::shared_ptr<MediaElement> &mediaElement);

	VOID InsertAfter(std::shared_ptr<MediaElement> mediaElement, vector<std::shared_ptr<MediaElement>>::size_type Index)
			{shpMediaArray.insert(shpMediaArray.begin()+(Index+1), mediaElement);}
	VOID Insert(std::shared_ptr<MediaElement> mediaElement, vector<std::shared_ptr<MediaElement>>::size_type Index)
		{shpMediaArray.insert(shpMediaArray.begin()+Index, mediaElement);}

	VOID AddHead(std::shared_ptr<MediaElement> mediaElement){shpMediaArray.insert(shpMediaArray.begin(), mediaElement);}
	VOID RemoveAt(DWORD index)
	{
		if (index < shpMediaArray.size())
		{
			shpMediaArray.erase(shpMediaArray.begin()+index);
		}
	}
	VOID RemoveAll(){shpMediaArray.clear();}
	VOID RemoveTailElement()
    {
        if (!shpMediaArray.empty())
            shpMediaArray.pop_back();
    }

	VOID AddTailMediaElement(std::shared_ptr<MediaElement> mediaElement){shpMediaArray.push_back(mediaElement);}

	std::shared_ptr<MediaElement> GetMediaElement(DWORD index){return (*(shpMediaArray.begin() + index));}
	vector<std::shared_ptr<MediaElement> > &GetMediaList(){return shpMediaArray;}
	VOID SetMediaList(vector<std::shared_ptr<MediaElement>> &pMediaArray){shpMediaArray = pMediaArray;}

	DWORD GetMediasCount(){return shpMediaArray.size();}
	DWORD GetMediasTotalDuraiton(BOOL bCheckMediaFileLen = FALSE);

	//VOID SetRect(FRect rectLayer){m_LogicPos = rectLayer;}
	
	VOID SetLogicPos(FRect rectLayer){m_LogicPos = rectLayer;}
	FRect GetLogicPos() {return m_LogicPos; }

	VOID SetLayerId(const DWORD dwLayerId){m_dwLayerId = dwLayerId;}
	DWORD GetLayerId(){return m_dwLayerId;}

	VOID SetPropertyType(PROPERTY_TYPE propertyType){m_Type = propertyType;}
	PROPERTY_TYPE GetPropertyType(){return m_Type;}

	VOID SetKeepAspect(BOOL bKeepAspect=TRUE){m_bKeepAspect = bKeepAspect;}
	BOOL GetKeepAspect(){return m_bKeepAspect;}

	//VOID SetAspectRatio(FLOAT fAspectRatio){m_fAspectRatio = fAspectRatio;}

	VOID SetTypeCount(const int nCountType){m_nCountType = nCountType;}
	int  GetTypeCount(){return m_nCountType;}

	VOID	SetTypeName(CString szTypeName){m_szTypeName = szTypeName;}
	CString GetTypeName(){return m_szTypeName;}

	HRESULT Save(TiXmlElement *pRootEle, BOOL bExportToDir, CString &dirPath, int &nDepth);

	UINT    GetDepth(){return m_nDepth;}
	VOID    SetDepth(UINT nDepth){m_nDepth = nDepth;}

	CRect GetPosition(){return m_Position;}
	VOID  SetPosition(CRect &Position){m_Position = Position;}

	VOID SetbLoopSync(const BOOL bLoopSync){m_bLoopSync = bLoopSync;}
	BOOL GetLoopSync(){return m_bLoopSync;}

    VOID SetSerialName(const CString szSerialName){m_szSerialName = szSerialName;}
    CString GetSerialName(){return m_szSerialName;}

    VOID SwapMedia(int pos1, int pos2)
    {
        if (pos1 >= 0 && pos1 < GetMediasCount() &&
            pos2 >= 0 && pos2 < GetMediasCount() )
        {
            std::shared_ptr<MediaElement> tmp;
            tmp = shpMediaArray[pos1];
            shpMediaArray[pos1] = shpMediaArray[pos2]; 
            shpMediaArray[pos2] = tmp;
        }
    }

private:
	vector<std::shared_ptr<MediaElement>> shpMediaArray;
	//list<MediaElement*> Layers;
	
	DWORD			m_dwLayerId;     // layerid:Associate with CS3DrawObj
	int             m_nCountType;       /// The obj type count typeid
    CString			m_szSerialName;
	CRect           m_Position;         /// The physical position info in the draw wnd
	FRect			m_LogicPos;

	CString			m_szTypeName;
	//int				m_TypeID;
	BOOL            m_bLoopSync;     // Layer loop sync the layout or not
	BOOL            m_bKeepAspect;   // Layer size need keep aspect ratio
	//FLOAT           m_fAspectRatio;
	UINT            m_nDepth;
	PROPERTY_TYPE   m_Type;          //layer type :background...
};

#endif