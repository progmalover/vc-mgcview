#pragma once
#include "resource.h"
#include <vector>
#include <stack>
using namespace std;
//
//zxy designed for via 2012.2
//
namespace  _CMDS
{
#define FAILURE		-1
#define SUCCESS		 1
#define IS_FAILURE(T)\
		(T < 0)
#define IS_SUCESS(T)\
		(T >= 1)
//Current operator,to point out "cmd" in which stack.
#define OPT_DO		0
#define OPT_UNDO	1
#define OPT_REDO	2

#define CMD_MIN 0
#define CMD_MAX 500
#define DECLARE_FRIEND_CLASS(T)\
		friend class T;

	//
	//cmd prototype
	//define a base cmd struct.
	//
	class _baseCmd;
	typedef  vector<_baseCmd *>  SUB_CMDS;
	class _baseCmd
	{
		//friend  class _baseCmdFactory<class Owner>;
	protected:
		_baseCmd( int id = 0 ,CWnd *pTarget = NULL){
			m_nCmd = id;
			m_pTargetWnd = pTarget;
			m_nOpt = -1;
		};
		~_baseCmd(){};
	public:
	public:
		int addSubCmd(_baseCmd *pCmd){
			m_subCmds.push_back(pCmd);
			pCmd->m_pParentCmd = this;
			return SUCCESS;
		};
		SUB_CMDS &	getSubCmds(){return m_subCmds;};
		bool		isObjOf(int nCmd){ return (m_nCmd == nCmd); };
	public:
		virtual int redo(){m_nOpt = OPT_UNDO;return SUCCESS;};
		virtual int undo(){m_nOpt = OPT_REDO; return SUCCESS;};
		virtual int ondo(){m_nOpt = OPT_UNDO;return SUCCESS;};
		int getID(){return m_nCmd;};
	protected:
		int m_nOpt;
		int m_nCmd;
		CWnd *m_pTargetWnd;
		_baseCmd *m_pParentCmd;
		SUB_CMDS m_subCmds;
	};

	//
	//cmd factor prototype
	//
	template<class CMDENGINE>
	class _baseCmdFactory
	{
	public:
		_baseCmdFactory(){};
		~_baseCmdFactory(){};
	public:
		void bind(CMDENGINE * pOwner){
			m_pOwner = pOwner;//this is an pointer to cmdengine.
		};
	protected:
		virtual bool delCmd(_baseCmd *pCmd)
		{
			if(NULL == pCmd)
				return false;
			SUB_CMDS &cmds = pCmd->getSubCmds();
			if(cmds.size() > 0)
			{
				for( size_t i = 0 ;i < cmds.size();i++)
				{
					_baseCmd *pCmd = cmds[i];
					delCmd(pCmd);
				}
				cmds.clear();
			}
			return true;
		}
		void FreeBaseCmd(_baseCmd *pCmd)
		{
            class _baseCmdEx:public _baseCmd{};
			if(pCmd->getID() < CMD_MIN)
			{
				TRACE(_T("Unsafe memory is detected!\n"));
				return;
			}
			_baseCmdEx *pCmdEx = (_baseCmdEx *)pCmd;
			delete pCmdEx;
		};

	public:
		virtual _baseCmd * newCmdBegin(int nID){return (_baseCmd *)NULL;};
		virtual _baseCmd * getBuffedCmd(){return (_baseCmd *)NULL;};
		virtual _baseCmd * newCmdEnd(){return (_baseCmd *)NULL;};
		virtual _baseCmd * newCmd(int nID){return (_baseCmd *)NULL;}
		virtual LPCTSTR getCmdName(_baseCmd *){return _T("");};	
		virtual CWnd  *	getTargetWnd(){return (CWnd *)NULL;};
		virtual void	release(){ delete this;};
	protected:
		CMDENGINE *m_pOwner; 
	};

	//
	// Cmds Engine ,to excute cmd,and reserve undo\redo stack. 
	//
	typedef stack<_baseCmd *>  CMD_STACK;
	template<class FACTORY>
	class CmdsEngine
	{
	public:
		CmdsEngine(){
			m_pFactory = NULL;
			m_pTargetWnd = NULL;
			m_hAccel = NULL;
		};
		~CmdsEngine(){

			clearStack(&m_redoCmds);
			clearStack(&m_undoCmds);
			if(m_pFactory)
				m_pFactory->release();
			if(m_hAccel)
				::DestroyAcceleratorTable(m_hAccel);
			
		};
	public:
		bool TransMessage( MSG *pMsg)
		{
			if(!m_pTargetWnd || !::IsWindow(m_pTargetWnd->m_hWnd))
				return false;

			if(pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
			{
				::TranslateAccelerator(m_pTargetWnd->m_hWnd,this->m_hAccel,pMsg);
			}
			return true;
		}

		virtual bool bindTarget( CWnd *pTargetWnd){
			m_pTargetWnd = pTargetWnd; //when engine obj bind to wnd instance,an factor obj was create and bind it.i
			if(NULL == this->m_pFactory)
				m_pFactory = new FACTORY;

			m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_UNDOREDO));
			return true;
		};
		FACTORY * getFactory(){return m_pFactory;};
		CWnd *getTargetWnd(){return m_pTargetWnd;};
		void clearStack(CMD_STACK *pStack)
		{
			ASSERT(pStack != NULL);
			while(pStack->size())
			{
				_baseCmd *pCmd = pStack->top();
				m_pFactory->delCmd(pCmd);
				pStack->pop();
			}
		};
	public:
		virtual int	undo(){
			if(m_undoCmds.size()<= 0)
				return 0;
			_baseCmd *cmd = m_undoCmds.top();
			if(IS_FAILURE(cmd->undo()))
			{
				TRACE(m_pFactory->getCmdName(cmd));
				TRACE(_T(" Undo Failured !\n"));
				ASSERT(FALSE);
				return FAILURE;
			}
	
			m_undoCmds.pop();
			m_redoCmds.push(cmd);
			return SUCCESS;
		};

		virtual int	 redo(){
			if(m_redoCmds.size() <= 0)
				return 0;
			_baseCmd *cmd = m_redoCmds.top();
			if(IS_FAILURE(cmd->redo()))
			{
				TRACE(m_pFactory->getCmdName(cmd));
				TRACE(_T(" Redo Failured !\n"));
				ASSERT(FALSE);
				return FAILURE;
			}
			m_redoCmds.pop();
			m_undoCmds.push(cmd);
			return SUCCESS;
		};

		virtual bool canUndo(){
			return  m_undoCmds.size() > 0;
		};

		virtual bool canRedo(){
			return m_redoCmds.size() > 0;
		};
	
		CMD_STACK & GetUndoCmds(){return m_undoCmds;};
		CMD_STACK &	GetRedoCmds(){return m_redoCmds;};
		virtual bool  execCmd(_baseCmd *cmd){
			if(IS_FAILURE(cmd->ondo()))
			{
				TRACE(m_pFactory->getCmdName(cmd));
				TRACE(_T(" Do Failured !\n"));
				return false;
			}
			m_undoCmds.push(cmd);
			clearStack(&m_redoCmds);
			return true;
		};
	private:
		HACCEL  m_hAccel;
	protected:
		CMD_STACK m_undoCmds;
		CMD_STACK m_redoCmds;
		FACTORY 	* m_pFactory;
		CWnd    * m_pTargetWnd;
	};

};