#pragma once

#include "LayoutSettings.h"
#include "BaseDropTarget.h"
#include "LayerElement.h"
// CMediaListBox
extern UINT WM_ON_MEDIA_NUM_CHANGE;
class CMediaListBox;
 

class CMediaListBox : public CVSListBox 
{
	friend class CVSDropTarget;
public:
    CMediaListBox();
    virtual ~CMediaListBox();

    void SetContent(std::shared_ptr<LayerElement> layerInfo, int pos = 0);
    DROPEFFECT  CanAccept(CString strId);
protected:
    afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnInitControl(WPARAM wParam, LPARAM lParam);
    BOOL MySetStandardButtons(UINT uiBtns = AFX_VSLISTBOX_BTN_NEW | AFX_VSLISTBOX_BTN_DELETE | AFX_VSLISTBOX_BTN_UP | AFX_VSLISTBOX_BTN_DOWN);
    DECLARE_MESSAGE_MAP()
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    virtual void OnClickButton(int iButton);
    virtual BOOL EditItem(int iIndex);
	
    //LAYERINFO* m_pLayer;
	std::shared_ptr<LayerElement> m_layerInfo;
    void UpdateButtons();
    void SwapLayer(int pos1, int pos2);
	
private:
	//// OLE drag/drop support
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	LRESULT OnAddDropedObject(CS3OleDropInfo *,CString strId);
protected:
	class CVSDropTarget :public COleDropTarget
	{
		public:
			CVSDropTarget():COleDropTarget(){};
			~CVSDropTarget(){};
		protected:
		CMediaListBox *WndConvert(CWnd * pWnd)
		{
			if( !pWnd || !IsWindow(pWnd->m_hWnd))
				return NULL;
			if (!pWnd->IsKindOf(RUNTIME_CLASS(CMediaListBox)))
			return NULL;

			return (CMediaListBox *)pWnd;
		}
		virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
			DWORD dwKeyState, CPoint point)
		{
			CMediaListBox* pList = WndConvert(pWnd);
			if( !pList)
				return DROPEFFECT_NONE;
			return pList->OnDragOver(pDataObject, dwKeyState, point);
		}
		virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
			DROPEFFECT dropEffect, CPoint point)
		{
			CMediaListBox* pList = WndConvert(pWnd);
			if( !pList)
				return DROPEFFECT_NONE;
			return pList->OnDrop(pDataObject, dropEffect, point);
		}

	};

	CVSDropTarget       m_DragTarget;
    CBaseDropTarget     m_BaseDragTarget;                   /// for drop target functionality

};