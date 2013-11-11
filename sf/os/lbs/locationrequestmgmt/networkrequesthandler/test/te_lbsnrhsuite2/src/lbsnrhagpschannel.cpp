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
 @file lbsnrhagpschannel.cpp
*/

#include <test/testexecutelogger.h>
#include <test/testexecutelogext.h>
#include "lbsnrhagpschannel.h"
#include "lbsprocessuiddefs.h"


// Channel Id for the NRH agps request channel.
const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierNRH = 
	{
		{KLbsGpsLocManagerUidValue}, {KLbsNetRequestHandlerUidValue}
	};


CAgpsChannel::CAgpsChannel(CTestExecuteLogger& aLogger) :
	CActive(EPriorityHigh),
	iLogger(aLogger)
	{
	CActiveScheduler::Add(this);
	}

CAgpsChannel::~CAgpsChannel()
	{
	Cancel();
	iAgpsMeasurementUpdateBus.Close();
	iAgpsPositionUpdateBus.Close();
	iRequestBuffer.ResetAndDestroy();
	iAgpsRequestChannel.Close();
	}

CAgpsChannel* CAgpsChannel::NewL(CTestExecuteLogger& aLogger)
	{
	CAgpsChannel* self = new (ELeave) CAgpsChannel(aLogger);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAgpsChannel::ConstructL()
	{
	iAgpsRequestChannel.OpenL(KChannelIdentifierNRH);
	iAgpsPositionUpdateBus.OpenL(KLbsGpsLocManagerUid);
	iAgpsMeasurementUpdateBus.OpenL();
	
	// Subscribe for the first AGPS request
	iAgpsRequestChannel.NotifyPositionUpdateRequestChange(iStatus);
	SetActive();
	}
	
void CAgpsChannel::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	// Re-subscribe for future requests
	iAgpsRequestChannel.NotifyPositionUpdateRequestChange(iStatus);
	SetActive();
	
	// Get the request and store it to be read later.
	TLbsPositionUpdateRequestBase req;
	iAgpsRequestChannel.GetPositionUpdateRequest(req);
	BufferRequestL(req);
	
	LogRequestUpdate(req);
	
	// Jump out of the AS loop
	if (iWait.IsStarted())
		{
		iWait.AsyncStop();
		}	
	}
	
void CAgpsChannel::DoCancel()
	{
	iAgpsRequestChannel.CancelNotifyPositionUpdateRequestChange();
	}

TInt CAgpsChannel::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CAgpsChannel::WaitForRequestL(TTimeIntervalSeconds /*aTimeout*/)
	{
	if (iRequestBuffer.Count() == 0)
		{
		// Start the active scheduler - this should be stopped
		// when RunL is called.
		iWait.Start();
		}
	}
	
TLbsPositionUpdateRequestBase::TType CAgpsChannel::GetNextRequestType()
	{
	if (iRequestBuffer.Count() == 0)
		{
		return TLbsPositionUpdateRequestBase::EUnknown;
		}
	else
		{
		return (iRequestBuffer[0]->Type());
		}
	}
	
void CAgpsChannel::GetNextRequestL(TLbsPositionUpdateRequestBase& aRequest)
	{
	if (iRequestBuffer.Count() == 0)
		{
		User::Leave(KErrNotFound);
		}
	
	if (aRequest.Type() != iRequestBuffer[0]->Type())
		{
		User::Leave(KErrArgument);
		}
	
	// request type should match what is in the buffer,
	// so copy the data then delete the message in the buffer.
	TLbsPositionUpdateRequestBase* req = iRequestBuffer[0];
	iRequestBuffer.Remove(0);
	aRequest = *req;
	delete req;	
	}

void CAgpsChannel::BufferRequestL(const TLbsPositionUpdateRequestBase& aRequest)
	{
	TLbsPositionUpdateRequestBase* req = new (ELeave) TLbsPositionUpdateRequestBase;
	CleanupStack::PushL(req);
	*req = aRequest;
	iRequestBuffer.AppendL(req);
	CleanupStack::Pop(req);
	}

