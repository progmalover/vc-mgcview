#include "stdafx.h"
#include "ScheduleCtrlCmds.h"
#include "ScheduleCtrl.h"
#include "AppointmentsManager.h"
#include "SchedulerPage.h"
#include ".\BorrowSkin\BORROW_DateTimeCtrl.h"

namespace _SCHEDULECTRLCMDS
{


	bool ScheduleCmdEngine::bindTarget(CWnd *pTargetWnd)
	{
		__super::bindTarget(pTargetWnd);
		m_pFactory->bind(this);
		return true;
	}

	/*
	Schedule Cmd Factor.
	Create cmd by id ,and hold suspend cmd.
	*/
	ScheduleCmdFactory::ScheduleCmdFactory(){
		m_pBuffCmd = NULL;
	};
	ScheduleCmdFactory::~ScheduleCmdFactory(){};
	_baseCmd * ScheduleCmdFactory::newCmdBegin(int nID)
	{
		if(m_pBuffCmd)
		{
			TRACE(_T("Bufferd Cmd is existing!\n"));
			return NULL;
		}
		this->m_pBuffCmd = newCmd(nID);
		return m_pBuffCmd;
	}
	_baseCmd * ScheduleCmdFactory::getBuffedCmd()
	{
		return (_baseCmd *)m_pBuffCmd;
	}
	_baseCmd * ScheduleCmdFactory::newCmdEnd()
	{
		_baseCmd *pCmd = m_pBuffCmd;
		m_pBuffCmd = NULL;
		return (_baseCmd *)pCmd;
	}
	_baseCmd * ScheduleCmdFactory::newCmd(int nID)
	{
		_baseCmd *pNewCmd = NULL;
		ASSERT(m_pOwner);
		CWnd *pTargetWnd = m_pOwner->getTargetWnd();

		ASSERT(pTargetWnd);
		switch(nID)
		{
		case CMD_SCHEDULE_ALIGN:
			//pNewCmd = new schedulea
			pNewCmd = new scheduleAlignCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_CREATE:
			pNewCmd = new scheduleCreateCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_DELETE:
			pNewCmd = new scheduleDeleteCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_DRAG:
			pNewCmd = new scheduleDragCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_DURATION_CHANGE:
			pNewCmd = new scheduleDurationChangeCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_HPASTE:
			pNewCmd = new scheduleHPasteCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_PASTE:
			pNewCmd = new schedulePasteCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_VPASTE:
			pNewCmd = new scheduleVPasteCmd(nID,pTargetWnd);
			break;
		case CMD_SCHEDULE_SUB_DELETE:
			pNewCmd = new scheduleSubDeleteCmd(nID,pTargetWnd);
			break;
		default: // unknown cmd id.
			ASSERT(0);
			return NULL;
		}
		return (_baseCmd *)pNewCmd;
	}

	bool ScheduleCmdFactory::delCmd(_baseCmd *pCmd)
	{
		__super::delCmd(pCmd);

		ASSERT(pCmd);
		switch(pCmd->getID())
		{
		case CMD_SCHEDULE_CREATE:
			delete (scheduleCreateCmd *)pCmd;
			break;
		case CMD_SCHEDULE_DELETE:
			delete (scheduleDeleteCmd *)pCmd;
			break;
		case CMD_SCHEDULE_ALIGN:
			delete (scheduleAlignCmd *)pCmd;
			break;
		case CMD_SCHEDULE_DRAG:
			delete (scheduleDragCmd *)pCmd;
			break;
		case CMD_SCHEDULE_DURATION_CHANGE:
			delete (scheduleDurationChangeCmd *)pCmd;
			break;
		case CMD_SCHEDULE_HPASTE:
			delete (scheduleHPasteCmd *)pCmd;
			break;
		case CMD_SCHEDULE_PASTE:
			delete (schedulePasteCmd *)pCmd;
			break;
		case CMD_SCHEDULE_VPASTE:
			delete (scheduleVPasteCmd *)pCmd;
			break;
		case CMD_SCHEDULE_SUB_DELETE:
			delete (scheduleSubDeleteCmd *)pCmd;
			break;
		default:
			TRACE(_T("Try to free unknown cmd class!"));
			this->FreeBaseCmd(pCmd);
			break;
		}
		return true;
	}

