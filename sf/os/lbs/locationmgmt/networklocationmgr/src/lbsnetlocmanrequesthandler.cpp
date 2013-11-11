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
// Includes
// Component
// 
//

#include "lbsnetlocmanrequesthandler.h"
#include "lbsnetlocmanlogic.h"
#include "lbsdevloggermacros.h"


//****************************************************************
// Class CNetLocManRequestHandler
//****************************************************************
/** Static constructor.
@param aChannel A reference to the channel identifier
@param aNetLocMan A reference to the parent class
@released
@internalComponent
*/
CNetLocManRequestHandler* CNetLocManRequestHandler::NewL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, CNetLocManLogic& aNetLocMan)
	{
	CNetLocManRequestHandler* self = new(ELeave) CNetLocManRequestHandler (aChannel, aNetLocMan);

	CleanupStack::PushL(self);

	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}
	

/** Constructor.
@param aChannel A reference to the channel identifier
@param aNetLocMan A reference to the parent class
@released
@internalComponent
*/
CNetLocManRequestHandler::CNetLocManRequestHandler(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, CNetLocManLogic& aNetLocMan) : 
CActive(CActive::EPriorityStandard), iChannel(aChannel), iNetLocMan(aNetLocMan), iTimer(NULL)
	{
	}


/** 
@see CActive::DoCancel
@released
@internalComponent
*/
void CNetLocManRequestHandler::DoCancel()
	{
	// cancel RLbsPositionUpdateRequests async request
	iPosUpdateReq.CancelNotifyPositionUpdateRequestChange();
	}


/** Second phase constructor.
@released
@internalComponent
*/
void CNetLocManRequestHandler::ConstructL()
	{
	iPosUpdateReq.OpenL(iChannel);
	iLocationReq.OpenL(KLbsNetLocManagerUid);
	iTimer = CLbsCallbackTimer::NewL(*this);
	CActiveScheduler::Add(this);
	}


/** Destructor
@released
@internalComponent
*/
CNetLocManRequestHandler::~CNetLocManRequestHandler()
	{
	Cancel();
	iPosUpdateReq.Close();
	iLocationReq.Close();
	if(iTimer)
		{
		iTimer->Cancel();
		delete iTimer;	
		}
	}
 

/** Subscribe to the position update request property
@released
@internalComponent
*/
void CNetLocManRequestHandler::StartGettingRequests()
	{
	LBSLOG(ELogP2, "CNetLocManRequestHandler::StartGettingRequests:");

	Cancel();
	// subscribe to property from Network Location Manager -> Network Gateway
	iPosUpdateReq.NotifyPositionUpdateRequestChange(iStatus);
	SetActive();	
	}


/** 
@see CActive::RunError
@param aError An error code
@released
@internalComponent
*/
TInt CNetLocManRequestHandler::RunError(TInt aError)
	{
	// handle RunL leaving - in our case we do nothing 
	// as this means we have had encountered a program inconsistency
	// By doing nothing here  causes the main manager thread to exit
	// and the Lbs root process resolves any issues - by stopping and restarting
	// lbs components
 	return aError;
	}


/** Handle requests from the LocServer
This function handles the incomming position requests from the LocServer,
and pass them on to the NetworkGateway
@see CActive::RunL
@released
@internalComponent
*/
void CNetLocManRequestHandler::RunL()
	{
	TInt status = iStatus.Int();

	StartGettingRequests(); // we want further location requests!
	
	switch (status)
		{
		case KErrNone:
			{
			TLbsPositionUpdateRequestBase updateRequest; 
			User::LeaveIfError(iPosUpdateReq.GetPositionUpdateRequest(updateRequest));
			
			// when there is a new request forwarded, increase session ID by one
			TLbsNetSessionIdInt sessionId;
			iNetLocMan.GetSessionId(sessionId);
			
			switch(updateRequest.Type())
				{
				case TLbsPositionUpdateRequestBase::ECancel:
					{
					iTimer->Cancel();
					TLbsNetLocNetworkLocationCancelMsg locCancel(sessionId, 0);
					User::LeaveIfError(iLocationReq.SetNetworkLocationRequest(locCancel));
					break;
					}
		
				case TLbsPositionUpdateRequestBase::EUpdateRequest:
					{
					// only increment when new update request comes in
					iNetLocMan.IncrementSessionId();
					TLbsPositionUpdateRequest* request = 
						(reinterpret_cast<TLbsPositionUpdateRequest*>(&updateRequest));
					
					TTime target = request->TargetTime();
					TTime now;
					now.UniversalTime();
					
					TTimeIntervalMicroSeconds timeout;
					timeout = target.MicroSecondsFrom(now);
					
					if (timeout < 0)
						timeout = 0;
					
					iTimer->Cancel();
					iTimer->EventAfter(timeout ,0);
					break;
					}
				}
			break;
			}
		default:
			{
			User::LeaveIfError(status);
			break;
			}
		}
	}
	

void CNetLocManRequestHandler::OnTimerEventL(TInt /*aTimerId*/)
	{
	TLbsPositionUpdateRequestBase updateRequest; 
	User::LeaveIfError(iPosUpdateReq.GetPositionUpdateRequest(updateRequest));

	TLbsNetSessionIdInt sessionId;
	iNetLocMan.GetSessionId(sessionId);
	
	TLbsPositionUpdateRequest* request = 
		(reinterpret_cast<TLbsPositionUpdateRequest*>(&updateRequest));
	
	TLbsNetPosRequestQualityInt quality; 
	quality.SetMaxFixTime(request->RequestQuality().MaxFixTime());
	
	quality.SetMinHorizontalAccuracy(request->RequestQuality().MinHorizontalAccuracy());
	quality.SetMinVerticalAccuracy(request->RequestQuality().MinVerticalAccuracy());										

	// Set device status to EDeviceActive
	iNetLocMan.UpdateDeviceStatus(TPositionModuleStatus::EDeviceActive);
	
	TLbsNetLocNetworkLocationRequestMsg locRequest(sessionId, ETrue, quality);
	User::LeaveIfError(iLocationReq.SetNetworkLocationRequest(locRequest));
	}
	
	
TInt CNetLocManRequestHandler::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	return aError;
	}


