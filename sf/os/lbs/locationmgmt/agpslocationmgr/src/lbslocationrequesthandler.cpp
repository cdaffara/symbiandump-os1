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
// locationrequesthandler.cpp
// 
//


#include "lbscommoninternaldatatypes.h"
#include "lbslocationrequesthandler.h"
#include "lbslocsourcegps.h"
#include "lbsdevloggermacros.h"
#include <lbs/lbslocerrors.h>

CLocationRequestHandler * CLocationRequestHandler::NewL
	(
	MLocationRequestHandlerObserver& aObserver, 
	const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel
	)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::NewL()\n");
	CLocationRequestHandler * self = new(ELeave) CLocationRequestHandler (aObserver, aChannel);
	CleanupStack::PushL(self);
	self->ConstructL(aChannel);
	CleanupStack::Pop();
	return self;
	}
	
CLocationRequestHandler::CLocationRequestHandler(MLocationRequestHandlerObserver& aObserver,	const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel) : 
CActive(CActive::EPriorityStandard),	
iObserver(aObserver),
iChannel(aChannel),
iRequestState(EIdle)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::CLocationRequestHandler()\n");
	}

void CLocationRequestHandler::ConstructL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::ConstructL()\n");
	CActiveScheduler::Add(this);
	iposUpdateReq.OpenL(aChannel);
	iTimer = CLbsCallbackTimer::NewL(*this);
	
	iposUpdateReq.NotifyPositionUpdateRequestChange(iStatus);
	SetActive();
	}

CLocationRequestHandler::~CLocationRequestHandler()
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::~CLocationRequestHandler()\n");
	Cancel();
	iposUpdateReq.Close();
	delete iTimer;
	}
	
void CLocationRequestHandler::ActivateRequest()
	{
	TLbsLocRequestQualityInt fred;
	
	LBSLOG(ELogP1, "CLocationRequestHandler::ActivateRequest()\n");
	// Invalidate if any
	InvalidateRequest();

	TTime timeNow;
	TTime endTime;
	timeNow.UniversalTime();
	iRequestActivationTime = timeNow;
	
	// TT=0 means immediate request
	endTime = iRequest.TargetTime() != 0 ? iRequest.TargetTime() : timeNow;
    endTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(endTime,iRequest.RequestQuality().MaxFixTime());
	
	if (endTime > timeNow)	// if it end in the future
		{
		iTimer->EventAtUTC(endTime, 0);
		iRequestState = EActive;
		}
	}
	
void CLocationRequestHandler::InvalidateRequest()
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::InvalidateRequest()\n");
	iRequestState = EIdle;
	iTimer->Cancel();
	}
	
TBool CLocationRequestHandler::IsRequestActive() const
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::IsRequestActive()\n");
	return	iRequestState == EActive || iRequestState == EActiveRestartOnTargetTime;
	}
	
TTime CLocationRequestHandler::GetStartTime() const
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::GetStartTime()\n");
	return (iRequest.TargetTime());
	};

TTime CLocationRequestHandler::GetTimeActivated() const
	{
	// Returns the time the request was actually activated
	// Needed when calculating max fix time for combined requests
	// when TargetTime is zero (i.e. immediate requests)
	LBSLOG(ELogP1, "CLocationRequestHandler::GetTimeActivated()\n");
	return iRequestActivationTime;
	};

TLbsLocRequestQualityInt CLocationRequestHandler::GetQuality() const
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::GetQuality()\n");

	return iRequest.RequestQuality();
	};

TBool CLocationRequestHandler::GetNewClient() const
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::GetNewClient()\n");
	return iRequest.NewClient();
	};

void CLocationRequestHandler::DoCancel()
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::DoCancel()\n");
	iposUpdateReq.CancelNotifyPositionUpdateRequestChange();
	iTimer->Cancel();
	}