	LPCTSTR ScheduleCmdFactory::getCmdName(_baseCmd *)
	{
		return _T("");
	}

	CWnd* ScheduleCmdFactory::getTargetWnd()
	{
		return NULL;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//functions for cmds
void CopyAppointmentList(AppointmentList &targetList,AppointmentList &srcList)
{
	POSITION pos = srcList.GetHeadPosition();
	while(pos)
	{
		Appointment *pApp = srcList.GetAt(pos);
		targetList.AddTail(pApp);
		srcList.GetNext(pos);
	}
}

void UnselectAppointments( AppointmentList &Items,BOOL bEnable=TRUE)
{
	BOOL bSelect = bEnable ? FALSE:TRUE;
	POSITION pos = Items.GetHeadPosition();
	while(pos)
	{
		Appointment *pApp = Items.GetAt(pos);
		pApp->SetSelected(bSelect);
		Items.GetNext(pos);
	}

}

void  UpdateAppointment( ScheduleCtrl *pSchedul,Appointment *pApp,COleDateTime &dtStart,COleDateTime &dtEnd)
{
	//ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
	class ScheduleCtrlPrivate  : public ScheduleCtrl
	{
		friend void  UpdateAppointment( ScheduleCtrl *,Appointment *,COleDateTime &,COleDateTime &);
	};

	ScheduleCtrlPrivate *pSchedlPri = (ScheduleCtrlPrivate *)pSchedul;
	ASSERT(pSchedlPri);
	ASSERT_VALID (pApp);
	pApp->SetStart(dtStart);
	pApp->SetEnd(dtEnd);

	// UpdateIcon
	if (pApp->GetImageList())
	{
		UINT Index;
		if (!pSchedlPri->m_IconDetailIDMapper.Lookup(pApp->GetDetailID(), Index))
		{
			CBitmap bitmap;
			pSchedlPri->GetListImage(*pApp->GetImageList(), bitmap, pApp->GetImageIndex());
			Index = pSchedlPri->m_ScheduleItemIconList.Add(&bitmap, RGB(255,255,255));

			pSchedlPri->m_IconDetailIDMapper.SetAt(pApp->GetDetailID(), Index);
		}

		pApp->SetImage(&pSchedlPri->m_ScheduleItemIconList, Index);
	}

	pSchedlPri->QueryAppointments(FALSE);
	pSchedlPri->AddJustCurrentViewLayout(FALSE);
	GetCurrentPageCtrl(RUNTIME_CLASS(CSchedulerPage))->SendMessage
		(SCHEDULE_UPDATE_APPOINTMENT, (WPARAM) FALSE, (LPARAM) pApp);
	//pSchedul->GetCurrentView ()->AdjustLayout (FALSE);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//
	// "basecmd" to record / undo/redo current date range of schedulerctrl.
	//
	_scheduleBaseCmd::_scheduleBaseCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){};
	_scheduleBaseCmd::~_scheduleBaseCmd(){};

	int _scheduleBaseCmd::undo()
	{
		__super::undo();

		SUB_CMDS &subCmds = getSubCmds();
		if(subCmds.size())
		{
			SUB_CMDS::reverse_iterator it = subCmds.rbegin();
			for( ;it != subCmds.rend();it++)
			{
				_baseCmd *pCmd = *it;
				pCmd->undo();
			}
		}

        ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
        ASSERT(pSchedul);
        pSchedul->SetDateInterval(_DateBegin,_DateEnd);
        pSchedul->pEndTimeCtrl->SetDate(_DateEnd.m_dt);
        pSchedul->pStartTimeCtrl->SetDate(_DateBegin.m_dt);

		return SUCCESS;
	}

	int _scheduleBaseCmd::redo()
	{
		__super::redo();

		SUB_CMDS &subCmds = getSubCmds();
		if(subCmds.size())
		{
			SUB_CMDS::iterator it = subCmds.begin();
			for( ;it != subCmds.end();it++)
			{
				_baseCmd *pCmd = *it;
				pCmd->redo();
			}
		}

        ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
        ASSERT(pSchedul);
        pSchedul->SetDateInterval(_DateBegin,_DateEnd);
        pSchedul->pEndTimeCtrl->SetDate(_DateEnd.m_dt);
        pSchedul->pStartTimeCtrl->SetDate(_DateBegin.m_dt);
		return SUCCESS;
	}

	int _scheduleBaseCmd::ondo()
	{
		__super::ondo();

		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		pSchedul->GetDateInterval(_DateBegin,_DateEnd);

		return SUCCESS;
	}

	//
	//Create or undo/redo "create" function
	//
	scheduleCreateCmd::scheduleCreateCmd(int nID,CWnd *pTarget):_scheduleBaseCmd(nID,pTarget){}
	scheduleCreateCmd::~scheduleCreateCmd(){
		if(this->m_nOpt == OPT_REDO) //in redo stack.
		{
			POSITION pos = _Appointments.GetHeadPosition();
			while(pos)
			{
				Appointment *pApp = _Appointments.GetAt(pos);
				delete pApp;
				_Appointments.GetNext(pos);
			}
		}
	}

	int scheduleCreateCmd::undo()
	{
		__super::undo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		ASSERT(_Appointments.GetCount()> 0);
		
		//
		//restore selection status.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		AppointmentsManager &Items = pSchedul->m_AppsManager;

		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pApp->SetSelected(FALSE);
			pSchedul->RemoveAppointment(pApp);
			_Appointments.GetNext(pos);
		}
		Items.m_bAutoDelete = bSaveVal;
		GetCurrentPageCtrl(RUNTIME_CLASS(CSchedulerPage))->SendMessage
			(SCHEDULE_SELECT_APPOINTMENT, (WPARAM) 0, (LPARAM) 0);
		return SUCCESS;
	}

