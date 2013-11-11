// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "panagtutils.h"
using namespace PanAgent;

void PanAgent::PanAgentPanic(TPanAgentPanic aPanicCode)
/**
PAN agent panic
@param aPanicCode A code identifying the specific panic
*/
	{
	User::Panic(KPanAgentName, aPanicCode);
	}
	
void PanAgent::PanAgentPanic(TPanAgentPanic aPanicCode, TInt aState)
/**
PAN agent panic
@param aPanicCode A code identifying the specific panic
@param aState The state in which the panic occurred.
*/
	{
	User::Panic(KPanAgentName, (aPanicCode*KStatePanicMultiplier) + KStatePanicDelimiter + aState);
	}

	
//
// Timer helper class
//

CPanAgtTimerHelper* CPanAgtTimerHelper::NewL(MPanAgtTimerCallback& aCallback)
/**

*/
	{
	CPanAgtTimerHelper* self = new(ELeave) CPanAgtTimerHelper(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CPanAgtTimerHelper::CPanAgtTimerHelper(MPanAgtTimerCallback& aCallback) :
	CActive(EPriorityStandard), iCallback(aCallback)
/**

*/
	{	
	CActiveScheduler::Add(this);
	}
	
void CPanAgtTimerHelper::ConstructL()
/**

*/
	{
	// create and set timer
	User::LeaveIfError(iTimer.CreateLocal());
	}

void CPanAgtTimerHelper::SetTimer(TTimeIntervalMicroSeconds32 aInterval)
/**
Set the timer for the specified interval
*/
	{
	// set a timer for the maximum amount of time we will wait for the role switch to complete
	iTimer.After(iStatus, aInterval);
	SetActive();	
	}

void CPanAgtTimerHelper::RunL()
/**
Timer has completed
*/
	{
	if(iStatus==KErrNone)
		{
		iCallback.TimerComplete();
		}
	else
		{
		iCallback.TimerError(iStatus.Int());
		}
	}
	
void CPanAgtTimerHelper::DoCancel()
/**
Cancel the timer
*/
	{
	iTimer.Cancel();
	}
