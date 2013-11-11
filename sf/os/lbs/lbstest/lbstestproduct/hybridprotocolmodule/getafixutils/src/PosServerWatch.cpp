/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
//
// PosServerWatch.cpp
//
//

#include "PosServerWatch.h"

#include <lbs.h>
	
EXPORT_C CPosServerWatcher* CPosServerWatcher::NewLC(RPositioner& aPos, MPosServerObserver& aObserver)
	{
	CPosServerWatcher* p = new (ELeave) CPosServerWatcher(aPos, aObserver);
	CleanupStack::PushL(p);
	p->ConstructL();
	
	return p;
	}

CPosServerWatcher::CPosServerWatcher(RPositioner& aPos, MPosServerObserver& aObserver)
:CActive(EPriorityNormal),
 iObserver(aObserver),
 iPos(aPos)
 {
 ;
 }
 
void CPosServerWatcher::ConstructL()
	{
	CActiveScheduler::Add(this);	
	}
	
	
EXPORT_C CPosServerWatcher::~CPosServerWatcher()
//
// NOT responsable for closing the server etc down...
//
	{
	if(IsActive())
		Cancel();
	
	if(IsAdded())
		Deque();
	}

EXPORT_C void CPosServerWatcher::IssueGetLastKnownPosition()
	{
	__ASSERT_DEBUG(iPos.SubSessionHandle(), User::Invariant());
	__ASSERT_DEBUG(IsAdded(), User::Invariant());
	__ASSERT_DEBUG(!IsActive(), User::Invariant());

	iPos.GetLastKnownPosition(iData, iStatus);
	iRequest = EPositionerGetLastKnownPosition;
	
	SetActive();
	}
	
EXPORT_C void CPosServerWatcher::IssueNotifyPositionUpdate()
	{
	__ASSERT_DEBUG(iPos.SubSessionHandle(), User::Invariant());
	__ASSERT_DEBUG(IsAdded(), User::Invariant());
	__ASSERT_DEBUG(!IsActive(), User::Invariant());

	iPos.NotifyPositionUpdate(iData, iStatus);
	iRequest = EPositionerNotifyPositionUpdate;
	
	SetActive();
	}

EXPORT_C void CPosServerWatcher::CancelRequest()
	{
	switch(iRequest)
		{
		case EPositionerNotifyPositionUpdate: // Fall through
		case EPositionerGetLastKnownPosition: iPos.CancelRequest(iRequest);
											  break;	
		default: // Do nothing!
			break;
		}
	}


EXPORT_C void CPosServerWatcher::CompleteRequest()
	{
	iPos.CompleteRequest(EPositionerNotifyPositionUpdate);
	}

void CPosServerWatcher::DoCancel()
	{
	// cancel calls DoCancel. Note that cancel consumes the Cancel error
	iPos.CancelRequest(iRequest);
	}

void CPosServerWatcher::RunL()
	{
	switch(iRequest)
		{
		case EPositionerNotifyPositionUpdate: iObserver.OnNotifyPositionUpdate(iStatus.Int(), iData);
											  break;	
		case EPositionerGetLastKnownPosition: iObserver.OnGetLastKnownPosition(iStatus.Int(), iData);
											  break;	
		default: User::Invariant();
		}
	}

TInt CPosServerWatcher::RunError(TInt /* aError */)
	{ // if RunError is called then a test failed so stop the active scheduler
	CActiveScheduler::Stop();
	return KErrNone;
	}