	int scheduleCreateCmd::redo()
	{
		__super::redo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		ASSERT(_Appointments.GetCount()> 0);
		//AppointmentArray &Items = pSchedul->m_QueryAppsArray;

		//
		//restore selection status.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pSchedul->AddAppointment(pApp);
			pApp->SetSelected(TRUE);
			 
			_Appointments.GetNext(pos);
		}

		//
		//restore selection status.
		//
		CopyAppointmentList(selList,_Appointments);
		GetCurrentPageCtrl(RUNTIME_CLASS(CSchedulerPage))->SendMessage
			(SCHEDULE_SELECT_APPOINTMENT, (WPARAM) 0, (LPARAM) 0);

	//	pSchedul->Invalidate();
		return SUCCESS;
	}



	int scheduleCreateCmd::ondo()
	{
		__super::ondo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		if(_Appointments.GetCount() <= 0)
		{
			AppointmentList &selList = pSchedul->m_SelectedAppsList;
			if(selList.GetCount() <= 0)
				return FAILURE;
			CopyAppointmentList(_Appointments,selList);
		}
		
		return SUCCESS;
	}

	/*
	schedule delete
	*/
	scheduleDeleteCmd::scheduleDeleteCmd(int nID,CWnd *pTarget):_scheduleBaseCmd(nID,pTarget){}
	//
	//free objects when cmd in undo stack.
	//
	scheduleDeleteCmd::~scheduleDeleteCmd(){
		if(this->m_nOpt == OPT_UNDO)
		{
			if(_Appointments.GetCount() <= 0)
				return;

			POSITION pos = _Appointments.GetHeadPosition();
			while(pos)
			{
				Appointment *pApp = _Appointments.GetAt(pos);
				delete pApp;
				_Appointments.GetNext(pos);
			}
		}
	}
 
	int scheduleDeleteCmd::undo()
	{
		__super::undo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		
		ASSERT(pSchedul);
		if(_Appointments.GetCount() <= 0)
			return FAILURE;
		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pSchedul->AddAppointment(pApp,1,0,1);
			COleDateTime dtStart = pApp->GetStart();
			COleDateTime dtEnd = pApp->GetFinish();
			UpdateAppointment(pSchedul,pApp, dtStart,dtEnd);
			_Appointments.GetNext(pos);
		}	

		//
		//restore selection
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		CopyAppointmentList(selList,_Appointments);
		UnselectAppointments(selList,FALSE);
		pSchedul->Invalidate();
		return SUCCESS;
	}

	int scheduleDeleteCmd::redo()
	{
		__super::redo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
	
		if(_Appointments.GetCount() <= 0)
			return FAILURE;

		//
		//restore selection first.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();
		CopyAppointmentList(selList,_Appointments);

		AppointmentsManager &Items = pSchedul->m_AppsManager;
		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		pSchedul->RemoveSelectedAppointments(TRUE);
		Items.m_bAutoDelete = bSaveVal;

		GetCurrentPageCtrl(RUNTIME_CLASS(CSchedulerPage))->SendMessage
			(SCHEDULE_SELECT_APPOINTMENT, (WPARAM) 0, (LPARAM) 0);
		return SUCCESS;
	}

	int  scheduleDeleteCmd::ondo()
	{
		__super::ondo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		if(_Appointments.GetCount() <= 0)
		{
			if(selList.GetCount() <= 0)
				return FAILURE;
			CopyAppointmentList(_Appointments,selList);
		}else//for reuse in unselection mode.
		{
			UnselectAppointments(selList);
			selList.RemoveAll();
			CopyAppointmentList(selList,_Appointments);
		}

		AppointmentsManager &Items = pSchedul->m_AppsManager;
		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		pSchedul->RemoveSelectedAppointments(TRUE);
		Items.m_bAutoDelete = bSaveVal;
		 
		return SUCCESS;
	}

	/*
	schedule drag and drop.
	*/
	scheduleDragCmd::scheduleDragCmd(int nID,CWnd *pTarget):_scheduleBaseCmd(nID,pTarget){}
	scheduleDragCmd::~scheduleDragCmd(){}

	void scheduleDragCmd::SaveBeforeDrag()
	{
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		if(selList.GetCount() <= 0)
		 return;

		POSITION pos = selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = selList.GetAt(pos);
			APPOINTMENT_INFO info;
			info.pAppointment = pApp;
			info.dtStart = pApp->GetStart();
			info.dtEnd = pApp->GetFinish();
			_StoreInfo.push_back(info);

			selList.GetNext(pos);

		}
	}

	int scheduleDragCmd::undo()
	{
		__super::undo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		//
		//exchange data,and restore selection.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		//Not delete object,only remove pointer.
		AppointmentsManager &Items = pSchedul->m_AppsManager;
		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;

		//first remove all selected items from control.
		for(int i = 0;i < _StoreInfo.size();i++)
		{
			//Items.Remove(_StoreInfo[i].pAppointment);
			pSchedul->RemoveAppointment(_StoreInfo[i].pAppointment,0,0);
		}
		pSchedul->QueryAppointments(FALSE);
		//second, at itmes to new place again.
		for(int i = 0;i < _StoreInfo.size();i++)
		{
			APPOINTMENT_INFO &info = _StoreInfo[i];
			selList.AddTail(info.pAppointment);
			info.pAppointment->SetSelected(TRUE);

			COleDateTime  curStart,curEnd;
			curStart = info.pAppointment->GetStart();
			curEnd = info.pAppointment->GetFinish();
		
			//info.pAppointment->SetInterval(info.dtStart,info.dtEnd);
			info.pAppointment->SetStart(info.dtStart);
			info.pAppointment->SetEnd(info.dtEnd);
			pSchedul->AddAppointment(info.pAppointment,1,0,1);	
			UpdateAppointment(pSchedul,info.pAppointment,info.dtStart,info.dtEnd);
			info.dtStart = curStart;
			info.dtEnd = curEnd;

		}
		Items.m_bAutoDelete  = bSaveVal;
		pSchedul->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		return SUCCESS;
	}

	int scheduleDragCmd::redo()
	{
		__super::redo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		//
		//exchange data,and restore selection.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		//second, at itmes to new place again.
		//Not delete object,only remove pointer.
		AppointmentsManager &Items = pSchedul->m_AppsManager;
		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;

		//first remove all selected items from control.
		for(int i = 0;i < _StoreInfo.size();i++)
		{
			pSchedul->RemoveAppointment(_StoreInfo[i].pAppointment,0,0);
		}
		pSchedul->QueryAppointments(FALSE);
		for(int i = 0;i < _StoreInfo.size();i++)
		{
			APPOINTMENT_INFO &info = _StoreInfo[i];
			selList.AddTail(info.pAppointment);
			info.pAppointment->SetSelected(TRUE);

			COleDateTime  orgStart,orgEnd;
			orgStart = info.pAppointment->GetStart();
			orgEnd = info.pAppointment->GetFinish();

		    info.pAppointment->SetStart(info.dtStart);
		    info.pAppointment->SetEnd(info.dtEnd);
			pSchedul->AddAppointment(info.pAppointment,1,0,1);
			UpdateAppointment(pSchedul,info.pAppointment,info.dtStart,info.dtEnd);
			info.dtStart = orgStart;
			info.dtEnd = orgEnd;

			//info.pAppointment->setr
		}		
		Items.m_bAutoDelete  = bSaveVal;
		pSchedul->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		return SUCCESS;
	}

	int scheduleDragCmd::ondo()
	{
		__super::ondo();
		if(this->_StoreInfo.size() <= 0)
			return FAILURE;

		APPOINTMENT_INFO info = _StoreInfo[0];
		COleDateTimeSpan span =  info.pAppointment->GetStart() - info.dtStart; 
		if(abs(span.GetTotalSeconds()) <= 2)
			return FAILURE;
		
#if 0
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);

		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		if(selList.GetCount() <= 0)
			return FAILURE;
		POSITION pos = selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = selList.GetAt(pos);

			APPOINTMENT_INFO info;
			info.pAppointment = pApp;
			info.dtStart = pApp->GetStart();
			info.dtEnd = pApp->GetFinish();
			selList.GetNext(pos);
		}