//------------------------------------------------------------------------------------------------------------
void CLocationRequestHandler::RunL()
	{
	LBSLOG2(ELogP1, "CLocationRequestHandler::RunL() Begin. Status: %d\n", iStatus.Int());
	__ASSERT_DEBUG(iStatus.Int() == KErrNone, User::Panic(KLbsAGPSManFault, iStatus.Int()));
	
	iposUpdateReq.NotifyPositionUpdateRequestChange(iStatus);
	SetActive();
	
	ProcessAnyOutstandingRequestL();
	LBSLOG(ELogP1, "CLocationRequestHandler::RunL() End\n");
	}

/* When ran the first time (after AGPS Manager startup) the last known
   system status is passed to the NG */
void CLocationRequestHandler::ProcessAnyOutstandingRequestL()
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::ProcessAnyOutstandingRequestL() Begin\n");

	TLbsPositionUpdateRequestBase updateRequest; 
	User::LeaveIfError(iposUpdateReq.GetPositionUpdateRequest(updateRequest));
	
	TLbsPositionUpdateRequest* request = (reinterpret_cast<TLbsPositionUpdateRequest*>(&updateRequest));

	// Process the requst
	switch(updateRequest.Type())
		{
		case TLbsPositionUpdateRequestBase::ECancel:
			LBSLOG(ELogP2, "Location Cancel received from LS or NRH.\n");
			ProcessSystemStatusAdvice(updateRequest);
			
			if(!iLastRequestWasCancel)
				{
				InvalidateRequest();
				iObserver.OnLocationRequestStateChanged(iChannel, MLocationRequestHandlerObserver::EReasonCancel);
				iLastRequestWasCancel = ETrue;
				}
			break;
		
		case TLbsPositionUpdateRequestBase::EUpdateRequest:
			LBSLOG(ELogP2, "Location Request received from LS or NRH.\n");
			LBSLOG5(ELogP2, "   TargetTime:   %d:%d:%d.%d", request->TargetTime().DateTime().Hour(), 
															request->TargetTime().DateTime().Minute(),
															request->TargetTime().DateTime().Second(),
															request->TargetTime().DateTime().MicroSecond());
			LBSLOG2(ELogP2, "   Max Fix Time: %ld", request->RequestQuality().MaxFixTime().Int64());
			LBSLOG3(ELogP2, "   Accuracy H/V: %f/%f", request->RequestQuality().MinHorizontalAccuracy(),
													  request->RequestQuality().MinVerticalAccuracy());

			iLastRequestWasCancel = EFalse;
			ProcessSystemStatusAdvice(updateRequest);				
					
			iRequest = *request;
			ActivateRequest();
			if (iRequestState == EActive)
				{
				iObserver.OnLocationRequestStateChanged(iChannel, MLocationRequestHandlerObserver::EReasonRequest); // call back to MainLogic
				}
			break;

		case TLbsPositionUpdateRequestBase::EStatus:
			LBSLOG(ELogP2, "Status received from LS or NRH.\n");
			iLastRequestWasCancel = EFalse;
			ProcessSystemStatusAdvice(updateRequest);
			break;
			
		default:
			{
			LBSLOG_WARN2(ELogP2, "Unknown update request type: %d \n", updateRequest.Type());
			iLastRequestWasCancel = EFalse;
			break;
			}
		}
	LBSLOG(ELogP1, "CLocationRequestHandler::ProcessAnyOutstandingRequestL() End\n");
	}

/** */
void CLocationRequestHandler::ProcessSystemStatusAdvice(TLbsPositionUpdateRequestBase& aRequest)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::ProcessSystemStatusAdvice()\n");
	TLbsPositionUpdateRequestStatus* status;
	status = reinterpret_cast<TLbsPositionUpdateRequestStatus*>(&aRequest);
	ProcessPowerModeAdvice(status->PowerAdvice());
	iObserver.OnSystemStatusAdvice(iChannel, status->Tracking());
	}

