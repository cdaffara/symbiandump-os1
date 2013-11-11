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

#include "MmfBtDevSoundCallbackHandler.h"

//
//BufferToBeFilled//
//
CMMFBufferToBeFilledHandler* CMMFBufferToBeFilledHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver)
	{
	CMMFBufferToBeFilledHandler* self = new(ELeave) CMMFBufferToBeFilledHandler(aDevSoundProxy, aDevSoundObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFBufferToBeFilledHandler::CMMFBufferToBeFilledHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver) :
	CActive(EPriorityLow), iDevSoundProxy(aDevSoundProxy), iDevSoundObserver(aDevSoundObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMMFBufferToBeFilledHandler::ConstructL()
	{
	}

CMMFBufferToBeFilledHandler::~CMMFBufferToBeFilledHandler()
	{
	Cancel();
	}

void CMMFBufferToBeFilledHandler::ReceiveEvents()
	{
	_LIT(KBTBFPanic, "CMMFBufferToBeFilledHandler");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KBTBFPanic, 1));

	iDevSoundProxy->ReceiveBufferToBeFilledEvent(iSetPckg, iStatus);
	SetActive();
	}

void CMMFBufferToBeFilledHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFBufferToBeFilledHandler::RunL()
	{
	TMMFDevSoundProxyHwBuf aSet;
	aSet.iBufferType = iSetPckg().iBufferType;
	aSet.iRequestSize = iSetPckg().iRequestSize;
	aSet.iBufferSize = iSetPckg().iBufferSize;
	aSet.iLastBuffer = iSetPckg().iLastBuffer;


	CMMFDataBuffer* buffer = CMMFDescriptorBuffer::NewL(aSet.iBufferSize);
	CleanupStack::PushL(buffer);
	buffer->SetRequestSizeL(aSet.iRequestSize);
	CleanupStack::Pop(buffer);
	buffer->SetLastBuffer(aSet.iLastBuffer);
	iDevSoundProxy->SetBuffer(buffer);//DevSoundProxy takes the owenership of "buffer"

	ReceiveEvents();
	iDevSoundObserver.BufferToBeFilled(buffer);
	}

void CMMFBufferToBeFilledHandler::DoCancel()
	{
	iDevSoundProxy->CancelReceiveBufferToBeFilledEvent();
	}

TInt CMMFBufferToBeFilledHandler::RunError(TInt aError)
	{
	// report error to client - client should make decesion what to do with the error.
	iDevSoundObserver.PlayError(aError);
	ReceiveEvents();
	
	return KErrNone;
	}


//
//BufferToBeEmptied//
//
CMMFBufferToBeEmptiedHandler* CMMFBufferToBeEmptiedHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver)
	{
	CMMFBufferToBeEmptiedHandler* self = new(ELeave) CMMFBufferToBeEmptiedHandler(aDevSoundProxy, aDevSoundObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFBufferToBeEmptiedHandler::CMMFBufferToBeEmptiedHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver) :
	CActive(EPriorityLow), iDevSoundProxy(aDevSoundProxy), iDevSoundObserver(aDevSoundObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMMFBufferToBeEmptiedHandler::ConstructL()
	{
	}

CMMFBufferToBeEmptiedHandler::~CMMFBufferToBeEmptiedHandler()
	{
	Cancel();
	delete iBuffer;
	}

void CMMFBufferToBeEmptiedHandler::ReceiveEvents()
	{
	_LIT(KBTBEPanic, "CMMFBufferToBeEmptiedHandler");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KBTBEPanic, 1));

	iDevSoundProxy->ReceiveBufferToBeEmptiedEvent(iSetPckg, iStatus);
	SetActive();
	}

void CMMFBufferToBeEmptiedHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFBufferToBeEmptiedHandler::RunL()
	{
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

	iDevSoundObserver.BufferToBeEmptied(iBuffer);
	ReceiveEvents();
	}

void CMMFBufferToBeEmptiedHandler::DoCancel()
	{
	iDevSoundProxy->CancelReceiveBufferToBeEmptiedEvent();
	}

TInt CMMFBufferToBeEmptiedHandler::RunError(TInt aError)
	{
	// report error to client - client should make decesion what to do with the error.
	iDevSoundObserver.RecordError(aError);
	ReceiveEvents();

	return KErrNone;
	}

//
//PlayError//
//
CMMFPlayErrorHandler* CMMFPlayErrorHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver)
	{
	CMMFPlayErrorHandler* self = new(ELeave) CMMFPlayErrorHandler(aDevSoundProxy, aDevSoundObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFPlayErrorHandler::CMMFPlayErrorHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver) :
	CActive(EPriorityStandard+1), iDevSoundProxy(aDevSoundProxy), iDevSoundObserver(aDevSoundObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMMFPlayErrorHandler::ConstructL()
	{
	}

CMMFPlayErrorHandler::~CMMFPlayErrorHandler()
	{
	Cancel();
	}

void CMMFPlayErrorHandler::ReceiveEvents()
	{
	_LIT(KPEPanic, "CMMFPlayErrorHandler");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KPEPanic, 1));

	iDevSoundProxy->ReceivePlayErrorEvent(iSetPckg, iStatus);
	SetActive();
	}

void CMMFPlayErrorHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFPlayErrorHandler::RunL()
	{
	iDevSoundObserver.PlayError(iStatus.Int());
	iStatus = KErrNone;
	ReceiveEvents();
	}

void CMMFPlayErrorHandler::DoCancel()
	{
	iDevSoundProxy->CancelReceivePlayErrorEvent();
	}

TInt CMMFPlayErrorHandler::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

