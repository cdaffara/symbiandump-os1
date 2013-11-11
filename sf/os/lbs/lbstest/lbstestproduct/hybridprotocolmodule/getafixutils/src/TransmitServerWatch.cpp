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
// TransmitServerWatch.cpp
//
//

#include "TransmitServerWatch.h"

#include <lbs.h>

EXPORT_C CTransmitServerWatcher* CTransmitServerWatcher::NewLC(RLbsTransmitPosition& aTransmitter, MTransmitServerObserver* aObserver)
	{
	CTransmitServerWatcher* p = new (ELeave) CTransmitServerWatcher(aTransmitter, aObserver);
	CleanupStack::PushL(p);
	p->ConstructL();
	
	return p;
	}

CTransmitServerWatcher::CTransmitServerWatcher(RLbsTransmitPosition& aTransmitter, MTransmitServerObserver* aObserver):iObserver(aObserver), iTransmitter(aTransmitter)
	 {
	 // nothing to do
	 }
 
void CTransmitServerWatcher::ConstructL()
	{
	iRefPosWaiter = CTLbsX3pRefLocWaiter::NewL(iObserver);
	iTransmitPosWaiter = CTLbsX3pTransmitLocWaiter::NewL(iObserver);
	}
	
	
EXPORT_C CTransmitServerWatcher::~CTransmitServerWatcher()
	{
	CancelTransmit();
	delete iRefPosWaiter;
	delete iTransmitPosWaiter;
	}

	
EXPORT_C void CTransmitServerWatcher::IssueTransmitPosition(const TDesC& aDestinationID, const TUint aTransmitPriority)
	{
	__ASSERT_DEBUG(!iRefPosWaiter->IsActive(), User::Invariant());
	__ASSERT_DEBUG(!iTransmitPosWaiter->IsActive(), User::Invariant());
	__ASSERT_DEBUG(iTransmitPosWaiter->IsAdded(), User::Invariant());
	__ASSERT_DEBUG(iRefPosWaiter->IsAdded(), User::Invariant());
	
	iTransmitter.TransmitPosition(aDestinationID, aTransmitPriority, iRefPosWaiter->iStatus, iRefPosWaiter->iRefPosInfo, iTransmitPosWaiter->iStatus, iTransmitPosWaiter->iTransmitPosInfo);
	iRefPosWaiter->Activate();
	iTransmitPosWaiter->Activate();
	}

EXPORT_C void CTransmitServerWatcher::CancelTransmit()
	{
	iTransmitter.CancelTransmitPosition();
	iRefPosWaiter->Cancel();
	iTransmitPosWaiter->Cancel();
	}


CTLbsX3pRefLocWaiter::CTLbsX3pRefLocWaiter(MTransmitServerObserver* aObserver):CActive(EPriorityStandard), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	
	}


CTLbsX3pRefLocWaiter::~CTLbsX3pRefLocWaiter()
	{
	if(IsActive())
		Cancel();
	
	if(IsAdded())
		Deque();		
	}

CTLbsX3pRefLocWaiter* CTLbsX3pRefLocWaiter::NewL(MTransmitServerObserver* aObserver)
	{
	CTLbsX3pRefLocWaiter* self = new(ELeave)CTLbsX3pRefLocWaiter(aObserver);
	return self;		
	}
	

void CTLbsX3pRefLocWaiter::Activate()
	{
	SetActive();
	}

void CTLbsX3pRefLocWaiter::RunL()
	{
	if (iObserver)
		{
		iObserver->OnTransmitRefPosition(iStatus.Int(), iRefPosInfo);		
		}
	}

void CTLbsX3pRefLocWaiter::DoCancel()
	{
	
	}
	
TInt CTLbsX3pRefLocWaiter::RunError(TInt /* aError */)
	{// if RunError is called then a test failed so stop the active scheduler
	CActiveScheduler::Stop();
	return KErrNone;
	}
	


CTLbsX3pTransmitLocWaiter::CTLbsX3pTransmitLocWaiter(MTransmitServerObserver* aObserver):CActive(EPriorityStandard), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);	
	}


CTLbsX3pTransmitLocWaiter::~CTLbsX3pTransmitLocWaiter()
	{
	if(IsActive())
		Cancel();
	
	if(IsAdded())
		Deque();		
	}

CTLbsX3pTransmitLocWaiter* CTLbsX3pTransmitLocWaiter::NewL(MTransmitServerObserver* aObserver)
	{
	CTLbsX3pTransmitLocWaiter* self = new(ELeave)CTLbsX3pTransmitLocWaiter(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;		
	}
	
void CTLbsX3pTransmitLocWaiter::ConstructL()
	{
		
	}

void CTLbsX3pTransmitLocWaiter::Activate()
	{
	SetActive();
	}

void CTLbsX3pTransmitLocWaiter::RunL()
	{
	if (iObserver)
		{
		iObserver->OnTransmitPosition(iStatus.Int(), iTransmitPosInfo);		
		}
	}
	
void CTLbsX3pTransmitLocWaiter::DoCancel()
{

}

TInt CTLbsX3pTransmitLocWaiter::RunError(TInt /* aError */)
{// if RunError is called then a test failed so stop the active scheduler
CActiveScheduler::Stop();
return KErrNone;
}