void CLocationRequestHandler::ProcessPowerModeAdvice(TLbsPositionUpdateRequestBase::TPowerAdvice aMode)
 	{
 	LBSLOG(ELogP1, "CLocationRequestHandler::ProcessPowerModeAdvice()\n");
 	if (TLbsPositionUpdateRequestBase::ENoPowerAdvice != aMode)
 		{
 		CLbsLocationSourceGpsBase::TPowerMode powerMode;
 		switch (aMode)
 			{
 			//Identify the power advice being sent
 			case TLbsPositionUpdateRequestBase::EPowerAdviceOn:
 				{
 				powerMode = CLbsLocationSourceGpsBase::EPowerModeOn;
 				break;
 				}
 			case TLbsPositionUpdateRequestBase::EPowerAdviceStandby:
 				{
 				powerMode = CLbsLocationSourceGpsBase::EPowerModeStandby;
 				break;
 				}
			case TLbsPositionUpdateRequestBase::EPowerAdviceOff:
 				{
 				powerMode = CLbsLocationSourceGpsBase::EPowerModeOff;
 				break;
 				}
 			case TLbsPositionUpdateRequestBase::EPowerAdviceClosed:
 				{
 				powerMode = CLbsLocationSourceGpsBase::EPowerModeClose;
 				break;
 				}
 			default:
 				{
 				LBSLOG_WARN2(ELogP2, "Unknown power mode: %d \n", aMode);
 				break;
 				}
 			}
 		
 		iObserver.OnPowerModeAdvice(iChannel, powerMode);
 		}
 	}
 	
TBool CLocationRequestHandler::IsAccuracyRequirementMet(TReal32 aHorizontalAccuracy, TReal32 /*aVerticalAccuracy*/)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::ProcessLocationUpdate()\n");
	TBool rc = EFalse;
	if (EIdle != iRequestState)
		{
		TLbsLocRequestQualityInt quality = GetQuality();

		TTimeIntervalMicroSeconds mft = quality.MaxFixTime();

		TTime targ = iRequest.TargetTime();
		if (targ == TTime(0))
			{
			targ.UniversalTime();
			}
		TTime endtarg = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(targ,mft);
		
		TTime now;
		now.UniversalTime();
		
		if ((now >= targ) && (now <= endtarg) &&
			(aHorizontalAccuracy <= quality.MinHorizontalAccuracy()))
			{
			LBSLOG(ELogP2, "Time window ok. Quality meet. Invalidating request.\n");
			InvalidateRequest();
			rc = ETrue;
			}
		}
	
	return rc;
	}

void CLocationRequestHandler::CompleteSelf(TInt aReason)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::CompleteSelf()\n");
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, aReason);
	SetActive();
	}


void CLocationRequestHandler::OnTimerEventL(TInt /*aTimerId*/)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::OnTimerEventL()\n");
	
	//Need to return to EActive state
	if(iRequestState == EActiveRestartOnTargetTime)
		{
		LBSLOG(ELogP2, "Start time occured for the LS request. Reactivate request.\n");
		InvalidateRequest();
		
		TTime timeNow;
		timeNow.UniversalTime();
		
		// TT=0 means immediate request
		TTime endTime = iRequest.TargetTime() != 0 ? iRequest.TargetTime() : iRequestActivationTime;
		endTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(endTime,iRequest.RequestQuality().MaxFixTime());
			
		if (endTime > timeNow)	// if it end in the future
			{
			iTimer->EventAtUTC(endTime, 0);
			iRequestState = EActive;
			iObserver.OnLocationRequestStateChanged(iChannel, MLocationRequestHandlerObserver::EReasonRequest);
			}
		else
			{
			LBSLOG(ELogP2, "LS or NRH request timed out.\n");
			iObserver.OnLocationRequestStateChanged(iChannel, MLocationRequestHandlerObserver::EReasonTimeout);
			}
		}
	else
		{
		LBSLOG(ELogP2, "LS or NRH request timed out.\n");
		InvalidateRequest();
		iObserver.OnLocationRequestStateChanged(iChannel, MLocationRequestHandlerObserver::EReasonTimeout);
		}
	}
	
