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
* CallBack Timer and Callback state base.
* 
*
*/



/**
 @file Nd_callback.h
 @internalComponent
*/

#ifndef __ND_CALLBACK_H__
#define __ND_CALLBACK_H__

#include <comms-infras/cagentsmbase.h>


class MNdCallBackObserver
/**
Callback state m class.
*/
	{
public:
	virtual void TimerComplete(TInt aError) =0;
	};


class CCallBackTimer : public CTimer
/**
CCallBackTimer class.
*/
	{
public:
	static CCallBackTimer* NewL(MNdCallBackObserver* aNotifier);
	CCallBackTimer(MNdCallBackObserver* aNotifier);
	void Start(TTimeIntervalMicroSeconds32 aTimer);
private:
	void RunL();
private:
	MNdCallBackObserver* iNotifier;
	};


class CNdCallBackStateBase : public CAgentStateBase, public MNdCallBackObserver
/**
CallBack state base.
*/
	{
public:
	CNdCallBackStateBase(MAgentStateMachineEnv& aSMObserver);
	void ConstructL();
	virtual ~CNdCallBackStateBase();
	//NdCallBack Observer Inheritance
	virtual void TimerComplete(TInt aError);
protected:
	CCallBackTimer* iCallBackTimer;
	};

#endif
