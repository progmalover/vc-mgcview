#pragma once
#include "cmds.h"
#include "S3Types.h"
#include "S3DrawRectObj.h"
#include "LayoutSettings.h"
#include "MediaElement.h"
#include "LayerElement.h"
#include "boost/tuple/tuple.hpp"
#include "boost/math/common_factor.hpp"
using namespace _CMDS;

#define CMD_PROPERLAYOUTBASE            CMD_PROPERLAYOUT * 3
#define CMD_PROPERACTIONS               CMD_PROPERLAYOUTBASE  + 1
#define CMD_PROPERLAYOUTSIZE            CMD_PROPERLAYOUTBASE  + 2
#define CMD_PROPERLAYOUTCOLOR           CMD_PROPERLAYOUTBASE  + 3
#define CMD_PROPERLAYOUTTEXTCHANGE      CMD_PROPERLAYOUTBASE  + 4

#define CMD_LAYERBASE                   CMD_PROPERLAYOUTBASE * 3
#define CMD_LAYERNAME                   CMD_LAYERBASE  + 1
#define CMD_BASECONTENT                 CMD_LAYERBASE  + 2                 
#define CMD_TEXTSETTING                 CMD_LAYERBASE  + 3
#define CMD_TRANSITON                   CMD_LAYERBASE  + 4
#define CMD_LOOPCHECK                   CMD_LAYERBASE  + 5
#define CMD_KEEPASPECTCHECK             CMD_LAYERBASE  + 6
namespace LAYOUTPROPERTYCMDS  
{

#define DELARE_FRIEND_PROPERTY_CMDS()\
	DECLARE_FRIEND_CLASS(actionsLayoutCmd)\
	DECLARE_FRIEND_CLASS(actionsInstCmd)\
	DECLARE_FRIEND_CLASS(layoutSizeCmd)\
    DECLARE_FRIEND_CLASS(layoutColorCmd)\
	DECLARE_FRIEND_CLASS(mediaListNumCmd)\
	DECLARE_FRIEND_CLASS(mediaListItemChangeCmd)\
	DECLARE_FRIEND_CLASS(mediaListNewItemCmd)\
	DECLARE_FRIEND_CLASS(mediaListDeleteItemCmd)\
	DECLARE_FRIEND_CLASS(layerNameChangeCmd)\
    DECLARE_FRIEND_CLASS(layerBasicContentChangeCmd)\
    DECLARE_FRIEND_CLASS(layerTextSettingCmd)\
    DECLARE_FRIEND_CLASS(layerTransitionCmd)\
    DECLARE_FRIEND_CLASS(layerLoopCheckCmd)\
    DECLARE_FRIEND_CLASS(layerKeepAspectCheckCmd)
#if 0
    DECLARE_FRIEND_CLASS(layerColorChangeCmd)\
    DECLARE_FRIEND_CLASS(layerDurationCmd)\
    DECLARE_FRIEND_CLASS(layerVolumeCmd)\
    DECLARE_FRIEND_CLASS(layerTransparencyCmd)\
    DECLARE_FRIEND_CLASS(layerRefreshIntervalCmd)\
  
#endif

typedef vector<std::shared_ptr<MediaElement>> MEDIAS;
class _basePropCmd:public _baseCmd
{
	public:
	_basePropCmd(int id = 0 ,CWnd *pTarget = NULL);
	~_basePropCmd();

public:
	int undo();
	int redo();
	int ondo();
};

class actionsLayoutCmd:public _basePropCmd
{
public:
	actionsLayoutCmd(int id = 0 ,CWnd *pTarget = NULL);
	~actionsLayoutCmd();
public:
	CS3DrawObj *pDrawObj; 
	UINT	   nMessage;
    DWORD      dwParam;
    DWORD      dwLParam;
public:
    BOOL  BeginPrepareStatus();
    BOOL  EndPrepareStatus();
public:
	int undo();
	int redo();
	int ondo();
};

 
class actionsInstCmd:public _basePropCmd
{
public:
	actionsInstCmd(int id = 0 ,CWnd *pTarget = NULL);
	~actionsInstCmd();
public:
	UINT	   nMessage;
	DWORD	   dwWParam;
	DWORD	   dwLParam;
	/*CS3DrawObj *pDrawObj; */
public:
    BOOL BeginPrepareStatus();
    BOOL EndPrepareStatus();
	int undo();
	int redo();
	int ondo();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// action cmds
class baseActionCmd :public _baseCmd
{
public:
    baseActionCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){};
    ~baseActionCmd(){
        nScrollPos = -1;
        pScrollCtrl = NULL;
    };
public:
    int nScrollPos ;
    CWnd *pScrollCtrl;
public:
    int redo();
    int undo();
    int ondo();
};

class layoutSizeCmd :public _baseCmd
{
	//friend class LayoutCmdFactor ;
public:
	layoutSizeCmd(int nID,CWnd *pTarget);
	~layoutSizeCmd();
public:
    boost::tuple<CSize ,CSize> storeSize;
    boost::tuple<vector<CRect> ,vector<CRect>>  storeRects;
    boost::tuple<FLOAT,FLOAT> storeRotateDegree;
    boost::tuple<FLOAT,FLOAT> storeTransX;
    boost::tuple<FLOAT,FLOAT> storeTransY;
   