TInt CLocationRequestHandler::RunError(TInt aError)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::RunError() Begin\n");
	// handle RunL leaving - in our case we do nothing 
	// as this means we have had encountered a program inconsistency
	// By doing nothing here  causes the main manager thread to exit
	// and the Lbs root process resolves any issues - by stopping and restarting
	// lbs components
	LBSLOG_ERR2(ELogP2, " %d \n", aError);
	
	LBSLOG(ELogP1, "CLocationRequestHandler::RunError() End\n");
 	return aError;
	}

TInt CLocationRequestHandler::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::OnTimerError() Begin\n");
	// handle the same way as CLocationRequestHandler::RunError
	LBSLOG_ERR2(ELogP2, " %d \n", aError);
	LBSLOG(ELogP1, "CLocationRequestHandler::OnTimerError() End\n");
 	return aError;
	}

TBool CLocationRequestHandler::GpsTimingOfCellFramesRequested() const
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::GpsTimingOfCellFramesRequested()\n");
	return iRequest.RequestMethod().GpsTimingOfCellFramesRequested();
	}


TPositionModuleInfo::TTechnologyType CLocationRequestHandler::GetTechnologyTypeFromRequest(TInt aIndex) const
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::GetTechnologyTypeFromRequest() Begin\n");
	TLbsNetPosMethodInt posMethod;
	TInt result = iRequest.RequestMethod().GetPosMethod(aIndex, posMethod);
	if (result == KErrNone)
		{
		return posMethod.PosMode();
		}
	LBSLOG(ELogP1, "CLocationRequestHandler::GetTechnologyTypeFromRequest() End\n");
	return TPositionModuleInfo::ETechnologyUnknown;
	}

	


void CLocationRequestHandler::RestartOnTargetTime()
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::RestartOnTargetTime() Begin\n");
	if(iRequestState == EIdle)
		{
		__ASSERT_DEBUG(0, User::Invariant());
		LBSLOG(ELogP1, "CLocationRequestHandler::RestartOnTargetTime() End\n");
		return;
		}
	
	InvalidateRequest();
	
	TTime timeNow;
	timeNow.UniversalTime();
		
	TTime startTime = iRequest.TargetTime()==0 ? iRequestActivationTime : iRequest.TargetTime();
	//Even though it starts in the past, it is ok - it will be restarted immediately.
	iTimer->EventAtUTC(startTime, 0);  
	iRequestState = EActiveRestartOnTargetTime;
	
	LBSLOG(ELogP1, "CLocationRequestHandler::RestartOnTargetTime() End\n");
	}


void CLocationRequestHandler::CancelRestartOnTargetTime()
	{
	LBSLOG(ELogP1, "CLocationRequestHandler::CancelRestartOnTargetTime() Begin\n");
	if(iRequestState == EActiveRestartOnTargetTime)
		{
		InvalidateRequest();
			
		TTime timeNow;
		timeNow.UniversalTime();
			
		// TT=0 means immediate request
		TTime endTime = iRequest.TargetTime() != 0 ? iRequest.TargetTime() : iRequestActivationTime;
		endTime = LbsTimerUtils::AddUpInt64TimersWithOverflowCheck(endTime,iRequest.RequestQuality().MaxFixTime());
			
		if(endTime > timeNow)	// if it end in the future
			{
			iTimer->EventAtUTC(endTime, 0);
			iRequestState = EActive;
			}
		else
			{
			LBSLOG(ELogP2, "LS or NRH request timed out.\n");
			iObserver.OnLocationRequestStateChanged(iChannel, MLocationRequestHandlerObserver::EReasonTimeout);
			}
		}
		LBSLOG(ELogP1, "CLocationRequestHandler::CancelRestartOnTargetTime() End\n");
	}
