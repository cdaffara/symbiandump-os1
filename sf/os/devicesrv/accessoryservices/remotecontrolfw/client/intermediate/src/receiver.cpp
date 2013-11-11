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

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "remconclient.h"
#include "receiver.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_IL_RECV);
#endif

CReceiver* CReceiver::NewL(RRemCon& aRemCon, 
						   CRemConInterfaceSelector& aObserver, 
						   TUint aMaxDataLength,
						   TRemConClientType aType)
	{
	LOG_STATIC_FUNC
	CReceiver* self = new(ELeave) CReceiver(aRemCon, aObserver, aType);
	CleanupStack::PushL(self);
	self->ConstructL(aMaxDataLength);
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
*/
CReceiver::~CReceiver()
	{
	LOG_FUNC
	Cancel();

	iData.Close();
	}

CReceiver::CReceiver(RRemCon& aRemCon, 
					 CRemConInterfaceSelector& aObserver, 
					 TRemConClientType aType)
:	CActive(CActive::EPriorityStandard),
	iType(aType),
	iRemCon(aRemCon),
	iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CReceiver::ConstructL(TUint aMaxDataLength)
	{
	iData.CreateL(aMaxDataLength);
	// NB If a remote sends longer data than we expect to receive here, our 
	// receive will be errored and the message will effectively be dropped in 
	// the intermediate layer. (It won't get passed to the outer layer.)

	// Start the perpetual receive cycle.
	Receive();
	}

void CReceiver::Receive()
	{
	LOG_FUNC

	iRemCon.Receive(iStatus, iReceivePackage, iData);
	SetActive();
	}

void CReceiver::RunL()
	{
	LOG_FUNC
	LOG1(_L("\tiStatus = %d"), iStatus.Int());

	// Record the result of our last request, because the stuff we
	// do depends on it, but may also issue further requests.
	TInt err = iStatus.Int();
	
	if(err == KErrNone)
		{
		iObserver.ReceiveComplete(iReceivePackage.iInterfaceUid, 
			iReceivePackage.iOperationId, 
			iReceivePackage.iMessageSubType, 
			iReceivePackage.iRemoteAddress, 
			iData, 
			iType);
		}
	else
		{
		iObserver.Error(err);
		}
		
	if(err != KErrServerTerminated)
		{
		// Repost request.
		Receive();
		}
	}

void CReceiver::DoCancel()
	{
	LOG_FUNC

	// There's nothing we can do about any error here, and it probably 
	// indicates that the server has gone away anyway for some reason.
	(void)iRemCon.ReceiveCancel();
	}
