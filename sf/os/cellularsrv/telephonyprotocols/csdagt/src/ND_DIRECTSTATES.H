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
* Declares direct connection state classes.
* 
*
*/



/**
 @file Nd_directstates.h
 @internalComponent
*/

#ifndef __ND_DIRECTSTATES_H__
#define __ND_DIRECTSTATES_H__

#include <comms-infras/cagentsmbase.h>


class CDirCtInit : public CAgentStateBase
/**
Direct Connection Init State.

@internalComponent
*/
	{
public:
	static CDirCtInit* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtInit();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDirCtInit(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	void ConstructL();
private:
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};


class CDirCtScanScript : public CAgentStateBase, public MNetDialScriptObserver
/**
Direct Connection Scan Script State.

@internalComponent
*/
	{
public:
	static CDirCtScanScript* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtScanScript();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	void ScriptFunctionComplete(TInt aError);
private:
	CDirCtScanScript(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	void ConstructL();
private:
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDirCtGetLoginInfo : public CGetLoginInfo
/**
Direct Connection Get Login Info State.

@internalComponent
*/
	{
public:
	static CDirCtGetLoginInfo* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	~CDirCtGetLoginInfo();
private:
	CDirCtGetLoginInfo(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	};

class CDirCtConnect : public CAgentStateBase, public MNetDialScriptObserver
/**
Direct Connection Connect State.

@internalComponent
*/
	{
public:
	static CDirCtConnect* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtConnect();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	void ScriptFunctionComplete(TInt aError);
private:
	CDirCtConnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	void ConstructL();
	void DoStartStateL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDirCtOpen : public CAgentStateBase
/**
Direct Connection Open State.

@internalComponent
*/
	{
public:
	static CDirCtOpen* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtOpen();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDirCtOpen(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDirCtHangUp : public CAgentStateBase
/**
Direct Connection HangUp State.

@internalComponent
*/
	{
public:
	static CDirCtHangUp* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtHangUp();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDirCtHangUp(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDirCtDisconnect : public CAgentStateBase
/**
Direct Connection Disconnect State.

@internalComponent
*/
	{
public:
	static CDirCtDisconnect* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtDisconnect();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDirCtDisconnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDirCtOpenDataPort : public CAgentStateBase
/**
Direct Connection Open Data Port State.

@internalComponent
*/
	{
public:
	static CDirCtOpenDataPort* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtOpenDataPort();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDirCtOpenDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDirCtConfigureCommPort : public CAgentStateBase
/**
Direct Connection Configure Comm Port State.

@internalComponent
*/
	{
public:
	static CDirCtConfigureCommPort* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDirCtConfigureCommPort();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);

private:
	CDirCtConfigureCommPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	void ConstructL();
	void DoStartStateL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	TCommConfig iConfigPckg;
	};

class CDirCtReConfigureCommPort : public CAgentStateBase
/**
Direct Connection ReConfigure Comm Port State.

@internalComponent
*/
	{
public:
	static CDirCtReConfigureCommPort* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TBool aReConnect);
	virtual ~CDirCtReConfigureCommPort();
	// Derived from CAgentStateBase
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);

private:
	CDirCtReConfigureCommPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TBool aReConnect);
	void ConstructL();
	void DoStartStateL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	TBool iDropping;
	TBool iReConnect;
	};

class CDirCtCloseDataPort : public CAgentStateBase
/**
Direct Connection Close Data Port State.

@internalComponent
*/
	{
public:
	static CDirCtCloseDataPort* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError);
	virtual ~CDirCtCloseDataPort();
	//CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	//MNetDialScriptObserver Inheritance
private:
	CDirCtCloseDataPort(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv, TInt aError);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	TInt iError;
	};

#endif
