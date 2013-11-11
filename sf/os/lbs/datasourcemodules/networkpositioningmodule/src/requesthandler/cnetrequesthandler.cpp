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
 @InternalComponent
*/

#include "cnetrequesthandler.h"
#include "cpositionerq.h"
#include "lbsdevloggermacros.h"
#include "trequestparameters.h"
	
/*
* CRequestHandler::NewL
* Two-phased constructor.
*/
CNETRequestHandler* CNETRequestHandler::NewL(CPositionerQ* aPositionerQ)
    {
    CNETRequestHandler* self = new( ELeave ) CNETRequestHandler(aPositionerQ);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
* CNETRequestHandler::~CNETRequestHandler
* Destructor.
*/
CNETRequestHandler::~CNETRequestHandler()
    {
	delete iNETRequestBus;
    }

/**
@param aPositionerQ 	the positioner Q
*/
CNETRequestHandler::CNETRequestHandler(CPositionerQ* aPositionerQ) 
	{
	iPositionerQ = aPositionerQ;
	}

/**
construct the request bus by channel identifier
*/
void CNETRequestHandler::ConstructL()
	{
	CRequestHandler::ConstructL();
	
	iChannel = KChannelArray[KNetworkManagerToLocServerChannelIndex];	// PSY<->NetLocManager channel

	iNETRequestBus = CNETRequestBus::NewL(iChannel);
	}

/**
Merge the request with the local iMergedRequest

@param aTime The target time of the request
@param aTimeForFix The maximum fix time
@param aQuality The requested position info quality
@see CPositionerSubSession
*/
TInt CNETRequestHandler::MergeRequest(const TRequestParameters* aRequestParameters, const TBool aTracking, 
							TLbsPositionUpdateRequestBase::TPowerAdvice aPower)
	{
	TPositionQuality posQuality; 
	TLbsPositionUpdateRequest newReq;
	TLbsLocRequestQuality newRequestQuality;

	aRequestParameters->iCriteria.GetRequiredQuality(posQuality);
	
	BuildUpdateRequest(newRequestQuality, aRequestParameters->iTimeForFix, posQuality);
	newReq.TargetTime() 	= aRequestParameters->iTargetTime;
	
	newReq.RequestQuality().SetMaxFixTime(newRequestQuality.MaxFixTime());
	newReq.RequestQuality().SetMinHorizontalAccuracy(newRequestQuality.MinHorizontalAccuracy());
	newReq.RequestQuality().SetMinVerticalAccuracy(newRequestQuality.MinVerticalAccuracy());

	newReq.NewClient() 		= aRequestParameters->iNewClient;
	newReq.Tracking() 		= aTracking;
	newReq.SetPowerAdvice(aPower);

	if(newReq.TargetTime() < iMergedRequest.TargetTime())
		{
		iMergedRequest.TargetTime() = newReq.TargetTime();
		}
		
	if(newReq.RequestQuality().MinHorizontalAccuracy() < iMergedRequest.RequestQuality().MinHorizontalAccuracy())
		{
		iMergedRequest.RequestQuality().SetMinHorizontalAccuracy(newReq.RequestQuality().MinHorizontalAccuracy());
		}
		
	if(newReq.RequestQuality().MinVerticalAccuracy() < iMergedRequest.RequestQuality().MinVerticalAccuracy())
		{
		iMergedRequest.RequestQuality().SetMinVerticalAccuracy(newReq.RequestQuality().MinVerticalAccuracy());
		}

	if(newReq.RequestQuality().MaxFixTime() < iMergedRequest.RequestQuality().MaxFixTime())
		{
		iMergedRequest.RequestQuality().SetMaxFixTime(newReq.RequestQuality().MaxFixTime());
		}
		
	iMergedRequest.NewClient() = iMergedRequest.NewClient() || newReq.NewClient();
	iMergedRequest.Tracking()  = iMergedRequest.Tracking()  || newReq.Tracking();
 
 	TLbsPositionUpdateRequest& reqNC = const_cast<TLbsPositionUpdateRequest&>(newReq);
 	iMergedRequest.SetPowerAdvice(reqNC.PowerAdvice());
 	
 	return KErrNone;
	}

/** Used before merging */
void CNETRequestHandler::ResetMergedRequest()
	{
	iMergedRequest.NewClient()  = EFalse;
	iMergedRequest.Tracking()   = EFalse;
	iMergedRequest.TargetTime() = KMaxTInt64;
	iMergedRequest.RequestQuality().SetMinHorizontalAccuracy(KMaxTReal32);
	iMergedRequest.RequestQuality().SetMinVerticalAccuracy(KMaxTReal32);
	iMergedRequest.RequestQuality().SetMaxFixTime(KMaxTInt64);
	}	

/**
Publish the request to the request bus
	
@param aRequest The request to be updated
@param aTimeForFix The maximum fix time
@param aQuality The requested position info quality
*/
void CNETRequestHandler::IssueMergedRequestL()
	{
	iCurrentRequest = iMergedRequest;
	TLbsPositionUpdateRequest request = iCurrentRequest;
	
	// If target time is 'now' then force the request target time to be 0
	TTime currentTime;
	currentTime.UniversalTime();
	
	if (!request.Tracking())	// Not tracking
		{
		if (request.TargetTime() <= currentTime)
			{
			request.TargetTime() = TTime(0);
			}
		}
	else if (request.NewClient())	// Tracking!
		{
		request.TargetTime() = TTime(0);
		}

	iNETRequestBus->PublishRequestL(request);
	}
	
/**
Publish the Cancel request to the request bus.
Cancel always disables tracking (the tracking flag is EFalse by default)
*/
void CNETRequestHandler::IssueCancel(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower)
	{
	TLbsPositionUpdateRequestCancel cancel;

	// note, here we choose to send EPowerAdviceStandby straight away
	cancel.SetPowerAdvice(aPower);
	cancel.Tracking() = aTracking;
	
	TRAP_IGNORE(iNETRequestBus->PublishRequestL(cancel));
	}
	
/**
Publish statutory information (e.g tracking) to the request bus.
*/
void CNETRequestHandler::IssueStatus(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower)
   	{
   	TLbsPositionUpdateRequestStatus status;
   	status.Tracking() = aTracking;
 	status.SetPowerAdvice(aPower);
   	TRAP_IGNORE(iNETRequestBus->PublishRequestL(status));
   	}
