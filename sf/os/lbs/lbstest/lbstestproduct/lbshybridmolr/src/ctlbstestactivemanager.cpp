// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctlbstestactivemanager.h"

CT_LbsTestActiveManager* CT_LbsTestActiveManager::NewL(TInt aObjectId, MTestFlowObserver& aObserver, TPriority aPriority, CTestStep& aStep)
	{
	CT_LbsTestActiveManager* self = new(ELeave) CT_LbsTestActiveManager(aObjectId, aObserver, aPriority, aStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CT_LbsTestActiveManager::~CT_LbsTestActiveManager()
	{
	Cancel();
	}

CT_LbsTestActiveManager::CT_LbsTestActiveManager(TInt aObjectId, MTestFlowObserver& aObserver, TPriority aPriority, CTestStep& aStep)
	: CTimer(aPriority), iState(EStateNone), iObjectId(aObjectId), 
		iWaiting(EFalse), iIsDeactivated(EFalse), iObserver(aObserver), iStep(aStep)
	{
	CActiveScheduler::Add(this);
	}

void CT_LbsTestActiveManager::StartL(TTimeIntervalMicroSeconds32 aDelay)
	{
	if(iWaiting || iIsDeactivated)
		{
		return;
		}

	iState = EStateWaitingToStart;
	After(aDelay);

	iWaiting = ETrue;
	}

void CT_LbsTestActiveManager::WaitForPosition(TTimeIntervalMicroSeconds32 aDelay)
	{
	if(iWaiting || iIsDeactivated)
		{
		return;
		}

	iState = EStateWaitingForPosition;
	After(aDelay);
	iWaiting = ETrue;
	}

void CT_LbsTestActiveManager::Deactivate()
	{
	iIsDeactivated = ETrue;
	}

TBool CT_LbsTestActiveManager::IsDeactivated()
	{
	return iIsDeactivated;
	}

// MPosServerObserver
void CT_LbsTestActiveManager::OnGetLastKnownPosition(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	iWaiting = EFalse;
	Cancel();
	TRAPD(err, iObserver.OnGetLastKnownPositionL(iObjectId, aErr, aPosInfo));
	if(err != KErrNone)
		{
		RunError(err);
		}
	}


void CT_LbsTestActiveManager::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	iWaiting = EFalse;
	Cancel();
	TRAPD(err, iObserver.OnNotifyPositionUpdateL(iObjectId, aErr, aPosInfo));
	if(err != KErrNone)
		{
		RunError(err);
		}
	}

void CT_LbsTestActiveManager::RunL()
	{
	iWaiting = EFalse;
	switch(iState)
		{
		case EStateWaitingToStart:
			{
			iState = EStateNone;
			iObserver.OnReadyL(iObjectId);
			break;
			}
		case EStateWaitingForPosition:
			{
			iState = EStateNone;
			iObserver.OnTimedOutL(iObjectId);
			break;
			}
		default:
			{
			User::Leave(KErrCorrupt);
			}
		}
	}

TInt CT_LbsTestActiveManager::RunError(TInt aError)
	{
	iStep.SetTestStepError(aError);
	iStep.SetTestStepResult(EFail);
	iObserver.StopTest();
	return KErrNone;
	}
