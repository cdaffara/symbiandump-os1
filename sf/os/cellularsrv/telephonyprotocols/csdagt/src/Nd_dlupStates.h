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
* Declares dial up state classes.
* 
*
*/



/**
 @file Nd_dlupstates.h
 @internalComponent
*/

#ifndef __ND_DLUPSTATES_H__
#define __ND_DLUPSTATES_H__


#include "ND_DLUP.H"
#include "Nd_dlinStates.h"
#include "ND_SCR.H"

class CDlUpInit : public CAgentStateBase
/**
Dial Up Init State.

@internalComponent
*/
	{
public:
	static CDlUpInit* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpInit();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDlUpInit(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDlUpDialling : public CAgentStateBase, public MNetDialEtelObserver
/**
Dial Up Dialling State.

@internalComponent
*/
	{
public:
	static CDlUpDialling* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpDialling(); 
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	//MNetDialEtelObserver Inheritance
	virtual void TelFunctionComplete(TInt aError);
private:
	CDlUpDialling(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
private:
	MNetdialEnv* iNdEnv;
	};

class CDlUpOpenDataPort : public CAgentStateBase
/**
Dial Up Open Data Port State.

@internalComponent
*/
	{
public:
	static CDlUpOpenDataPort* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpOpenDataPort();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDlUpOpenDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDlUpScanScript : public CAgentStateBase, public MNetDialScriptObserver
/**
Dial Up Scan Script State.

@internalComponent
*/
	{
public:
	static CDlUpScanScript* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpScanScript();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	//MNetDialScriptObserver Inheritance
	virtual void ScriptFunctionComplete(TInt aError);
private:
	CDlUpScanScript(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDlUpGetLoginInfo : public CGetLoginInfo
/**
Dial Up Get Login Info State.

@internalComponent
*/
	{
public:
	static CDlUpGetLoginInfo* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	~CDlUpGetLoginInfo();
private:
	CDlUpGetLoginInfo(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	};

class CDlUpLogin : public CAgentStateBase, public MNetDialScriptObserver
/**
Dial Up Login State.

@internalComponent
*/
	{
public:
	static CDlUpLogin* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpLogin();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	void DoStartStateL();
	//MNetDialScriptObserver Inheritance
	virtual void ScriptFunctionComplete(TInt aError);
private:
	CDlUpLogin(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDlUpCloseDataPort : public CAgentStateBase
/**
Dial Up Close Data Port State.

@internalComponent
*/
	{
public:
	static CDlUpCloseDataPort* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError);
	virtual ~CDlUpCloseDataPort();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	//MNetDialScriptObserver Inheritance
private:
	CDlUpCloseDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	TInt iError;
	};

#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
class CDlUpOpen : public CNdCallBackStateBase, public MNetDialEtelObserver
#else
class CDlUpOpen : public CNdCallBackStateBase
#endif
/**
Dial Up Open State.

@internalComponent
*/
	{
public:
	static CDlUpOpen* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpOpen();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT	
	//MNetDialEtelObserver Inheritance
	virtual void TelFunctionComplete(TInt aError);
#endif	
private:
	CDlUpOpen(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
#ifdef SYMBIAN_NETWORKING_CSDAGENT_BCA_SUPPORT
	TBool iTelEvent:1;
#endif	
	};

class CDlUpHangUp : public CNdCallBackStateBase, public MNetDialEtelObserver
/**
Dial Up HangUp State

@internalComponent
*/
	{
public:
	static CDlUpHangUp* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpHangUp();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	//MNetDialEtelObserver Inheritance
	virtual void TelFunctionComplete(TInt aError);
private:
	CDlUpHangUp(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	TBool iLogCallEnd ;
	};

class CDlUpDisconnect : public CNdCallBackStateBase
/**
Dial Up Disconnect State.

@internalComponent
*/
	{
public:
	static CDlUpDisconnect* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlUpDisconnect();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDlUpDisconnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

#endif
