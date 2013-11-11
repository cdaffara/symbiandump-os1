// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <comms-infras/commsdebugutility.h>
#include <e32atomics.h>
#include "bulkreceiver.h"
#include "remconclient.h"
#include "remconbulkclient.h"
#include "utils.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_IF_BASE);
#endif

#ifdef _DEBUG
PANICCATEGORY("bulkrecv");
#endif // _DEBUG

CBulkReceiver* CBulkReceiver::NewL(CRemConInterfaceSelector& aObserver)
	{
	LOG_STATIC_FUNC;
	CBulkReceiver* self = new(ELeave) CBulkReceiver(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CBulkReceiver::~CBulkReceiver()
	{
	LOG_FUNC;
	Cancel();
	iConnectCall.Close();
	iData.Close();
	}

CBulkReceiver::CBulkReceiver(CRemConInterfaceSelector& aObserver)
	: CActive(CActive::EPriorityStandard)
	, iObserver(aObserver)
	{
	LOG_FUNC;
	}

void CBulkReceiver::ConstructL()
	{
	LOG_FUNC;
	TCallBack connectCb(StaticConnect, this);
	LEAVEIFERRORL(iConnectCall.Create(connectCb, EPriorityStandard));
	BindToCurrentThreadL();
	}

void CBulkReceiver::BindToCurrentThreadL()
	{
	LOG_FUNC;
	ASSERT_DEBUG(!IsAdded());
	TRAPD(err, DoBindToCurrentThreadL());
	if(err != KErrNone)
		{
		this->Deque();
		LEAVEL(err);
		}
	}

void CBulkReceiver::DoBindToCurrentThreadL()
	{
	LOG_FUNC;
	CActiveScheduler::Add(this);
	LEAVEIFERRORL(iConnectCall.Start());
	iThreadId = RThread().Id();
	}

TBool CBulkReceiver::InitialisationRequired() const
	{
	LOG_FUNC;
	return IsAdded();
	}

TInt CBulkReceiver::StaticConnect(TAny* aSelf)
	{
	LOG_STATIC_FUNC;
	ASSERT_DEBUG(aSelf);
	return reinterpret_cast<CBulkReceiver*>(aSelf)->Connect();
	}

TInt CBulkReceiver::Connect()
	{
	LOG_FUNC;
	// issue the connect from the bulk thread.
	TRAPD(err, iObserver.BulkSessionConnectL());
	if(err == KErrNone)
		{
		// Start the perpetual receive cycle.
		Receive();
		}
	return err;
	}

void CBulkReceiver::InitialiseL(RRemConBulk& aRemConBulk, TUint aMaxDataLength)
	{
	LOG_FUNC;
	iData.CreateL(aMaxDataLength);
	iRemConBulk = &aRemConBulk;
	// NB If a remote sends longer data than we expect to receive here, our 
	// receive will be errored and the message will effectively be dropped in 
	// the intermediate layer. (It won't get passed to the outer layer.)
	
	// Receive is queued once we have a connected bulk server session
	}

void CBulkReceiver::Receive()
	{
	LOG_FUNC
	ASSERT_DEBUG(iRemConBulk);
	iRemConBulk->Receive(iStatus, iInterfaceUid, iOperationId, iData);
	SetActive();
	}

void CBulkReceiver::RunL()
	{
	LOG_FUNC
	LOG1(_L8("\tiStatus = %d"), iStatus.Int());

	TInt err = iStatus.Int();
	
	if(err == KErrNone)
		{
		iObserver.BulkReceiveComplete(iInterfaceUid, iOperationId, iData);
		}
	else
		{
		iObserver.BulkError(err);
		}
	
	if(err != KErrServerTerminated)
		{
		// Repost request.
		Receive();
		}
	}

void CBulkReceiver::DoCancel()
	{
	LOG_FUNC
	
	if(RThread().Id() == iThreadId)
		{
		// There's nothing we can do about any error here, and it probably 
		// indicates that the server has gone away anyway for some reason.
		ASSERT_DEBUG(iRemConBulk);
		static_cast<void>(iRemConBulk->ReceiveCancel());
		
		// No need to cancel the "connecting" request as it is a synchronous kick
		}
	else
		{
		// being run in a different thread - this means that something has gone
		// wrong and we merely want to tidy up the CActive state (i.e. not active).
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrCancel);
		}
	}

/**
This function deals with connecting to the bulk server - it handles the
cases where the bulk interfaces are running in the same thread as the control
interfaces, and also when running in a different thread (which is as one might
expect more complicated). 
*/
void CBulkReceiver::WaitUntilConnectedL()
	{
	LOG_FUNC;
	LEAVEIFERRORL(iConnectCall.CallBack());
	}