#endif
		return SUCCESS;
	}

	/*
	schedule duration change.
	*/
	scheduleDurationChangeCmd::scheduleDurationChangeCmd(int nID,CWnd *pTarget):_scheduleBaseCmd(nID,pTarget){}
	scheduleDurationChangeCmd::~scheduleDurationChangeCmd(){}

	void scheduleDurationChangeCmd::SaveBeforeDrag()
	{
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		if(selList.GetCount() <= 0)
		 return;

		POSITION pos = selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = selList.GetAt(pos);
			APPOINTMENT_INFO info;
			info.pAppointment = pApp;
			info.dtStart = pApp->GetStart();
			info.dtEnd = pApp->GetFinish();
			_StoreInfo.push_back(info);

			selList.GetNext(pos);

		}
	}

	int scheduleDurationChangeCmd::undo()
	{
		__super::undo();
			ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		//
		//exchange data,and restore selection.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		for(int i = 0;i < _StoreInfo.size();i++)
		{
			APPOINTMENT_INFO &info = _StoreInfo[i];
			selList.AddTail(info.pAppointment);
			info.pAppointment->SetSelected(TRUE);

			COleDateTime  curStart,curEnd;
			curStart = info.pAppointment->GetStart();
			curEnd = info.pAppointment->GetFinish();

			UpdateAppointment(pSchedul,info.pAppointment,info.dtStart,info.dtEnd);

			info.dtStart = curStart;
			info.dtEnd = curEnd;

		}
		pSchedul->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		return SUCCESS;
	}

	int scheduleDurationChangeCmd::redo()
	{
		__super::redo();

		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		//
		//exchange data,and restore selection.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		for(int i = 0;i < _StoreInfo.size();i++)
		{
			APPOINTMENT_INFO &info = _StoreInfo[i];
			selList.AddTail(info.pAppointment);
			info.pAppointment->SetSelected(TRUE);

			COleDateTime  orgStart,orgEnd;
			orgStart = info.pAppointment->GetStart();
			orgEnd = info.pAppointment->GetFinish();

			UpdateAppointment(pSchedul,info.pAppointment,info.dtStart,info.dtEnd);

			info.dtStart = orgStart;
			info.dtEnd = orgEnd;

			//info.pAppointment->setr
		}		
		pSchedul->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		return SUCCESS;
	}
	int scheduleDurationChangeCmd::ondo()
	{
		__super::ondo();

		if(this->_StoreInfo.size() <= 0)
			return FAILURE;
		return SUCCESS;
	}

	/*
		paste cmds
		hold new created objects,and make undo/redo effect.
	*/
	schedulePasteCmd::schedulePasteCmd(int nID,CWnd *pTarget):_scheduleBaseCmd(nID,pTarget){}
	schedulePasteCmd::~schedulePasteCmd(){
		if(this->m_nOpt == OPT_REDO)
		{
			POSITION pos = _PasteList.GetHeadPosition();
			while(pos)
			{
				Appointment *pApp = _PasteList.GetAt(pos);
				delete pApp;
				_PasteList.GetNext(pos);
			}
		}
	}

	int schedulePasteCmd::undo()
	{
		__super::undo();

		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);

		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		if(_PasteList.GetSize() <= 0)
			return FAILURE;

		AppointmentsManager &Items = pSchedul->m_AppsManager;
		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		
		POSITION pos = _PasteList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _PasteList.GetAt(pos);
			pSchedul->RemoveAppointment(pApp,1,0);
			_PasteList.GetNext(pos);
		}

		Items.m_bAutoDelete = bSaveVal;
		pSchedul->RedrawWindow();
		return SUCCESS;
	}

	int schedulePasteCmd::redo()
	{
		__super::redo();

		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		if(_PasteList.GetSize() <= 0)
			return FAILURE;
		
		POSITION pos = _PasteList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _PasteList.GetAt(pos);
			pSchedul->AddAppointment(pApp,1,0);
			_PasteList.GetNext(pos);
		}
		pSchedul->RedrawWindow();
		return SUCCESS;
	}

	int schedulePasteCmd::ondo()
	{
		__super::ondo();

		if(this->_PasteList.GetSize()<= 0)
			return FAILURE;

		return SUCCESS;
	}

	//
	//paste across  several  days.
	//
	scheduleVPasteCmd::scheduleVPasteCmd(int nID,CWnd *pTarget):schedulePasteCmd(nID,pTarget){}
	scheduleVPasteCmd::~scheduleVPasteCmd(){}

	int scheduleVPasteCmd::undo()
	{
		return __super::undo();
	}
	int scheduleVPasteCmd::redo()
	{
		return __super::redo();
	}

	int scheduleVPasteCmd::ondo()
	{
		__super::ondo();
		return SUCCESS;
	}

	//
	//Paste in a day.
	//
	scheduleHPasteCmd::scheduleHPasteCmd(int nID,CWnd *pTarget):schedulePasteCmd(nID,pTarget){};
	scheduleHPasteCmd::~scheduleHPasteCmd(){}
	int scheduleHPasteCmd::undo()
	{

		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);

		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		if(_PasteList.GetSize() <= 0)
		{
			ASSERT(FALSE);
			return FAILURE;
		}
		AppointmentsManager &Items = pSchedul->m_AppsManager;
		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		POSITION pos = _PasteList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _PasteList.GetAt(pos);
			pSchedul->RemoveAppointment(pApp,1,0);
			_PasteList.GetNext(pos);
		}

		Items.m_bAutoDelete = bSaveVal;
		
		return __super::undo();
	}

	int scheduleHPasteCmd::redo()
	{
		return __super::redo();
	}
	int scheduleHPasteCmd::ondo()
	{
		__super::ondo();
		return SUCCESS;
	}

	/*
	align cmd
	*/
	scheduleAlignCmd::scheduleAlignCmd(int nID,CWnd *pTarget):_scheduleBaseCmd(nID,pTarget){
		_AlignDirection = ALIGN_UP;
	}
	scheduleAlignCmd::~scheduleAlignCmd(){}
	int scheduleAlignCmd::undo()
	{
		__super::undo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		//
		//exchange data,and restore selection.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		for(int i = 0;i < _StoreInfo.size();i++)
		{
			APPOINTMENT_INFO &info = _StoreInfo[i];
			selList.AddTail(info.pAppointment);
			info.pAppointment->SetSelected(TRUE);

			COleDateTime  curStart,curEnd;
			curStart = info.pAppointment->GetStart();
			curEnd = info.pAppointment->GetFinish();

			pSchedul->UpdateAppointment(info.pAppointment,info.dtStart,info.dtEnd);

			info.dtStart = curStart;
			info.dtEnd = curEnd;
			
		}

		return SUCCESS;
	}

	int scheduleAlignCmd::redo()
	{
		__super::redo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		//
		//exchange data,and restore selection.
		//
		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		UnselectAppointments(selList);
		selList.RemoveAll();

		for(int i = 0;i < _StoreInfo.size();i++)
		{
			APPOINTMENT_INFO &info = _StoreInfo[i];
			selList.AddTail(info.pAppointment);
			info.pAppointment->SetSelected(TRUE);

			COleDateTime  orgStart,orgEnd;
			orgStart = info.pAppointment->GetStart();
			orgEnd = info.pAppointment->GetFinish();

			pSchedul->UpdateAppointment(info.pAppointment,info.dtStart,info.dtEnd);

			info.dtStart = orgStart;
			info.dtEnd = orgEnd;

			//info.pAppointment->setr
		}
		return SUCCESS;
	}

	//
	//exec this cmd,before align start.
	//
	int scheduleAlignCmd::ondo()
	{
		__super::ondo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);

		AppointmentList &selList = pSchedul->m_SelectedAppsList;
		if(selList.GetCount() <= 0)
			return FAILURE;
		POSITION pos = selList.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = selList.GetAt(pos);

			APPOINTMENT_INFO info;
			info.pAppointment = pApp;
			info.dtStart = pApp->GetStart();
			info.dtEnd = pApp->GetFinish();

			this->_StoreInfo.push_back(info);
			selList.GetNext(pos);
		}

		//
		//start align here.
		//
		if( _AlignDirection == ALIGN_UP)
			pSchedul->AlignUpSelectedAppointments(1);
		else
			pSchedul->AlignDownSelectedAppointments(1);
		return SUCCESS;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//sub cmds

	scheduleSubDeleteCmd:: scheduleSubDeleteCmd(int nID,CWnd *pTarget):_baseCmd(nID,pTarget){
	}
	scheduleSubDeleteCmd:: ~scheduleSubDeleteCmd()
	{
		if(this->m_nOpt == OPT_UNDO) //in redo stack.
		{
			POSITION pos = _Appointments.GetHeadPosition();
			while(pos)
			{
				Appointment *pApp = _Appointments.GetAt(pos);
				delete pApp;
				_Appointments.GetNext(pos);
			}
		}
	}

	int scheduleSubDeleteCmd::RemoveAppointment()
	{
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		AppointmentsManager &Items = pSchedul->m_AppsManager;
		BOOL bSaveVal = Items.m_bAutoDelete;
		Items.m_bAutoDelete = FALSE;
		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pSchedul->RemoveAppointment(pApp,1,0);
			_Appointments.GetNext(pos);
		}
		Items.m_bAutoDelete = bSaveVal;
		return SUCCESS;
	}

	int scheduleSubDeleteCmd::ondo()
	{
		__super::ondo();
		if(this->_Appointments.GetSize() <= 0)
			return FAILURE;
		RemoveAppointment();
		return SUCCESS;
	}

	int scheduleSubDeleteCmd::redo()
	{
		__super::redo();
		RemoveAppointment();
		return SUCCESS;
	}

	int scheduleSubDeleteCmd::undo()
	{
		__super::undo();
		ScheduleCtrl *pSchedul = (ScheduleCtrl *)this->m_pTargetWnd ;
		ASSERT(pSchedul);
		POSITION pos = _Appointments.GetHeadPosition();
		while(pos)
		{
			Appointment *pApp = _Appointments.GetAt(pos);
			pSchedul->AddAppointment(pApp,1,0);
			_Appointments.GetNext(pos);
		}
		return SUCCESS;
	}

};