TInt CAgpsChannel::SendGpsLocationUpdate(
		TInt aReason, 
		const TPositionInfoBase& aPosInfo,
		const TTime& aTargetTime)
	{
	INFO_PRINTF1(_L("<<<< AGPS Channel: Send GPS location update."));
	TTime now;
	now.UniversalTime();
	return iAgpsPositionUpdateBus.SetPositionInfo(
				aReason, 
				EFalse, 
				&aPosInfo, 
				aPosInfo.PositionClassSize(),
				aTargetTime,
				now);
	}

TInt CAgpsChannel::SendGpsMeasurementUpdate(
		TInt aReason,
		const TPositionGpsMeasurementInfo& aMeasurementInfo)
	{
	INFO_PRINTF1(_L("<<<< AGPS Channel: Send GPS measurement update."));
	TTime now;
	now.UniversalTime();
	return iAgpsMeasurementUpdateBus.SetGpsMeasurementInfo(
				aReason,
				&aMeasurementInfo,
				aMeasurementInfo.PositionClassSize(),
				now);
	}

void CAgpsChannel::LogRequestUpdate(TLbsPositionUpdateRequestBase& aRequest)
	{
	switch (aRequest.Type())
		{
		case TLbsPositionUpdateRequestBase::EStatus:
			{
			INFO_PRINTF1(_L(">>>> AGPS Channel: Received status update."));
			TLbsPositionUpdateRequestStatus& req = static_cast<TLbsPositionUpdateRequestStatus&>(aRequest);
			LogPowerAdviceMode(req.PowerAdvice());
			break;
			}
		case TLbsPositionUpdateRequestBase::ECancel:
			{
			INFO_PRINTF1(_L(">>>> AGPS Channel: Received cancel request."));
			TLbsPositionUpdateRequestCancel& req = static_cast<TLbsPositionUpdateRequestCancel&>(aRequest);
			LogPowerAdviceMode(req.PowerAdvice());
			break;
			}
		case TLbsPositionUpdateRequestBase::EUpdateRequest:
			{
			INFO_PRINTF1(_L(">>>> AGPS Channel: Received location request."));
			TLbsPositionUpdateRequest& req = static_cast<TLbsPositionUpdateRequest&>(aRequest);
			LogPowerAdviceMode(req.PowerAdvice());
			break;
			}
		default:
			{
			INFO_PRINTF2(_L(">>>> AGPS Channel: Received unknown request : %d"), aRequest.Type());
			break;
			}
		}
	}

void CAgpsChannel::LogPowerAdviceMode(TLbsPositionUpdateRequestBase::TPowerAdvice aPowerAdvice)
	{
	switch (aPowerAdvice)
		{
		case TLbsPositionUpdateRequestBase::ENoPowerAdvice:
			{
			INFO_PRINTF1(_L("-> ENoPowerAdvice"));
			break;
			}
		case TLbsPositionUpdateRequestBase::EPowerAdviceOn:
			{
			INFO_PRINTF1(_L("-> EPowerAdviceOn"));
			break;
			}
		case TLbsPositionUpdateRequestBase::EPowerAdviceStandby:
			{
			INFO_PRINTF1(_L("-> EPowerAdviceStandby"));
			break;
			}
		case TLbsPositionUpdateRequestBase::EPowerAdviceOff:
			{
			INFO_PRINTF1(_L("-> EPowerAdviceOff"));
			break;
			}
		case TLbsPositionUpdateRequestBase::EPowerAdviceClosed:
			{
			INFO_PRINTF1(_L("-> EPowerAdviceClosed"));
			break;
			}
		default:
			{
			INFO_PRINTF2(_L("-> Unrecognised power advice : %d"), aPowerAdvice);
			break;
			}
		}
	}

