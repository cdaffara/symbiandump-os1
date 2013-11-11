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
 @file lbsnrhx3pchannel.cpp
*/

#include <test/testexecutelogger.h>
#include <test/testexecutelogext.h>
#include "lbsnrhx3pchannel.h"


CX3pRequest::CX3pRequest(MX3pRequestObserver& aObserver,
						 TUint aRequestId) :
	CActive(EPriorityHigh),
	iObserver(aObserver),
	iRequestId(aRequestId)
	{
	CActiveScheduler::Add(this);
	}
	
CX3pRequest::~CX3pRequest()
	{
	Cancel();
	delete iRefPosMonitor;
	iTransmitPosition.Close();
	}

CX3pRequest* CX3pRequest::NewL(
	MX3pRequestObserver& aObserver, 
	RLbsTransmitPositionServer& aServer,
	TUint aRequestId)
	{
	CX3pRequest* self = new (ELeave) CX3pRequest(aObserver, aRequestId);
	CleanupStack::PushL(self);
	self->ConstructL(aServer);
	CleanupStack::Pop(self);
	return self;
	}

void CX3pRequest::ConstructL(RLbsTransmitPositionServer& aServer)
	{
	User::LeaveIfError(iTransmitPosition.Open(aServer));
	iRefPosMonitor = CX3pRefPosMonitor::NewL(*this);
	}

void CX3pRequest::DoTransmitPosition(
		TUint aRequestId,
		const TDesC& aDestinationID,
		TUint aTransmitPriority,
		TBool aGetReferencePosUpdate)
	{
	__ASSERT_ALWAYS(!IsActive(), User::Invariant());
	
	iRequestId = aRequestId;
	
	if (aGetReferencePosUpdate)
		{
		iTransmitPosition.TransmitPosition(aDestinationID, 
										   aTransmitPriority, 
										   iRefPosMonitor->iStatus,
										   iRefPosInfo,
										   iStatus, 
										   iFinalPosInfo);
		SetActive();
		iRefPosMonitor->StartMonitor();
		}
	else
		{
		iTransmitPosition.TransmitPosition(aDestinationID, 
										   aTransmitPriority, 
										   iStatus, 
										   iFinalPosInfo);
		SetActive();
		}
	}

void CX3pRequest::DoCancelTransmitPosition()
	{
	iTransmitPosition.CancelTransmitPosition();
	}
		
void CX3pRequest::OnReferencePositionUpdate()
	{
	iObserver.OnX3pReferencePositionUpdate(iRequestId, iRefPosInfo);
	}

void CX3pRequest::RunL()
	{
	iObserver.OnX3pCompletion(iRequestId, iStatus.Int(), iFinalPosInfo);
	}
	
void CX3pRequest::DoCancel()
	{
	iTransmitPosition.CancelTransmitPosition();
	}
	
TInt CX3pRequest::RunError(TInt& /*aError*/)
	{
	return KErrNone;
	}


		
CX3pRequest::CX3pRefPosMonitor::CX3pRefPosMonitor(CX3pRequest& aX3pRequest) :
	CActive(EPriorityHigh),
	iX3pRequest(aX3pRequest)
	{
	CActiveScheduler::Add(this);
	}
	
CX3pRequest::CX3pRefPosMonitor::~CX3pRefPosMonitor()
	{
	Cancel();
	}
	
CX3pRequest::CX3pRefPosMonitor* CX3pRequest::CX3pRefPosMonitor::NewL(CX3pRequest& aX3pRequest)
	{
	CX3pRefPosMonitor* self = new (ELeave) CX3pRefPosMonitor(aX3pRequest);
	return self;	
	}

void CX3pRequest::CX3pRefPosMonitor::StartMonitor()
	{
	SetActive();
	}

void CX3pRequest::CX3pRefPosMonitor::RunL()
	{
	iX3pRequest.OnReferencePositionUpdate();
	}
	
void CX3pRequest::CX3pRefPosMonitor::DoCancel()
	{
	}
	
TInt CX3pRequest::CX3pRefPosMonitor::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}


//
//
//

	
CX3pChannel::CX3pChannel(CTestExecuteLogger& aLogger) :
	iLogger(aLogger)
	{
	
	}
	