    BOOL   bOpenClient;
public:
	int redo();
	int undo();
	int ondo();
};

class layoutColorCmd :public _baseCmd
{

    //friend class LayoutCmdFactor ;
public:
    layoutColorCmd(int nID,CWnd *pTarget);
    ~layoutColorCmd();
public:
    COLORREF swapColor;
    COLORREF curColor;
public:
    int redo();
    int undo();
    int ondo();

};

class LayoutTextChangeCmd :public _baseCmd
{
public:
    LayoutTextChangeCmd(int nID ,CWnd *pTarget);
    ~LayoutTextChangeCmd();
protected:
    boost::tuple<CString,CString>  tupleNames;
    boost::tuple<CString,CString>  tupleDescs;
public:
    int nType;
public:
    int redo();
    int undo();
    int ondo();
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Layer media list Cmds
//
enum MEDIALIST_ACTION{
    ACTION_DEL = 0,
    ACTION_ADD,
    ACTION_ITEM_MOVE_UP,
    ACTION_ITEM_MOVE_DOWN
};
class mediaListNewItemCmd:public baseActionCmd
{
public:
	mediaListNewItemCmd(int nID,CWnd *pTarget);
	~mediaListNewItemCmd();
public:
	 std::shared_ptr<MediaElement> MediaSetting;
	 CS3DrawObj  *pDrawObj;
public:
	int redo();
	int undo();
	int ondo();
};

class mediaListDeleteItemCmd:public baseActionCmd
{
public:
	mediaListDeleteItemCmd(int nID,CWnd *pTarget);
	~mediaListDeleteItemCmd();
public:
	std::shared_ptr<MediaElement> MediaSetting;
	CS3DrawObj  *pDrawObj;
	int		iIndex;
public:
	int redo();
	int undo();
	int ondo();
};

class mediaListItemChangeCmd :public baseActionCmd
{
public:
	mediaListItemChangeCmd(int nID,CWnd *pTarget);
	~mediaListItemChangeCmd();
protected:
		void  SaveInstMediaList(CS3DrawObj *pDrawObj,  MEDIAS &medias,BOOL bCreate = 0);
		std::shared_ptr<LayerElement>  RestoreInstMediaList( MEDIAS &medias,CS3DrawObj *pDraw);
public:
	CS3DrawObj *pcurObj;
	MEDIAS  storeMedias; //存储对象，保留对象的全部信息
	MEDIAS  saveMedias;  //保留对象 ，只是保留指针序列
public:
	int redo();
	int undo();
	int ondo();

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Layer Change Cmds
//
class layerNameChangeCmd :public baseActionCmd
{
public:
    layerNameChangeCmd(int nID,CWnd *pTarget);
    ~layerNameChangeCmd();
public:
    boost::tuple<CString ,CString> tupleNames;
    CS3DrawObj *pcurObj;
public:
    int redo();
    int undo();
    int ondo();
};


class layerLoopCheckCmd :public baseActionCmd
{
public:
    layerLoopCheckCmd(int nID,CWnd *pTarget);
    ~layerLoopCheckCmd();
public:
    CS3DrawObj *pcurObj;
    BOOL        nCheck; 
public:
    int redo();
    int undo();
    int ondo();
};

class layerKeepAspectCheckCmd:public baseActionCmd
{
public:
    layerKeepAspectCheckCmd(int nID,CWnd *pTarget);
    ~layerKeepAspectCheckCmd();
public:
    CS3DrawObj *pcurObj;
    BOOL        nCheck; 
public:
    int redo();
    int undo();
    int ondo();

};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Layer property basic content
//

class layerBasicContentChangeCmd :public baseActionCmd
{
public:
    layerBasicContentChangeCmd(int nID,CWnd *pTarget);
    ~layerBasicContentChangeCmd();
public:
    CS3DrawObj *pcurObj;
    //MEDIASETTING  *pcurMedia;
    shared_ptr<MediaElement> pcurMedia;
    MediaElement  swapMedia;
    int           nTransparency;
public:
    BOOL SaveSwapMedia();
    int redo();
    int undo();
    int ondo();
};

//
//Layer color change.
//
#if 0
class layerColorChangeCmd :public _baseCmd
{
public:
    layerColorChangeCmd(int nID,CWnd *pTarget);
    ~layerColorChangeCmd();
public:
    boost::tuple<COLORREF ,COLORREF> tupleColors;
    CS3DrawObj *pcurObj;
    MEDIASETTING *pSetting;
public:
    int redo();
    int undo();
    int ondo();
};

//
//Layer Duration
//

class layerDurationCmd :public _baseCmd
{
public:
    layerDurationCmd(int nID ,CWnd *pTarget);
    ~layerDurationCmd();
public:
    boost::tuple<DWORD ,DWORD> tupleTimes;
    CS3DrawObj *pcurObj;
    MEDIASETTING *pSetting;
public:
    int redo();
    int undo();
    int ondo();
};

//
//Layer volume
//
class layerVolumeCmd :public _baseCmd
{
public:
    layerVolumeCmd(int nID ,CWnd *pTarget);
    ~layerVolumeCmd();
public:
    boost::tuple<int ,int > tupleVals;
    CS3DrawObj *pcurObj;
    MEDIASETTING *pSetting;
public:
    int redo();
    int undo();
    int ondo();
};

//
//Layer Transparentcy
//
class layerTransparencyCmd :public _baseCmd
{
public:
    layerTransparencyCmd(int nID ,CWnd *pTarget);
    ~layerTransparencyCmd();
public:
    boost::tuple<int ,int> tupleVals;
    CS3DrawObj *pcurObj;
    MEDIASETTING *pSetting;
public:
    int redo();
    int undo();
    int ondo();
};

//
//Layer  Refresh Interval
//
class layerRefreshIntervalCmd :public _baseCmd
{
public:
    layerRefreshIntervalCmd(int nID ,CWnd *pTarget);
    ~layerRefreshIntervalCmd();
public:
    boost::tuple<DWORD ,DWORD> tupleTimes;
    CS3DrawObj *pcurObj;
    MEDIASETTING *pSetting;
public:
    int redo();
    int undo();
    int ondo();
};
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//layer text setting :font ,font size ,font color ,font style(bold,italy , under line)
//
class layerTextSettingCmd :public baseActionCmd
{
public:
    layerTextSettingCmd(int nID ,CWnd *pTarget);
    ~layerTextSettingCmd();
public:  
    CS3DrawObj *pcurObj;
    shared_ptr<S3SIGNAGE_TEXT_SETTING>pcurSetting;
    S3SIGNAGE_TEXT_SETTING  swapSetting;
public:
    BOOL SaveSwapSetting();
    BOOL IsNullSetting(S3SIGNAGE_TEXT_SETTING *pSetting);
    int ondo();
    int redo();
    int undo();
};  

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//Transition
//
class layerTransitionCmd :public baseActionCmd
{
public:
    layerTransitionCmd(int nID ,CWnd *pTarget);
    ~layerTransitionCmd();
public:
    CS3DrawObj              *pcurObj;
    S3SIGNAGE_TRANSIION     *pcurTrans;
    S3SIGNAGE_TRANSIION     swapTransition;
    BOOL                    bInOut;
public:
    BOOL SaveSwapTrans();
    BOOL IsNullTrans(S3SIGNAGE_TRANSIION *pTrans);
    int ondo();
    int redo();
    int undo();
};
//public function,expern for LayoutDesignerCmds
extern void ReselectObj(CS3DrawObjList *pSelList);
};