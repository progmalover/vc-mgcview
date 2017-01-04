#pragma once


#include "ComponentPane.h"
#include "MediaFileBrowserPane.h"
#include "LayoutPropertiesPane.h"
#include "LayoutTemplatesPane.h"
#include "LayoutDesignerCtrl.h"
#include "LayoutDesignerPane.h"
#include "S3DrawObj.h"
#include "ComponentListCtrl.h"
#include "LayoutSettings.h"

#ifdef _MEDIALIBRARY2
#include "MediaLibrary 2/MediaClient.h"
//#include "MediaLibrary 2/MediaLibrary.h"
#else
#include "MediaClient/MediaClient.h"
//#include "MediaLibrary/MediaLibrary.h"
#endif

#include "MediaLibraryInfoPane.h"
#include "MediaFileListCtrl.h"
#include "layoutdesignerctrl.h"
#include "propertysubcmds.h"
#include "LayoutElement.h"
extern UINT WM_ON_LAYOUT_OPEN;
// CDesignerPage
using namespace DESIGNERCMDS;
using namespace LAYOUTPROPERTYCMDS;
class CharacterInfo{
public:
    TCHAR   Char;  
    DWORD   Width;
    DWORD   Height;
    int     abcC;
    DWORD   XPos;
    DWORD   YPos;

    FRect   Texture;
};

static const COLORREF FillColor[8] =
{
    RGB(167, 224, 180),
    RGB(255, 128, 0),
    RGB(255, 128, 255),
    RGB(72, 164, 255),
    RGB(128, 128, 192),
    RGB(128, 0, 255),
    RGB(255, 0, 255),
    RGB(255, 128, 64)
};

class CDesignerPage : public CBaseControllerPage
{

protected:
    DECLARE_DYNAMIC(CDesignerPage)
	DELARE_FRIEND_LAYOUT_CMDS()
	DELARE_FRIEND_PROPERTY_CMDS()
    DECLARE_FRIEND_CLASS(LayoutWidget)
public:
    CDesignerPage();
    virtual ~CDesignerPage();


public:
    // Close the main designer window.
    void Close();

    // Create the ribbon buttons
    BOOL CreateRibbonButtons();


    BOOL UpdateOptions();
    BOOL CheckSupportFile(const CString &ext);

	/// Function to generate the text file for message show
	//HRESULT GenerateTextFile(MEDIASETTING* pMessage, IStream *pMemStream);
protected:
    /// Create / Delete draw obj
    void CreateRectDrawObj(const CRect rect, const PROPERTY_TYPE propertyType, const CString szMediaType, std::shared_ptr<IS3RODesigner2> pDesigner2 = NULL,
        std::shared_ptr<IS3RODesigner> pDesiger = NULL, std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >* pArrMedia = NULL);
    BOOL RemoveDrawObj(CS3DrawObj* pObj);
    BOOL RemoveLayoutSettingWithObj(CS3DrawObj* pObj);
    BOOL RemoveLayers();
    void RemoveAllDrawObj();
	void FocusLayoutNameEditCtrl();
    CS3DrawObj* GetSelectDrawObj();

    /// Function to implement the message
    void NewLayout(int width = 1920, int height = 1080, FLOAT fRotateDegree = 0.0f);

    /// Function to generate the text file for message show
    //HRESULT GenerateTextFile(MEDIASETTING* pMessage, IStream *pMemStream);

    /// Clean clip draw obj list
    void CleanClipDrawObj();

    /// Query Plugin related info
    HRESULT QueryDesignerWithName(CString szName, std::shared_ptr<IS3RODesigner2>& pDesigner2, std::shared_ptr<IS3RODesigner>& pDesigner);
    HRESULT QueryMediaType(const CString szFilePath, CString& szType);

    // Read/Write layout file
    HRESULT CloseLayout();

	//used only in StandaloneEdition
	HRESULT Standalone_LoadLayoutFile(CString DirPath, const CString& LayoutName = _T(""), int GroupID = -1, CString ContentXML = _T(""));

    HRESULT LoadLayoutFile(CString Filename, const CString& LayoutName = _T(""), int GroupID = -1, CString ContentXML = _T(""));
    HRESULT WriteLayoutFile(CString Filename, CString &ContentXML, BOOL bExportToDir = false);
    //HRESULT UploadMediaFiles(LAYOUT* m_pLayoutSetting, CString szlayoutName);
    HRESULT DownloadMediaFiles(CString MediaServerFolder, CString LocalFolder);

