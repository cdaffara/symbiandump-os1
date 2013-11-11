// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LbsInternalInterface.h"
#include "lbspoweradvicehandler.h"

CPowerModeAdviceHandler * CPowerModeAdviceHandler::NewL
	(
	MPowerModeObserver* aObserver
	)
	{

	CPowerModeAdviceHandler * self = new(ELeave) CPowerModeAdviceHandler (aObserver);

	CleanupStack::PushL(self);

	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}
	

	
CPowerModeAdviceHandler::CPowerModeAdviceHandler(MPowerModeObserver* aObserver) : 
CActive(CActive::EPriorityStandard),	
iObserver(aObserver)
	{

	}

void CPowerModeAdviceHandler::DoCancel()
	{
	iManagerPowerModeAdvice.CancelNotifyPowerModeAdviceUpdate();
	}

void CPowerModeAdviceHandler::ConstructL()
	{

   	iManagerPowerModeAdvice.OpenL(KLbsGpsLocManagerUid);

  	CActiveScheduler::Add(this);
	}

//------------------------------------------------------------------------------------------------------------
CPowerModeAdviceHandler::~CPowerModeAdviceHandler()
	{
	Cancel();
	iManagerPowerModeAdvice.Close();
	}
   
void CPowerModeAdviceHandler::RunL()
	{
	
	TInt status = iStatus.Int();
	User::LeaveIfError(status);
	
	StartGettingPowerUpdates();
	
	CLbsLocationSourceGpsBase::TPowerMode powerModeAdvice;

	User::LeaveIfError(iManagerPowerModeAdvice.GetPowerModeAdvice(powerModeAdvice));
	
	if (iObserver)
		{
		iObserver->PowerModeAdvice(powerModeAdvice); // call back to MainLogic
		}
	
	}

void CPowerModeAdviceHandler::StartGettingPowerUpdates()
	{
	Cancel();
	iManagerPowerModeAdvice.NotifyPowerModeAdviceUpdate(iStatus);

	SetActive();
	
	}
