#pragma once
#include "cmds.h"
#include "S3Types.h"
#include "S3DrawRectObj.h"
#include "LayoutSettings.h"
#include "LayerElement.h"
#include "MediaElement.h"
//
//zxy designed for via 2012.2
//
using namespace _CMDS;
namespace DESIGNERCMDS
{
enum cmds
{
	CMD_START = CMD_MIN,
	CMD_OBJBUILD = 1,
	CMD_OBJMOVE,
	CMD_OBJSIZE,
	CMD_OBJTOFRONT,
	CMD_OBJTOBACK,
	//CMD_OBJPASTE,
	CMD_OBJDELETE,
	CMD_OBJSGROUPED,
	CMD_OBJSGROUPMOVE,
	CMD_OBJSGROUPDELETE,
	CMD_OBJSCOLONE,
	CMD_WNDSCROLL,
	CMD_LAYOUTSIZE,
	CMD_LAYOUTCLR,
	CMD_LAYOUTZOOM,
	CMD_BINDLINE,
	CMD_BINDLINEMOVE,
	CMD_BINDLINEDELETE,

	CMD_PROPERLAYOUT, //for layout property
	CMD_PROPERINST //for plugin instance property.

};

//use this macro to declare a mass friend class.
#define DELARE_FRIEND_LAYOUT_CMDS()\
DECLARE_FRIEND_CLASS(_baseLayoutWndCmd)\
DECLARE_FRIEND_CLASS(objBuildCmd)\
DECLARE_FRIEND_CLASS(objMoveCmd)\
DECLARE_FRIEND_CLASS(objDeleteCmd)\
DECLARE_FRIEND_CLASS(objPasteCmd)\
DECLARE_FRIEND_CLASS(objSizeCmd)\
DECLARE_FRIEND_CLASS(objToFrontCmd)\
DECLARE_FRIEND_CLASS(objToBackCmd)\
DECLARE_FRIEND_CLASS(objsGroupedCmd)\
DECLARE_FRIEND_CLASS(objsGroupMoveCmd)\
DECLARE_FRIEND_CLASS(objsGroupDeleteCmd)\
DECLARE_FRIEND_CLASS(objsColoneCmd)\
DECLARE_FRIEND_CLASS(wndScrollCmd)\
DECLARE_FRIEND_CLASS(layoutClrCmd)\
DECLARE_FRIEND_CLASS(layoutZoomCmd)\
DECLARE_FRIEND_CLASS(bindLineCmd)\
DECLARE_FRIEND_CLASS(bindLineMoveCmd)\
DECLARE_FRIEND_CLASS(bindLineDeleteCmd)\
DECLARE_FRIEND_CLASS(properLayoutCmd)\
DECLARE_FRIEND_CLASS(properInstCmd)


//struct for  ojbmove,bindlinemove and objsize
typedef  struct objOrgInfo{

	CS3DrawObj *pInst;
	FRect	    orgRect;
}OBJ_ORG_INFO;

typedef vector<OBJ_ORG_INFO> OBS_INFO;
//struct for  objdelete
typedef struct ojbStoreInfo
{
	ojbStoreInfo()
	{
		pInstance = NULL;
		//pLayerInfo = NULL;
		instPos = -1;
	}
	CS3DrawObj *pInstance;
	//LAYER  *pLayerInfo;
	std::shared_ptr<LayerElement> layerInfo;
	//std::shared_ptr<MediaElement>> MediaArray;
	int	    	instPos;//instance pos in drawlist of designerctrl.
}OBJ_INFO;


typedef vector<OBJ_INFO> STORE_OBJS;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class LayoutCmdEngine;
class LayoutCmdFactory :public _baseCmdFactory<LayoutCmdEngine>
{
public:
	LayoutCmdFactory();
	~LayoutCmdFactory();
public:
	_baseCmd * newCmdBegin(int nID);
	_baseCmd * getBuffedCmd();
	_baseCmd * newCmdEnd();
	_baseCmd * newCmd(int nID);
	LPCTSTR    getCmdName(_baseCmd *);
	bool	   delCmd(_baseCmd *pCmd);
	CWnd	*  getTargetWnd();
private:
	_baseCmd * m_pBuffCmd;
};

class LayoutCmdEngine :public CmdsEngine<LayoutCmdFactory>
{
	friend class LayoutCmdFactory;
public:
	LayoutCmdEngine(){};
	~LayoutCmdEngine(){};
public:
	virtual bool bindTarget(CWnd *pTargetWnd);
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//base wnd cmd to support window status.
class _baseLayoutWndCmd :public _baseCmd
{

