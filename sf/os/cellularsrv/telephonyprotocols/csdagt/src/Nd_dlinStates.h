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
* Declares dial in state classes.
* 
*
*/



/**
 @file Nd_dlinstates.h
 @internalComponent
*/

#if !defined(__ND_DLINSTATES_H__)
#define __ND_DLINSTATES_H__

#include "ND_ETEL.H"
#include "Nd_CallBack.h"
#include "Nd_dlupStates.h"


class CDlInInit : public CAgentStateBase
/**
Dial In Init State.

@internalComponent
*/
	{
public:
	static CDlInInit* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlInInit();
	// CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDlInInit(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
private: 
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDlInAnswering : public CNdCallBackStateBase, public MNetDialEtelObserver
/**
Dial In Answering State.

@internalComponent
*/
	{
public:
	static CDlInAnswering* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlInAnswering(); 
	// CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	// MNetDialEtelObserver Inheritance
	virtual void TelFunctionComplete(TInt aError);
private:
	CDlInAnswering(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	TBool iConnectionAccepted;
	};


class CDlInOpen : public CAgentStateBase
/**
Dial In Open State.

@internalComponent
*/
	{
public:
	static CDlInOpen* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlInOpen();
	// CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDlInOpen(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDlInDisconnect : public CAgentStateBase
/**
Dial In Disconnect State.

@internalComponent
*/
	{
public:
	static CDlInDisconnect* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlInDisconnect();
	// CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
private:
	CDlInDisconnect(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	};

class CDlInHangUp : public CAgentStateBase, public MNetDialEtelObserver
/**
Dial In HangUp State.

@internalComponent
*/
	{
public:
	static CDlInHangUp* NewL(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual ~CDlInHangUp();
	// CAgentStateBase Inheritance
	virtual void StartState();
	virtual CAgentStateBase* NextStateL(TBool aContinue);
	// MNetDialEtelObserver Inheritance
	virtual void TelFunctionComplete(TInt aError);
private:
	CDlInHangUp(MAgentStateMachineEnv& aSMObserver, MNetdialEnv& aNdEnv);
	virtual void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
private:
	MNetdialEnv* iNdEnv;
	TBool iLogCallEnd;
	};

#endif