//
//RecordError//
//
CMMFRecordErrorHandler* CMMFRecordErrorHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver)
	{
	CMMFRecordErrorHandler* self = new(ELeave) CMMFRecordErrorHandler(aDevSoundProxy, aDevSoundObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFRecordErrorHandler::CMMFRecordErrorHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver) :
	CActive(EPriorityLow), iDevSoundProxy(aDevSoundProxy), iDevSoundObserver(aDevSoundObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMMFRecordErrorHandler::ConstructL()
	{
	}

CMMFRecordErrorHandler::~CMMFRecordErrorHandler()
	{
	Cancel();
	}

void CMMFRecordErrorHandler::ReceiveEvents()
	{
	_LIT(KREPanic, "CMMFRecordErrorHandler");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KREPanic, 1));

	iDevSoundProxy->ReceiveRecordErrorEvent(iSetPckg, iStatus);
	SetActive();
	}

void CMMFRecordErrorHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFRecordErrorHandler::RunL()
	{
	iDevSoundObserver.RecordError(iStatus.Int());
	iStatus = KErrNone;
	ReceiveEvents();
	}

void CMMFRecordErrorHandler::DoCancel()
	{
	iDevSoundProxy->CancelReceiveRecordErrorEvent();
	}

TInt CMMFRecordErrorHandler::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

//
//ToneFinished//
//


CMMFToneFinishedHandler* CMMFToneFinishedHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver)
	{
	CMMFToneFinishedHandler* self = new(ELeave) CMMFToneFinishedHandler(aDevSoundProxy, aDevSoundObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFToneFinishedHandler::CMMFToneFinishedHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver) :
	CActive(EPriorityLow), iDevSoundProxy(aDevSoundProxy), iDevSoundObserver(aDevSoundObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMMFToneFinishedHandler::ConstructL()
	{
	}

CMMFToneFinishedHandler::~CMMFToneFinishedHandler()
	{
	Cancel();
	}

void CMMFToneFinishedHandler::ReceiveEvents()
	{
	_LIT(KTFPanic, "CMMFToneFinishedHandler");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KTFPanic, 1));

	iDevSoundProxy->ReceiveToneFinishedEvent(iSetPckg, iStatus);
	SetActive();
	}

void CMMFToneFinishedHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFToneFinishedHandler::RunL()
	{
	iDevSoundObserver.ToneFinished(iStatus.Int());
	ReceiveEvents();
	}

void CMMFToneFinishedHandler::DoCancel()
	{
	iDevSoundProxy->CancelReceiveToneFinishedEvent();
	}

TInt CMMFToneFinishedHandler::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

//
//InitializeComplete//
//


CMMFInitializeCompleteHandler* CMMFInitializeCompleteHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver)
	{
	CMMFInitializeCompleteHandler* self = new(ELeave) CMMFInitializeCompleteHandler(aDevSoundProxy, aDevSoundObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFInitializeCompleteHandler::CMMFInitializeCompleteHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver) :
	CActive(EPriorityLow), iDevSoundProxy(aDevSoundProxy), iDevSoundObserver(aDevSoundObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMMFInitializeCompleteHandler::ConstructL()
	{
	}

CMMFInitializeCompleteHandler::~CMMFInitializeCompleteHandler()
	{
	Cancel();
	}

void CMMFInitializeCompleteHandler::ReceiveEvents()
	{
	_LIT(KICPanic, "CMMFInitializeCompleteHandler");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KICPanic, 1));

	iDevSoundProxy->ReceiveInitializeCompleteEvent(iSetPckg, iStatus);
	SetActive();
	}

void CMMFInitializeCompleteHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFInitializeCompleteHandler::RunL()
	{
	iDevSoundObserver.InitializeComplete(iStatus.Int());
	iStatus = KErrNone;
	ReceiveEvents();
	}

void CMMFInitializeCompleteHandler::DoCancel()
	{
	iDevSoundProxy->CancelReceiveInitializeCompleteEvent();
	}

TInt CMMFInitializeCompleteHandler::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

//
//SendEventToClient//
//


CMMFSendEventToClientHandler* CMMFSendEventToClientHandler::NewL(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver)
	{
	CMMFSendEventToClientHandler* self = new(ELeave) CMMFSendEventToClientHandler(aDevSoundProxy, aDevSoundObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CMMFSendEventToClientHandler::CMMFSendEventToClientHandler(RMMFDevSoundProxy* aDevSoundProxy, MDevSoundObserver& aDevSoundObserver) :
	CActive(EPriorityLow), iDevSoundProxy(aDevSoundProxy), iDevSoundObserver(aDevSoundObserver)
	{
	CActiveScheduler::Add(this);
	}

void CMMFSendEventToClientHandler::ConstructL()
	{
	}

CMMFSendEventToClientHandler::~CMMFSendEventToClientHandler()
	{
	Cancel();
	}

void CMMFSendEventToClientHandler::ReceiveEvents()
	{
	_LIT(KSETCPanic, "CMMFSendEventToClientHandler");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KSETCPanic, 1));

	iDevSoundProxy->ReceiveSendEventToClientEvent(iEventPckg, iStatus);
	SetActive();
	}

void CMMFSendEventToClientHandler::CancelReceiveEvents()
	{
	Cancel();
	}

void CMMFSendEventToClientHandler::RunL()
	{
	iDevSoundObserver.SendEventToClient(iEventPckg());
	iStatus = KErrNone;
	ReceiveEvents();
	}

void CMMFSendEventToClientHandler::DoCancel()
	{
	iDevSoundProxy->CancelReceiveSendEventToClientEvent();
	}

TInt CMMFSendEventToClientHandler::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