    //HRESULT ParserLayer(LAYOUT *pLayoutSetting, TiXmlElement *pLayerElement);
    //HRESULT ParserContent(MEDIASETTING *pContent, TiXmlElement *pContentElement, const PROPERTY_TYPE& propertyType);
    //HRESULT ParserTranstion(S3SIGNAGE_TRANSIION *pTransition, TiXmlElement *pTransitionElement);
    //HRESULT ParserTextFile(MEDIASETTING *pContent, TiXmlElement *pTextFileElement);

    void InitLayout();

    HRESULT CheckLayout(int& layoutID);

    void GetMediaServerLayoutPath(CString& szLayoutPath);
    void GetLocalTempPath(CString& szTempPath);

    // Data Transfer in load/write files
    void ConvertBOOL2Str(BOOL bValue, char* pValue);
    void ConvertDuration2Str(DWORD duration, char* pValue);

    // Remove media files physical path for publish layout
    HRESULT RemoveMediaPhysicalPath();

    // Generate the layout xml file and thumbnail
    HRESULT PackLayout(const CString szLayoutPath, CString& szLayoutFile, CString& szLayoutThumbnail);

    // Layout Groups
    void RemoveLayoutGroups();

    // Preview layout
    HRESULT PreviewLayout(const int nWidth, const int nHeight, CString szLayoutThumbnail = _T(""));

    void AddLayerDuration(CS3DrawObj* pObj, const DWORD duration);
    BOOL GetMediaIDListFromLayout(LayoutElement *pLayoutInfo, vector<int>& MediaIDs);

	void SetXForm(INT degree, XFORM *ptXForm);
	void SetLayerInfo(CS3DrawRectObj* pObj, const CString &szTypeName, const FRect &rectLogic, const PROPERTY_TYPE propertyType, const CString szMediaType, std::shared_ptr<IS3RODesigner2> pDesigner2,
		std::shared_ptr<IS3RODesigner> pDesigner, std::vector<std::pair<CString, CLayoutDesignerCtrl::DragMediaInfo> >* pMedia);
    void SetLayerInfoByObj(CS3DrawObj* pObj, std::shared_ptr<LayerElement> layerInfo);
	void SetObjInfoByLayerInfo();
public:
    /// To handle the ribbon button message
    void OnRibbonBtnNew();
    void OnRibbonBtnClose();
    void OnRibbonBtnPublish();
    void OnRibbonBtnExport();
    void OnRibbonBtnImport();
    void OnRibbonBtnUndo();
    void OnRibbonBtnRedo();
    void OnRibbonBtnDeleteItem();
    void OnRibbonBtnTabChange();
    void OnRibbonBtnMoveLeft();
    void OnRibbonBtnMoveRight();
    void OnRibbonBtnMoveUp();
    void OnRibbonBtnMoveDown();
    void OnRibbonBtnCopy();
    void OnRibbonBtnCut();
    void OnRibbonBtnPaste();
    void OnRibbonBtnSelectAll();
    void OnRibbonBtnPreviewWinMode();
    void OnRibbonBtnPreview();
    void OnRibbonBtnRefresh();
    void OnRibbonBtnMoveToFront();
    void OnRibbonBtnMoveForward();
    void OnRibbonBtnMoveToBack();
    void OnRibbonBtnMoveBackward();
    void OnRibbonBtnZoom();
    void OnRibbonBtnPreview10X7();
    void OnRibbonBtnPreview12X10();
    void OnRibbonBtnPreview19X12();
	void OnEditUndo();
	void OnEditRedo();
	void OnRibbonBtnBindLay();
    void OnUpdateRibbonBtnPreview(CCmdUI* pCmdUI);

    void OnRibbonBtnSameIntervalX();
    void OnRibbonBtnSameIntervalY();
    void OnRibbonBtnSameFillLayout();
    void OnRibbonBtnUpdateSameInterval(CCmdUI *);
    void OnRibbonBtnUpdateFillLayout(CCmdUI *);

