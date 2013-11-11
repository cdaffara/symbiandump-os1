/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Declares the MNetdialEnv, CNetdialSM and CGetLoginInfo classes.
* 
*
*/



/**
 @file Nd_bases.h
 @internalComponent
*/


#ifndef __ND_BASES_H__
#define __ND_BASES_H__

#include <comms-infras/cagentsmbase.h>
#include "ND_STD.H"
#include "ND_SCR.H"
#include <csdprog.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/nifprvar_internal.h>
#endif

// Forward declarations
class CCommsDbNetDialAccess;
class CNetDialScript;
class CEventLogger;
class CTelServerProcessor;

class MNetdialEnv
/**
Netdial environment class.
*/
	{
public:
	virtual MAgentStateMachineEnv* BaseEnv() =0;
	virtual CCommsDbNetDialAccess* NetDialDb() =0;
	virtual CNetDialScript* Script() =0;
	virtual CTelServerProcessor* TelServPrc() =0;
	virtual CEventLogger* Logger();
	virtual TBool UseScript() const =0;
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	virtual void GetUseScriptL() = 0;
#endif	
	virtual void SetUpScriptL() =0;
	virtual void DeleteScript() =0;
	};


class CNetdialSM : public CAgentSMBase,public MNetdialEnv
/**
Netdial State Machine base class.
*/
	{
public:
	virtual ~CNetdialSM();
	//Inherited from MNetdialEnv
	virtual MAgentStateMachineEnv* BaseEnv();
	virtual CCommsDbNetDialAccess* NetDialDb();
	virtual CNetDialScript* Script();
	virtual CTelServerProcessor* TelServPrc();
	virtual CEventLogger* Logger();
	virtual TBool UseScript() const;
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	virtual void GetUseScriptL();
#endif	
	virtual void SetUpScriptL() =0;
	virtual void DeleteScript();
	//Inherited from SMBase class
	virtual TInt GetExcessData(TDes8& aBuffer);
protected: 
	CNetdialSM(MAgentNotify& aObserver, CDialogProcessor* aDlgPrc, CCommsDbAccess& aDbAccess);
	virtual void ConstructL();
protected:
	CNetDialScript* iNdScript;
	CTelServerProcessor* iTelServer;
	CCommsDbNetDialAccess* iDbAccess;
	CEventLogger* iLogger;
	TBool iUseScript;
	};

class CGetLoginInfo : public CAgentStateBase, public MNetDialScriptObserver
/**
CGetLoginInfo base class. Class derived by direct connect and dial up sub-classes.
*/
	{
public:
	virtual ~CGetLoginInfo();
	//CAgentStateBase Inheritance
	virtual void StartState();
	//MNetDialScriptObserver Inheritance
	virtual void ScriptFunctionComplete(TInt aError);
protected:
	CGetLoginInfo(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
private:
	virtual void DoCancel();
	virtual void RunL();
protected:
	MNetdialEnv* iNdEnv;
	};

#endif
