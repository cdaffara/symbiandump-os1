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

/**
 @file lbsnrhprivctrlchannel.cpp
*/

#include <test/testexecutelogger.h>
#include <test/testexecutelogext.h>
#include <lbs/lbslocclasstypes.h>
#include "lbsnrhprivctrlchannel.h"


CPrivCtrlChannel::CPrivCtrlChannel(CTestExecuteLogger& aLogger) :
	CActive(EPriorityHigh),
	iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}
	
CPrivCtrlChannel::~CPrivCtrlChannel()
	{
	Cancel();
	delete iPrivController;
	iResponseBuffer.ResetAndDestroy();
	}
	
CPrivCtrlChannel* CPrivCtrlChannel::NewL(CTestExecuteLogger& aLogger)
	{
	CPrivCtrlChannel* self = new (ELeave) CPrivCtrlChannel(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CPrivCtrlChannel::ConstructL()
	{
	iPrivController = CLbsPrivacyController::NewL(*this);
	}
		
void CPrivCtrlChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	if (iWait.IsStarted())
		{
		iWait.AsyncStop();
		}
	}
	
void CPrivCtrlChannel::DoCancel()
	{
	// Cancel Timer?
	}

void CPrivCtrlChannel::SendPrivacyResponse(
		TUint aRequestId,
		CLbsPrivacyController::TRequestVerificationResult aResponse,
		TTimeIntervalSeconds /*aTimeout*/)
	{
	iPrivController->RespondNetworkLocationRequest(aRequestId, aResponse);
	}

void CPrivCtrlChannel::SendCancelPrivacyRequest(
		TUint aRequestId,
		TTimeIntervalSeconds /*aTimeout*/)
	{
	iPrivController->CancelNetworkLocationRequest(aRequestId);
	}

/* Wait for the next callback from the privacy controller API.
 */
void CPrivCtrlChannel::WaitForResponseL(
		TTimeIntervalSeconds /*aTimeout*/)
	{
	if (iResponseBuffer.Count() == 0)
		{
		// Start the active scheduler - this should be stopped
		// when one of the callbacks in MLbsPrivacyController
		// is called.
		iWait.Start();
		}
	}

CPrivCtrlChannel::TPrivCtrlResponseType CPrivCtrlChannel::GetNextResponseType()
	{
	if (iResponseBuffer.Count() == 0)
		{
		return CPrivCtrlChannel::EDataResponseUnknown;
		}
	else
		{
		CPrivCtrlChannel::TPrivCtrlResponseType type = iResponseBuffer[0]->Type();
		return (type);
		}
	}

void CPrivCtrlChannel::GetNextResponseAsPrivacyRequestL(
		TUint& aRequestId,
		MLbsPrivacyObserver::TNotificationType& aNotificationType,
		TLbsExternalRequestInfo& aRequestInfo)
	{
	if (iResponseBuffer.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	if (iResponseBuffer[0]->Type() != CPrivCtrlChannel::EDataResponsePrivReq)
		{
		User::Leave(KErrArgument);
		}

	// Take the response out of the buffer and copy data.
	TPrivCtrlPrivReq* privReq = static_cast<TPrivCtrlPrivReq*>(iResponseBuffer[0]);
	iResponseBuffer.Remove(0);
	aRequestId = privReq->iRequestId;
	aNotificationType = privReq->iNotificationType;
	if (aRequestInfo.ClassSize() <= privReq->iRequestInfo.ClassSize())
		{
		const TInt offset = sizeof(TLbsLocClassTypeBase);
		TAny* dst = (TAny*)((TInt)(&aRequestInfo) + offset);
		TAny* src = (TAny*)((TInt)(&privReq->iRequestInfo) + offset);
		TInt bytes = (aRequestInfo.ClassSize() - offset);
		Mem::Copy(dst, src, bytes);
		}
	else
		{
		Mem::Copy(&aRequestInfo,
				  &privReq->iRequestInfo,
				  privReq->iRequestInfo.ClassSize());		
		}
	delete privReq;
	}

void CPrivCtrlChannel::GetNextResponseAsPositionUpdateL(
		TUint& aRequestId,
		TPositionInfoBase& aPosInfo)
	{
	if (iResponseBuffer.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	if (iResponseBuffer[0]->Type() != CPrivCtrlChannel::EDataResponsePosUpdate)
		{
		User::Leave(KErrArgument);
		}

	// Take the response out of the buffer and copy data.
	TPosUpdate* posUpdate = static_cast<TPosUpdate*>(iResponseBuffer[0]);
	iResponseBuffer.Remove(0);
	aRequestId = posUpdate->iRequestId;
	Mem::Copy(&aPosInfo,
			  &posUpdate->iPosInfo,
			  Min(TInt(aPosInfo.PositionClassSize()),
				  TInt(posUpdate->iPosInfo.PositionClassSize())));
	delete posUpdate;
	}

void CPrivCtrlChannel::GetNextResponseAsRequestCompleteL(
		TUint& aRequestId,
		TInt& aReason)
	{
	if (iResponseBuffer.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	if (iResponseBuffer[0]->Type() != CPrivCtrlChannel::EDataResponseReqComplete)
		{
		User::Leave(KErrArgument);
		}

	// Take the response out of the buffer and copy data.
	TReqComplete* reqComplete = static_cast<TReqComplete*>(iResponseBuffer[0]);
	iResponseBuffer.Remove(0);
	aRequestId = reqComplete->iRequestId;
	aReason = reqComplete->iReason;
	delete reqComplete;
	}

void CPrivCtrlChannel::ProcessNetworkLocationRequest(
		TUint aRequestId, 
		const TLbsExternalRequestInfo& aRequestInfo, 
		const TNotificationType& aNotificationType)
	{
	TPrivCtrlPrivReq* privReq = new TPrivCtrlPrivReq;
	if (NULL != privReq)
		{
		privReq->iRequestId = aRequestId;
		privReq->iNotificationType = aNotificationType;
		Mem::Copy(&privReq->iRequestInfo,
				  &aRequestInfo,
				  Min(TInt(aRequestInfo.ClassSize()), 
					  TInt(privReq->iRequestInfo.ClassSize())));
		iResponseBuffer.Append(privReq);
		}
	
	if (iWait.IsStarted())
		{
		iWait.AsyncStop();
		}
	}

void CPrivCtrlChannel::ProcessNetworkPositionUpdate(
		TUint aRequestId, 
		const TPositionInfo& aPosInfo)
	{
	TPosUpdate* posUpdate = new TPosUpdate;
	if (NULL != posUpdate)
		{
		posUpdate->iRequestId = aRequestId;
		Mem::Copy(&posUpdate->iPosInfo,
				  &aPosInfo,
				  Min(TInt(aPosInfo.PositionClassSize()), 
					  TInt(posUpdate->iPosInfo.PositionClassSize())));
		iResponseBuffer.Append(posUpdate);
		}
	
	if (iWait.IsStarted())
		{
		iWait.AsyncStop();
		}
	}

void CPrivCtrlChannel::ProcessRequestComplete(
		TUint aRequestId, 
		TInt aReason)
	{
	TReqComplete* reqComplete = new TReqComplete;
	if (NULL != reqComplete)
		{
		reqComplete->iRequestId = aRequestId;
		reqComplete->iReason = aReason;
		iResponseBuffer.Append(reqComplete);
		}
	
	if (iWait.IsStarted())
		{
		iWait.AsyncStop();
		}
	}

