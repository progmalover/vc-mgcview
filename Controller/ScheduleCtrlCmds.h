#pragma once

#include "S3Types.h"
#include "S3DrawRectObj.h"
#include "LayoutSettings.h"
#include "appointment.h"
#include "cmds.h"
//
//zxy designed for via 2012.2
//
using namespace _CMDS;
namespace _SCHEDULECTRLCMDS
{
#define DECLARE_FRIEND_FUCTION(T)\
	friend T;

#define DECLARE_SCHEULE_FRIEND_CMDS()\
	DECLARE_FRIEND_CLASS(scheduleCreateCmd)\
	DECLARE_FRIEND_CLASS(scheduleDeleteCmd)\
	DECLARE_FRIEND_CLASS(scheduleDragCmd)\
	DECLARE_FRIEND_CLASS(schedulePasteCmd)\
	DECLARE_FRIEND_CLASS(scheduleVPasteCmd)\
	DECLARE_FRIEND_CLASS(scheduleHPasteCmd)\
	DECLARE_FRIEND_CLASS(scheduleAlignCmd)\
	DECLARE_FRIEND_CLASS(scheduleDurationChangeCmd)\
	DECLARE_FRIEND_CLASS(scheduleSubDeleteCmd)\
    DECLARE_FRIEND_CLASS(_scheduleBaseCmd)

	enum cmds{
		CMD_SCHEDULE_CREATE = 2,
		CMD_SCHEDULE_DELETE,
		CMD_SCHEDULE_PASTE,
		CMD_SCHEDULE_VPASTE,
		CMD_SCHEDULE_HPASTE,
		CMD_SCHEDULE_ALIGN,
		CMD_SCHEDULE_DRAG,		//keep duration
		CMD_SCHEDULE_DURATION_CHANGE, //duration changed.
		CMD_SCHEDULE_SUB_DELETE
		
	};

	class ScheduleCmdEngine;
	class ScheduleCmdFactory :public _baseCmdFactory<ScheduleCmdEngine>
	{
	public:
		ScheduleCmdFactory();
		~ScheduleCmdFactory();
	public:
		_baseCmd * newCmdBegin(int nID);
		_baseCmd * getBuffedCmd();
		_baseCmd * newCmdEnd();
		_baseCmd * newCmd(int nID);
		bool       delCmd(_baseCmd *pCmd);
		LPCTSTR    getCmdName(_baseCmd *);
		CWnd	*  getTargetWnd();
	private:
		_baseCmd * m_pBuffCmd;
	};

	class ScheduleCmdEngine :public CmdsEngine<ScheduleCmdFactory>
	{
		friend class ScheduleCmdFactory;
	public:
		ScheduleCmdEngine(){};
		~ScheduleCmdEngine(){};
	public:
		virtual bool bindTarget(CWnd *pTargetWnd);
	};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///cmds prototype.

	//
	//global data struction.
	//
	typedef struct AppointInfo
	{
		AppointInfo()
		{
			pAppointment = NULL;
		}
		Appointment *pAppointment;
		COleDateTime dtStart;
		COleDateTime dtEnd;
	} APPOINTMENT_INFO;
	typedef vector<APPOINTMENT_INFO> APPOINTMENT_VECTOR;

	/*
	base information for schedulerctrl,about time range etc.
	*/
	class _scheduleBaseCmd:public _baseCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		_scheduleBaseCmd(int,CWnd *);
		~_scheduleBaseCmd();
	protected:
		COleDateTime _DateBegin;
		COleDateTime _DateEnd;
	public:
		int undo();
		int redo();
		int ondo();
	};
	/*
	create new schedule.
	*/
	class scheduleCreateCmd :public _scheduleBaseCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		scheduleCreateCmd(int,CWnd *);
		~scheduleCreateCmd();
	public:
		 AppointmentList _Appointments;
	public:
		int undo();
		int redo();
		int ondo();
	};

	/*
	delete selection
	*/
	class scheduleDeleteCmd :public _scheduleBaseCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		scheduleDeleteCmd(int,CWnd *);
		~scheduleDeleteCmd();
	public:
		AppointmentList _Appointments;
	public:
		int undo();
		int redo();
		int ondo();
	};

	/*
	drag on 
	*/
	class scheduleDragCmd :public _scheduleBaseCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		scheduleDragCmd(int,CWnd *);
		~scheduleDragCmd();
	public:
		void SaveBeforeDrag();
	protected:
		APPOINTMENT_VECTOR _StoreInfo;
	public:
		int undo();
		int redo();
		int ondo();
	};

	/*
	duration change£¨include in drag on ?£©
	*/
	class scheduleDurationChangeCmd :public _scheduleBaseCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		scheduleDurationChangeCmd(int,CWnd *);
		~scheduleDurationChangeCmd();
	public:
		void SaveBeforeDrag();
	protected:
		APPOINTMENT_VECTOR _StoreInfo;
	public:
		int undo();
		int redo();
		int ondo();
	};

	/*
	paste
	*/
	class schedulePasteCmd:public _scheduleBaseCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		schedulePasteCmd(int,CWnd *);
		~schedulePasteCmd();
	public:
		AppointmentList _PasteList;
	public:
		int undo();
		int redo();
		int ondo();

	};

	/*
	v paste operation.
	*/
	class scheduleVPasteCmd:public schedulePasteCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		scheduleVPasteCmd(int,CWnd *);
		~scheduleVPasteCmd();
	public:
		int undo();
		int redo();
		int ondo();

	};

	/*
	h paste operation
	*/
	class scheduleHPasteCmd:public schedulePasteCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		scheduleHPasteCmd(int,CWnd *);
		~scheduleHPasteCmd();
	public:
		int undo();
		int redo();
		int ondo();

	};

	/*
	align operation.
	*/
	const int ALIGN_UP = 0;
	const int ALIGN_DOWN = 1;
	class scheduleAlignCmd :public _scheduleBaseCmd
	{
		friend class ScheduleCmdFactory;
	protected:
		scheduleAlignCmd(int,CWnd *);
		~scheduleAlignCmd();
	protected:
		APPOINTMENT_VECTOR _StoreInfo;
	public:
		int _AlignDirection;
	public:
		int undo();
		int redo();
		int ondo();
	};
	/////////////////////////////////////////////////////////////////////////////////////////
	//sub cmd
	class scheduleSubDeleteCmd :public _baseCmd
	{
		friend class ScheduleCmdFactory;
	public:
		scheduleSubDeleteCmd(int nID,CWnd *pWnd);
		~scheduleSubDeleteCmd();
	public:
		AppointmentList _Appointments;
	protected:
		int RemoveAppointment();
	public:
		int undo();
		int redo();
		int ondo();
	};
		
};