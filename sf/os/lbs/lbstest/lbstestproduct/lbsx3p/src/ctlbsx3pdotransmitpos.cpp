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
// ao wrapper around the a-gps mgr function NotifyPositionUpdate
// 
//

/**
 @file ctlbsx3pdotransmitpos.cpp
*/

#include <lbs/lbsx3p.h>

#include "ctlbsx3pdotransmitpos.h"


CT_LbsX3PDoTransmitPos* CT_LbsX3PDoTransmitPos::NewL(MT_LbsX3PDoTransmitPosObserver* aObserver, RLbsTransmitPosition& aPositionTransmitter)
/**
 *	'public constructor' may leave
 */
	{
	CT_LbsX3PDoTransmitPos* doTransmitPos = new(ELeave)CT_LbsX3PDoTransmitPos(aObserver, aPositionTransmitter);
	CleanupStack::PushL(doTransmitPos);
	doTransmitPos->ConstructL();
	CleanupStack::Pop(doTransmitPos);

	return doTransmitPos;
	}


CT_LbsX3PDoTransmitPos::CT_LbsX3PDoTransmitPos(MT_LbsX3PDoTransmitPosObserver* aObserver, RLbsTransmitPosition& aPositionTransmitter) : CActive(EPriorityStandard), iObserver(aObserver), iPositionTransmitter(aPositionTransmitter)
/**
 * 	Constructor - will not leave
 */
	{
	CActiveScheduler::Add(this); 
	}


void CT_LbsX3PDoTransmitPos::ConstructL()
	{
	iRefLocWaiter = CT_LbsX3pRefLocWaiter::NewL(iObserver);
	}
	

CT_LbsX3PDoTransmitPos::~CT_LbsX3PDoTransmitPos()
/**
 * 	Destructor
 */
	{
	Cancel();
	delete iRefLocWaiter;
	iPositionTransmitter.Close();
	}

void CT_LbsX3PDoTransmitPos::TransmitPosition(const TDesC& aDestinationID, TUint aTransmitPriority, TPositionInfo & aTransmittedPosInfo)
/**
 * 	wrapper for async  function RLbsPositionUpdates::NotifyPositionUpdate(). 
 *  Will panic if there's another outstanding request.
 */
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KCT_LbsX3PDoTransmitPos, KErrInUse));

	// Invoke the position update.	
	iPositionTransmitter.TransmitPosition(aDestinationID, aTransmitPriority, iStatus, aTransmittedPosInfo);
	SetActive();
	}

void CT_LbsX3PDoTransmitPos::TransmitPositionReportingRefLoc(const TDesC& aDestinationID, TUint aTransmitPriority, TPositionInfo & aTransmittedPosInfo)
/**
 * 	wrapper for async  function RLbsPositionUpdates::NotifyPositionUpdate(). 
 *  Will panic if there's another outstanding request.
 */
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KCT_LbsX3PDoTransmitPos, KErrInUse));

	// Invoke the position update.	
	iPositionTransmitter.TransmitPosition(aDestinationID, aTransmitPriority, iRefLocWaiter->iStatus, iRefLocWaiter->iRefPosInfo, iStatus, aTransmittedPosInfo);
	iRefLocWaiter->Activate();
	SetActive();
	}

void CT_LbsX3PDoTransmitPos::DoCancel()
	{
	iPositionTransmitter.CancelTransmitPosition(); 
	}

void CT_LbsX3PDoTransmitPos::RunL()
	{
	// iStatus will contain error code
	// async request completed. Notify caller via callback:
	if (iObserver)
		{
		iObserver->MT_LbsX3PDoTransmitPosCallback(iStatus, this);
		}
	}

TInt CT_LbsX3PDoTransmitPos::RunError(TInt aError)
	{
	return aError;
	}



CT_LbsX3pRefLocWaiter* CT_LbsX3pRefLocWaiter::NewL(MT_LbsX3PDoTransmitPosObserver* aObserver)
	{
	CT_LbsX3pRefLocWaiter* self = new(ELeave)CT_LbsX3pRefLocWaiter(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CT_LbsX3pRefLocWaiter::CT_LbsX3pRefLocWaiter(MT_LbsX3PDoTransmitPosObserver* aObserver):CActive(EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

void CT_LbsX3pRefLocWaiter::ConstructL()
	{
	}
	
CT_LbsX3pRefLocWaiter::~CT_LbsX3pRefLocWaiter()
	{
	Cancel();
	}
	
void CT_LbsX3pRefLocWaiter::RunL()
	{
	// Let the test know the returned status and the position in iRefPosInfo,
	// which is the Reference Location if the iStatus is KErrNone.
	if (iObserver)
		{
		iObserver->MT_LbsX3PDoRefPosCallback(iStatus, iRefPosInfo);
		}
	}
	
void CT_LbsX3pRefLocWaiter::DoCancel()
	{
	// Nothing to do. The ref loc request is cancelled when
	// TransmitPosition is cancelled.
	}

TInt CT_LbsX3pRefLocWaiter::RunError(TInt aError)
	{
	return aError;
	}
	
void CT_LbsX3pRefLocWaiter::Activate()
	{
    __ASSERT_ALWAYS(!IsActive(), User::Panic(KCT_LbsX3PDoTransmitPos, KErrInUse));
	SetActive();	
	}
