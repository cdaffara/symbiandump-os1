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
// locationrequesthandler.h
// Definition of location request handler sub-component of agps manager
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef LBSLOCREQHANDLER_H
#define LBSLOCREQHANDLER_H

#include <e32base.h>

#include "LbsInternalInterface.h"
#include "lbstimer.h"

#include <lbs/lbslocdatasourcegpsbase.h>

class MLocationRequestHandlerObserver
	{
public:

	enum TRequestStateChangeReason
		{
		EReasonNone,
		EReasonRequest,
		EReasonCancel,
		EReasonTimeout
		};

	virtual void OnLocationRequestStateChanged(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel,const TRequestStateChangeReason& aReason) = 0;
	virtual void OnSystemStatusAdvice(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, TBool aTracking) = 0;
	virtual void OnPowerModeAdvice(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel, CLbsLocationSourceGpsBase::TPowerMode& aPowerMode) = 0;

	};
	
	
class CLocationRequestHandler :	public CActive, private MLbsCallbackTimerObserver
	{
public:
	
	enum TRequestState
		{
		EIdle,
		EActive,
		EActiveRestartOnTargetTime
		};
		
	static CLocationRequestHandler* NewL(MLocationRequestHandlerObserver& aObserver, const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel);
	~CLocationRequestHandler();

	TBool IsAccuracyRequirementMet(TReal32 aHorizontalAccuracy, TReal32 aVerticalAccuracy);

	void RestartOnTargetTime();
	TBool IsRequestActive() const;
	void InvalidateRequest();
	
	//void RestartOnTargetTime();
	void CancelRestartOnTargetTime();
	
	TTime GetStartTime() const ;
	TTime GetTimeActivated() const;
	TLbsLocRequestQualityInt GetQuality() const ;
	TBool GetNewClient() const ;
	TPositionModuleInfo::TTechnologyType GetTechnologyTypeFromRequest(TInt aIndex) const;
	TBool GpsTimingOfCellFramesRequested() const;

private:
	CLocationRequestHandler(MLocationRequestHandlerObserver& aObserver,	const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel); 
	void ConstructL(const RLbsPositionUpdateRequests::TChannelIdentifer& aChannel);

	// From CActive
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
	// From MLbsCallbackTimerObserver
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

	// Helpers
	void ActivateRequest();
	void CompleteSelf(TInt aReason);
	void ProcessAnyOutstandingRequestL();
	void ProcessSystemStatusAdvice(TLbsPositionUpdateRequestBase& aRequest);
	void ProcessPowerModeAdvice(TLbsPositionUpdateRequestBase::TPowerAdvice aMode);

private:
	MLocationRequestHandlerObserver& iObserver;
	RLbsPositionUpdateRequests iposUpdateReq;
	RLbsPositionUpdateRequests::TChannelIdentifer iChannel;
	TLbsPositionUpdateRequest iRequest;
	TRequestState iRequestState;
	
	CLbsCallbackTimer*	iTimer;
	
	// Stores the actual start time for immediate reqs
	// Needed when calculating max fix time for a combined request
	TTime	iRequestActivationTime;

	TBool iLastRequestWasCancel;
	};

#endif // LBSLOCREQHANDLER_H

