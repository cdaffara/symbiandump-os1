// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CallBack Timer and Callback state base
// 
//

/**
 @file nd_callback.cpp 
*/

#include "Nd_CallBack.h"

CCallBackTimer* CCallBackTimer::NewL(MNdCallBackObserver* aNotifier)
/**
2 phased constructor, first phase.

@param aNotifier a pointer to the call back observer.
@exception Leaves if ContructL() leaves, or not enough memory is available.
@return a new CCallBackTimer object.
*/
	{
	CCallBackTimer* p=new(ELeave) CCallBackTimer(aNotifier);
	CleanupStack::PushL(p);
	p->ConstructL();	// CTimer::ConstructL()
	CleanupStack::Pop();
	return p;
	}

CCallBackTimer::CCallBackTimer(MNdCallBackObserver* aNotifier) 
	: CTimer(EPriorityStandard), iNotifier(aNotifier)
/**
Private constructor used in the first phase of construction.

@param aNotifier a pointer to the call back observer.
*/
	{
	CActiveScheduler::Add(this);
	}

void CCallBackTimer::Start(TTimeIntervalMicroSeconds32 aTimer)
/**
Start timer.

@param aTimer a value for the timer.
*/
	{
	__ASSERT_DEBUG(aTimer>TTimeIntervalMicroSeconds32(0), User::Invariant());	
	After(aTimer);
	}

void CCallBackTimer::RunL()
/**
Notify SM that timer complete.
*/
	{
	iNotifier->TimerComplete(KErrTimedOut);
	}

// NetDial CallBack State base definitions 
//
CNdCallBackStateBase::CNdCallBackStateBase(MAgentStateMachineEnv& aSMObserver)
	: CAgentStateBase(aSMObserver)
/**
Constructor.

@param aSMObserver a reference to the state machine observer.
*/
	{}

void CNdCallBackStateBase::ConstructL()
/**
Instantiate Member variable.
*/
	{
	iCallBackTimer=CCallBackTimer::NewL(this);
	}

CNdCallBackStateBase::~CNdCallBackStateBase()
/**
Destructor.
Delete member attributes.
*/
	{
	delete iCallBackTimer;
	}

void CNdCallBackStateBase::TimerComplete(TInt aError)
/**
Timer completed.

@param aError a error passed for completition.
*/
	{
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,aError);
	}
