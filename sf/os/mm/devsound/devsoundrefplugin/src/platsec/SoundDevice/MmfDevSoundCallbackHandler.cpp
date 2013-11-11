// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SoundDeviceBody.h"
#include "MmfDevSoundCallbackHandler.h"
#include "MmfAudioClientServer.h"

CMsgQueueHandler* CMsgQueueHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy,
										 MDevSoundObserver& aDevSoundObserver,
										 RMsgQueue<TMMFDevSoundQueueItem>* aMsgQueue,
										 MMMFDevSoundCustomInterfaceObserver& aDevSoundIntfcObserver)
	{
	CMsgQueueHandler* self = new(ELeave) CMsgQueueHandler(aDevSoundProxy, aDevSoundObserver, aMsgQueue, aDevSoundIntfcObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CMsgQueueHandler::SetObserver(MDevSoundObserver& aDevSoundObserver)
	{
	iDevSoundObserver = &aDevSoundObserver;
	}

CMsgQueueHandler::CMsgQueueHandler(RMMFDevSoundProxy* aDevSoundProxy,
								   MDevSoundObserver& aDevSoundObserver,
								   RMsgQueue<TMMFDevSoundQueueItem>* aMsgQueue,
								   MMMFDevSoundCustomInterfaceObserver& aDevSoundIntfcObserver) :
								   CActive(EPriorityStandard), iDevSoundProxy(aDevSoundProxy),
								   iDevSoundObserver(&aDevSoundObserver), iMsgQueue(aMsgQueue), iDevSoundIntfcObserver(aDevSoundIntfcObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMsgQueueHandler::ConstructL()
	{
	}

CMsgQueueHandler::~CMsgQueueHandler()
	{
	delete iBuffer;
	Cancel();
	iMsgQueue->Close();
	}

void CMsgQueueHandler::ReceiveEvents()
	{
	iMsgQueue->NotifyDataAvailable(iStatus);
	SetActive();
	}

void CMsgQueueHandler::RunL()
	{
	TInt queueErr = iMsgQueue->Receive(iCurrentItem);	
	TInt err = KErrNone;

	if (!queueErr || queueErr == KErrUnderflow)
		{
		// Signal that we're ready to process the next message
		iMsgQueue->NotifyDataAvailable(iStatus);
		SetActive();							
		}

	if (!queueErr)
		{	
		iCurrentRequest = iCurrentItem.iRequest;
		switch (iCurrentRequest)
			{
			case EMMFDevSoundProxyICEvent:
				DoInitComplete();
				break;
			case EMMFDevSoundProxyBTBFEvent:
				{
				TRAP(err, DoBTBFCompleteL());
				if (err)
					{
					iDevSoundObserver->PlayError(err);
					}
				}
				break;
			case EMMFDevSoundProxyBTBEEvent:
				{
				TRAP(err, DoBTBECompleteL());
				if (err)
					{
					iDevSoundObserver->RecordError(err);
					}
				}
				break;					
			case EMMFDevSoundProxyPEEvent:
				DoPlayErrorComplete();
				break;
			case EMMFDevSoundProxyREEvent:
				DoRecordErrorComplete();
				break;
			case EMMFDevSoundProxyTFEvent:
				DoToneFinishedComplete();
				break;
			case EMMFDevSoundProxySETCEvent:
				DoSendEventToClientComplete();
				break;
			case EMMFDevSoundCustomCommandCloseMuxDemuxPair:
				{
				TMMFEvent pckgevent = iCurrentItem.iEventPckg();
				TInt handle = pckgevent.iEventType.iUid;
				iDevSoundIntfcObserver.CloseCustomInterface(handle);
				}
				break;
			default:
				break;
			}
		
		}
	}

TInt CMsgQueueHandler::RunError(TInt aError)
	{
	TMMFEvent event;
	event.iErrorCode = aError;
	iDevSoundObserver->SendEventToClient(event);
	return KErrNone;
	}
	
void CMsgQueueHandler::DoCancel()
	{
	iMsgQueue->CancelDataAvailable();
	}

void CMsgQueueHandler::DoInitComplete()
	{
	iDevSoundObserver->InitializeComplete(iCurrentItem.iErrorCode);
	}

void CMsgQueueHandler::DoPlayErrorComplete()
	{
	iDevSoundObserver->PlayError(iCurrentItem.iErrorCode);
	}

void CMsgQueueHandler::DoBTBFCompleteL()
	{
	// Wanted to push both the message request and the message's data
	// through the queue in one go but the data is too large so try
	// issuing a new request for the data before doing all this...
	User::LeaveIfError(iDevSoundProxy->BufferToBeFilledData(iSetPckg));
	
	TMMFDevSoundProxyHwBuf set;
	set.iBufferType = iSetPckg().iBufferType;
	set.iRequestSize = iSetPckg().iRequestSize;
	set.iBufferSize = iSetPckg().iBufferSize;
	set.iLastBuffer = iSetPckg().iLastBuffer;

	CMMFDataBuffer* buffer = CMMFDescriptorBuffer::NewL(set.iBufferSize);
	CleanupStack::PushL(buffer);
	buffer->SetRequestSizeL(set.iRequestSize);
	CleanupStack::Pop(buffer);
	buffer->SetLastBuffer(set.iLastBuffer);
	iDevSoundProxy->SetBuffer(buffer);//DevSoundProxy takes the ownership of "buffer"

	iDevSoundObserver->BufferToBeFilled(buffer);
	}

void CMsgQueueHandler::DoBTBECompleteL()
	{
	User::LeaveIfError(iDevSoundProxy->BufferToBeEmptiedData(iSetPckg));
	
	TMMFDevSoundProxyHwBuf aSet;
	aSet.iBufferType = iSetPckg().iBufferType;
	aSet.iRequestSize = iSetPckg().iRequestSize;
	aSet.iBufferSize = iSetPckg().iBufferSize;
	aSet.iLastBuffer = iSetPckg().iLastBuffer;

	if(iBuffer)
		{
		delete iBuffer;
		iBuffer = NULL;
		}
	iBuffer = CMMFDescriptorBuffer::NewL(aSet.iBufferSize);
	iBuffer->SetRequestSizeL(aSet.iRequestSize);
	iBuffer->SetLastBuffer(aSet.iLastBuffer);
	
	User::LeaveIfError(iDevSoundProxy->GetRecordedBufferL(*iBuffer));  

	iDevSoundObserver->BufferToBeEmptied(iBuffer);
	}

void CMsgQueueHandler::DoRecordErrorComplete()
	{
	iDevSoundObserver->RecordError(iCurrentItem.iErrorCode);
	}
	
void CMsgQueueHandler::DoToneFinishedComplete()
	{
	iDevSoundObserver->ToneFinished(iCurrentItem.iErrorCode);
	}
	
void CMsgQueueHandler::DoSendEventToClientComplete()
	{
	iDevSoundObserver->SendEventToClient(iCurrentItem.iEventPckg());	
	}