    void CreateLayout(INT GroupID);
    void ImportLayout(int groupId, const CString& path);
	//void FocusLayoutNameEditCtrl();

	BOOL GetMediaSettingsByObj(CS3DrawObj* pObj, vector<std::shared_ptr<MediaElement>> * &mediaList);
	BOOL GetLayerInfoByObj(CS3DrawObj* pObj, std::shared_ptr<LayerElement> &layerInfo);
	BOOL AddLayerIntoLayout(CS3DrawObj * pInstance, std::shared_ptr<LayerElement> &layerInfo);
    void AdjustLayerInfoBySortedObjs(BOOL bRevert = FALSE);

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    afx_msg LRESULT OnLayoutZoom(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayoutSelectObj(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayoutRemoveObj(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayoutInsertObj(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayoutAction(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnObjAddMedia(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayoutSizeChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayerSizeChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLayerSizeChange2(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayerMediaSelChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayoutOpen(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayerLoopChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayerSelChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnLayerNameChange(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSimplePluginSizeChange(WPARAM wParam, LPARAM lParam);

#ifndef STARTER_EDITION
    afx_msg LRESULT MediaCategoryListSelChange(WPARAM wp, LPARAM lp);
#endif

    afx_msg void OnUpdateRibbonBtn(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedSelect(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedCopied(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedLayers(CCmdUI* pCmdUI);
    
	//afx_msg void OnUpdateRibbonBtnNeedMove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnCanMoveForward(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonBtnCanMoveBackward(CCmdUI* pCmdUI);

    afx_msg void OnUpdateRibbonBtnCreateLayout(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnCloseLayout(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnNeedSelectCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBindLayBtn(CCmdUI *pCmdUI);
    afx_msg void OnUpdateRibbonBtnPublish(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRibbonBtnMgrLayout(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);

	afx_msg LRESULT OnDesignerKeyDown (WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnDestroy();

protected:
    // Pane
    CMediaFileBrowserPane                   m_wndMediaFileBrowser;
    CBaseControllerPane                     m_wndMediaFileExplorer;
    CComponentPane                          m_wndComponentPane;
    CLayoutPropertiesPane                   m_wndLayoutProperties;
    CLayoutTemplatesPane                    m_wndLayoutTemplates;
    CLayoutDesignerPane                     m_wndLayoutDesign;
    CMediaLibraryInfoPane                   m_wndMediaInfo;

    // Ctrl
    CMediaFileListCtrl                      m_wndFileList;
    CComponentListCtrl                      m_wndComponentList;

    CMFCRibbonComboBox*                     m_pComboBoxZoom;

    /// Draw Obj info
    CS3DrawObjList                          m_DrawObjList;
    CS3DrawObjList                          m_ClipObjList;

    /// Layout Info
    CSize                                   m_layoutSize;
    UINT                                    m_nDepth;
    FLOAT                                   m_fRotateDegree;

    //LAYOUT*                                 m_pLayoutSetting;
	LayoutElement						    *m_LayoutElement;

    // Renderable obj
    //std::vector<PLUGIN_INFO*>                                           m_arrRenderableObj;
    //std::map<CString, IID>                                              m_PluginObj;
    //std::map<CString, PLUGIN_SUPPORTFILES*>                             m_PluginType;
   // CMap<CString, LPCTSTR, UINT, UINT>                                  m_ComponentObjCount;
    CMap<CString, LPCTSTR, COLORREF, COLORREF>                          m_ObjFillColor;
    CMap<CString, LPCTSTR, DWORD, DWORD>                                m_ObjTypeCount;
    CMap<CS3DrawObj*, CS3DrawObj*, UINT, UINT>                          m_ObjItemCount;
    CMap<CS3DrawObj*, CS3DrawObj*, DWORD, DWORD>                        m_layerDuration;
   // CMap<CS3DrawObj*, CS3DrawObj*, DWORD, DWORD>                        m_layerRefreshInterval;
     // Layout Group
    LAYOUTGROUP_LIST                                                    m_LayoutGroups;

    // Losed media contents when open or preview the layout
    CStringArray                                                        m_arrLosedFiles;
    // Suspicion media contents when open or preview the layout
    CStringArray                                                        m_arrSuspicionFiles;
public:
	CSize GetCurLayoutSize();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