CX3pChannel::~CX3pChannel()
	{
	iRequestBuffer.ResetAndDestroy();
	iResponseBuffer.ResetAndDestroy();
	iTransmitPositionServer.Close();
	}

CX3pChannel* CX3pChannel::NewL(CTestExecuteLogger& aLogger)
	{
	CX3pChannel* self = new (ELeave) CX3pChannel(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CX3pChannel::ConstructL()
	{	
	}

TBool CX3pChannel::IsClientIdEqual(
		const TUint* aClientId, 
		const CX3pRequest& aRequest)
	{
	return (*aClientId == aRequest.ClientId());
	}

void CX3pChannel::ConnectClientL(TUint aRequestId)
	{
	INFO_PRINTF1(_L("<<<< X3P Client: Connect to client :"));
	INFO_PRINTF2(_L("-> Client Id : %d"), aRequestId);
	
	TInt index = iRequestBuffer.Find(aRequestId, IsClientIdEqual);
	if (index != KErrNotFound)
		{
		User::Leave(KErrInUse);
		}
	
	if (!iConnected)
		{
		User::LeaveIfError(iTransmitPositionServer.Connect());
		iConnected = ETrue;	
		}
	
	CX3pRequest* request = CX3pRequest::NewL(*this, iTransmitPositionServer, aRequestId);
	iRequestBuffer.AppendL(request);
	}

void CX3pChannel::DisconnectClient(TUint aRequestId)
	{
	INFO_PRINTF1(_L("<<<< X3P Client: Disconnect from server :"));
	INFO_PRINTF2(_L("-> Client Id : %d"), aRequestId);	

	TInt index = iRequestBuffer.Find(aRequestId, IsClientIdEqual);
	if (index != KErrNotFound)
		{
		CX3pRequest* request = iRequestBuffer[index];
		iRequestBuffer.Remove(index);
		delete request;
		
		// If there are no active subsessions, disconnect from the server
		if (iRequestBuffer.Count() == 0)
			{
			iTransmitPositionServer.Close();
			iConnected = EFalse;
			}
		}
	}

/*
*/
void CX3pChannel::SendTransmitLocateRequestL(
		TUint aRequestId,
		const TDesC& aDestinationID,
		TUint aTransmitPriority,
		TBool aGetReferencePosUpdate)
	{
	INFO_PRINTF1(_L("<<<< X3P Client: Send transmit locate :"));
	INFO_PRINTF2(_L("-> Client Id : %d"), aRequestId);
	
	TInt index = iRequestBuffer.Find(aRequestId, IsClientIdEqual);
	if (index >= 0)
		{
		iRequestBuffer[index]->DoTransmitPosition(aRequestId, 
												aDestinationID, 
												aTransmitPriority, 
												aGetReferencePosUpdate);	
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}

void CX3pChannel::SendCancelTransmitLocateRequestL(
		TUint aRequestId)
	{
	INFO_PRINTF1(_L("<<<< X3P Client: Send cancel transmit locate :"));
	INFO_PRINTF2(_L("-> Client Id : %d"), aRequestId);
	
	TInt index = iRequestBuffer.Find(aRequestId, IsClientIdEqual);
	if (index >= 0)
		{
		iRequestBuffer[index]->DoCancelTransmitPosition();
		}
	else
		{
		User::Leave(KErrNotFound);
		}	
	}

/*
*/
void CX3pChannel::WaitForResponseL(TTimeIntervalSeconds /*aTimeout*/)
	{
	if (iResponseBuffer.Count() == 0)
		{
		// Start the active scheduler - this should be stopped
		// when one of the 'OnX3p...' callbacks is called.
		iWait.Start();
		}	
	}

/*
*/	
CX3pChannel::TX3pResponseType CX3pChannel::GetNextResponseType()
	{
	if (iResponseBuffer.Count() == 0)
		{
		return CX3pChannel::EX3pResponseUnknown;
		}
	else
		{
		return (iResponseBuffer[0]->Type());
		}	
	}

/*
*/	
void CX3pChannel::GetNextResponseAsReferencePositionUpdateL(
		TUint& aRequestId,
		TPositionInfoBase& aRefPosInfo)
	{
	if (iResponseBuffer.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	if (iResponseBuffer[0]->Type() != CX3pChannel::EX3pResponseRefPosUpdate)
		{
		User::Leave(KErrArgument);
		}

	// Take the response out of the buffer and copy data.
	TX3pRefPosUpdate* refPos = static_cast<TX3pRefPosUpdate*>(iResponseBuffer[0]);
	iResponseBuffer.Remove(0);
	aRequestId = refPos->iRequestId;
	Mem::Copy(&aRefPosInfo,
			  &refPos->iRefPosInfo,
			  Min(TInt(refPos->iRefPosInfo.PositionClassSize()), 
				  TInt(aRefPosInfo.PositionClassSize())));
	delete refPos;	
	}

/*
*/	
void CX3pChannel::GetNextResponseAsRequestCompleteL(
		TUint& aRequestId,
		TInt& aReason,
		TPositionInfoBase& aPosInfo)
	{
	if (iResponseBuffer.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	if (iResponseBuffer[0]->Type() != CX3pChannel::EX3pResponseComplete)
		{
		User::Leave(KErrArgument);
		}

	// Take the response out of the buffer and copy data.
	TX3pReqComplete* reqComplete = static_cast<TX3pReqComplete*>(iResponseBuffer[0]);
	iResponseBuffer.Remove(0);
	aRequestId = reqComplete->iRequestId;
	aReason = reqComplete->iReason;
	Mem::Copy(&aPosInfo,
			  &reqComplete->iPosInfo,
			  Min(TInt(reqComplete->iPosInfo.PositionClassSize()), 
				  TInt(aPosInfo.PositionClassSize())));
	delete reqComplete;	
	}

/*
*/
void CX3pChannel::OnX3pReferencePositionUpdate(
	TUint aRequestId, 
	const TPositionInfoBase& aRefPosInfo)
	{
	INFO_PRINTF1(_L(">>>> X3P Client: Received reference position :"));
	INFO_PRINTF2(_L("-> Client Id : %d"), aRequestId);
	
	TX3pRefPosUpdate* refPos = new TX3pRefPosUpdate;
	if (NULL != refPos)
		{
		refPos->iRequestId = aRequestId;
		Mem::Copy(&refPos->iRefPosInfo,
				  &aRefPosInfo,
				  Min(TInt(aRefPosInfo.PositionClassSize()), 
					  TInt(refPos->iRefPosInfo.PositionClassSize())));
		iResponseBuffer.Append(refPos);
		}

	if (iWait.IsStarted())
		{
		iWait.AsyncStop();
		}	
	}

/*
*/
void CX3pChannel::OnX3pCompletion(
	TUint aRequestId, 
	TInt aReason,
	const TPositionInfoBase& aPosInfo)
	{
	INFO_PRINTF1(_L(">>>> X3P Client: Request completed :"));
	INFO_PRINTF2(_L("-> Client Id : %d"), aRequestId);
	INFO_PRINTF2(_L("-> Reason : %d"), aReason);
	
	TX3pReqComplete* reqComp = new TX3pReqComplete;
	if (NULL != reqComp)
		{
		reqComp->iRequestId = aRequestId;
		reqComp->iReason = aReason;
		Mem::Copy(&reqComp->iPosInfo,
				  &aPosInfo,
				  Min(TInt(aPosInfo.PositionClassSize()), 
					  TInt(reqComp->iPosInfo.PositionClassSize())));
		iResponseBuffer.Append(reqComp);
		}

	if (iWait.IsStarted())
		{
		iWait.AsyncStop();
		}	
	}

/** Convert a TX3pResponseType into a string.
*/
const TDesC& CX3pChannel::X3pResponseTypeToDesC(CX3pChannel::TX3pResponseType aType)
	{
	_LIT(KX3pResponseRefPosUpdate, "EX3pResponseRefPosUpdate");
	_LIT(KX3pResponseComplete, "EX3pResponseComplete");
	_LIT(KX3pResponseUnknown, "EX3pResponseUnknown");
	
	switch (aType)
		{
		case CX3pChannel::EX3pResponseRefPosUpdate:
			{
			return KX3pResponseRefPosUpdate;
			}
		case CX3pChannel::EX3pResponseComplete:
			{
			return KX3pResponseComplete;
			}
		case CX3pChannel::EX3pResponseUnknown:
		default:
			{
			return KX3pResponseUnknown;
			}
		}
	}