	friend class LayoutCmdFactory;
protected:
	_baseLayoutWndCmd(int nID,CWnd *pTarget);
	~_baseLayoutWndCmd();
public:
	int redo();
	int undo();
	int ondo();

};

class wndScrollCmd :public _baseCmd
{
	friend class LayoutCmdFactory ;
protected:
	wndScrollCmd(int nID,CWnd *pTarget);
	~wndScrollCmd();
private:
	float xFTrans;
	float yFTrans;
	int	  nVScrollPos;
	int	  nHScrollPos;
	float fZoomScale;
	FRect viewPort;
	CRect cPageRect;
protected:
	int RetoreStatus();
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmds
class objBuildCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;   
protected:
	objBuildCmd(int nID,CWnd *pTarget);
	~objBuildCmd();
public:
	int redo();
	int undo();
	int ondo();
public://context information for undo function.
	CString			strMediaFile;
	CString			strMeidaType;
	POINT			dragPoint;
	DWORD			nWidth ;
	DWORD			nHeight;
	POBJINFO		pObjInfo;
	std::shared_ptr<MediaElement>	        pAppendedSetting; 
	std::shared_ptr<LayerElement>			pInstLayerElement;
	vector< std::shared_ptr<MediaElement> >   *pInstLayerInfo;
	//LAYERINFO	*	pInstLayerInfo;
	CS3DrawObj	*   pContainObj;
	CS3DrawObj  *	pInstance;
};
	 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class objPasteCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory;
protected:
	objPasteCmd(int nID,CWnd *pTarget);
	~objPasteCmd();
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class objDeleteCmd:public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory;
protected:
	objDeleteCmd(int nID,CWnd *pTarget);
	~objDeleteCmd();
public:
	int redo();
	int undo();
	int ondo();
private:
	int nCmdStatus;
public:
	STORE_OBJS		storeObjs;
	CS3DrawObjList	*pCtrlSelList;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class objMoveCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	objMoveCmd(int nID,CWnd *pTarget);
	~objMoveCmd();
private:
	void restoreMove();
	void restoreBoundMove();
public:
	void addObj(CS3DrawObj *pObj);
	void addBoundObj(CS3DrawObj *pObj);
	int redo(); //redo is equal to redo.
	int undo();
	int ondo();
public:
	OBJ_ORG_INFO	boundObj; 
	OBS_INFO		objsList;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class objSizeCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	objSizeCmd(int nID,CWnd *pTarget);
	~objSizeCmd();
public:
	int redo();
	int undo();
	int ondo();
public:
	CS3DrawObj* pDragObj;
	FRect		storeRC;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ONE_STEP 1
#define BIG_STEP 2
class objToFrontCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	objToFrontCmd(int nID,CWnd *pTarget);
	~objToFrontCmd();
public:
	int nType;
private:
	int DoMove(CS3DrawObj *);
	CS3DrawObj *pDrawObj;
	int		nPosIndex;
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class objToBackCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	objToBackCmd(int nID,CWnd *pTarget);
	~objToBackCmd();
public:
	int nType;
private:
	int DoMove(CS3DrawObj *);
	CS3DrawObj *pDrawObj;
	int			nPosIndex;
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef CList<OBJ_INFO> CS3DrawObjInfoList;
class objsColoneCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	objsColoneCmd(int nID,CWnd *pTarget);
	~objsColoneCmd();
public:
	CS3DrawObjList *pCopyObjsList;
private:
	CS3DrawObjInfoList objsColoneList;
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class objsGroupedCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	objsGroupedCmd(int nID,CWnd *pTarget);
	~objsGroupedCmd();
private:
	//void detectObjsList();
public:
	void setSelObjs(CS3DrawObjList *pList);
	void addObj(CS3DrawObj *pObj);
	int redo();
	int undo();
	int ondo();
public:
	CS3DrawObj *pGroupBound;
	CS3DrawObjList objsSelList;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class objsGroupMoveCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
	typedef struct Rect_Data
	{
		FRect orgRect;
		CS3DrawObj *pDrawObj;
	}FRECT_DATA;
	typedef vector<FRECT_DATA> FRECT_DATAS;
protected:
	objsGroupMoveCmd(int nID,CWnd *pTarget);
	~objsGroupMoveCmd();
	void DoDataExchange();
public:
	FRECT_DATA		boundData;
	FRECT_DATAS		fRects;
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class objsGroupDeleteCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	objsGroupDeleteCmd(int nID,CWnd *pTarget);
	~objsGroupDeleteCmd();
public:
	int redo();
	int undo();
	int ondo();
private:
	void removeStoreObjFromDesigner();
	void RestoreStoreObjToDesigner();
	void addObjsToStoreList(CS3DrawObjList *pObjs);
public:
	CS3DrawObj*		pGroupBound;
	STORE_OBJS		objStoreList;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class layoutClrCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	layoutClrCmd(int nID,CWnd *pTarget);
	~layoutClrCmd();
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class layoutZoomCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	layoutZoomCmd(int nID,CWnd *pTarget);
	~layoutZoomCmd();
public:
	int redo();
	int undo();
	int ondo();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//new build bind
class bindLineCmd :public _baseLayoutWndCmd
{
	friend class LayoutCmdFactory ;
protected:
	bindLineCmd(int nID,CWnd *pTarget);
	~bindLineCmd();
public:
	int redo();
	int undo();
	int ondo();
public:
	int nXIndex;
	int nYIndex;
	int nXLineData;
	int nYLineData;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class bindLineMoveCmd :public _baseLayoutWndCmd
{
	friend LayoutCmdFactory ;
protected:
	bindLineMoveCmd(int nID,CWnd *pTarget);
	~bindLineMoveCmd();
	
public:
	int redo();
	int undo();
	int ondo();
public:
	//void attachObjs(); //attach
	bool addAttach(CS3DrawObj *pObj);
	OBS_INFO  attachObjs;
private:
	void restoreObjs();
	void restoreLine();
public:
	int nXLine;
	int nXLineDate;
	int nYLine;
	int nYLineDate;  
	
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class bindLineDeleteCmd :public _baseLayoutWndCmd
{
	friend LayoutCmdFactory ;
protected:
	bindLineDeleteCmd(int nID,CWnd *pTarget);
	~bindLineDeleteCmd();
public:
	int redo();
	int undo();
	int ondo();
private:
	int nXLine; //line pos in bindline vector.
	int nYLine;
	int nXLineData;
	int nYLineData;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class properLayoutCmd :public _baseCmd
{
	friend LayoutCmdFactory ;
protected:
	properLayoutCmd(int nID,CWnd *pTarget);
	~properLayoutCmd();
public:
	int redo();
	int undo();
	int ondo();
};

class properInstCmd :public _baseCmd
{
	friend LayoutCmdFactory ;
protected:
	properInstCmd(int nID,CWnd *pTarget);
	~properInstCmd();
public:
	int redo();
	int undo(); 
	int ondo();
};


//public functions.
extern LPCTSTR GetTypeName(  const PROPERTY_TYPE propertyType);
extern BOOL RemoveDrawObjFromList(CS3DrawObjList *pInsts,CS3DrawObj *pObj);
extern BOOL InsertToObjListByIndex(CS3DrawObjList *pInsts,CS3DrawObj *pObj,int instPos);
extern BOOL FindObjInObjList(CS3DrawObj *pObj,CS3DrawObjList *pList ,POSITION  & pos ,int &index );
extern bool RectInRect(FRect fRect ,FRect fDeskRect);
extern void getObjsInRect( CS3DrawObjList *pSource,OBS_INFO *pDesk,FRect selRect);
extern void CopyObjsList( CS3DrawObjList *pSource,CS3DrawObjList *pDesk,BOOL bRevert);
extern  CS3DrawObj *  GetLastCreateObjInUndoCmds( LayoutCmdEngine *pEngine,CS3DrawObj *pObj);

};

