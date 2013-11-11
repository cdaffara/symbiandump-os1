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
// Async wrapper around a LBS X3P transmitter.
// 
//

/**
 @file ctlbsdox3p.cpp
*/

//#include <lbs/lbsx3p.h>

#include "ctlbsdox3p.h"


CT_LbsDoX3P* CT_LbsDoX3P::NewL(MT_LbsDoX3PObserver* aObserver, TInt aTransmitId)
	{
	CT_LbsDoX3P* self = new(ELeave)CT_LbsDoX3P(aObserver, aTransmitId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}


CT_LbsDoX3P::CT_LbsDoX3P(MT_LbsDoX3PObserver* aObserver, TInt aTransmitId) : CActive(EPriorityStandard), iObserver(aObserver), iTransmitId(aTransmitId)
	{ 	
	CActiveScheduler::Add(this); 
	}


void CT_LbsDoX3P::ConstructL()
	{
	User::LeaveIfError(iServer.Connect());
	User::LeaveIfError(iTransmitter.Open(iServer));
	}
	

CT_LbsDoX3P::~CT_LbsDoX3P()
	{
	Cancel();

	iTransmitter.Close();
	iServer.Close();
	}


TInt CT_LbsDoX3P::SetOptions(const TLbsTransmitPositionOptions& aTransmitOptions)
	{
	return iTransmitter.SetTransmitOptions(aTransmitOptions);
	}


void CT_LbsDoX3P::StartL(const TDesC& aDestinationID, TUint aTransmitPriority, TPositionInfo & aTransmittedPosInfo)
	{	
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KCT_LbsDoX3P, KErrInUse));

	// Invoke the X3P transmit update.	
	iTransmitter.TransmitPosition(aDestinationID, aTransmitPriority, iStatus, aTransmittedPosInfo);
	SetActive();
	}


void CT_LbsDoX3P::DoCancel()
	{
	iTransmitter.CancelTransmitPosition(); 
	}


void CT_LbsDoX3P::RunL()
	{
	// Notify caller via callback, process the err in the callback.
	if (iObserver)
		{
		iObserver->MT_LbsDoX3PCallback(iTransmitId, iStatus);
		}
	}

TInt CT_LbsDoX3P::RunError(TInt aError)
	{
	return aError;
	}

