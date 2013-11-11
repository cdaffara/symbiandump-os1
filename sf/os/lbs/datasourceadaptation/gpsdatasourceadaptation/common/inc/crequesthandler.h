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

#ifndef __LBSREQUESTHANDLER_H__
#define __LBSREQUESTHANDLER_H__

#include <e32base.h>
#include <lbssatellite.h>
#include <lbsposition.h>
#include <lbscriteria.h>
#include "lbstimer.h" // for MLbsCallbackTimerObserver & CLbsCallbackTimer
#include "LbsInternalInterface.h"

class CPositionerQ;
class TRequestParameters;
class MPositioner;

class CRequestHandler : public CBase, public MLbsCallbackTimerObserver

	{
	public:		
		virtual ~CRequestHandler();
		virtual void ReMergeWithCancelRequestL(TInt aError);
        virtual void CancelRequestL();
	    virtual void SubmitNewRequestL(MPositioner* aNewRequest);
	    virtual void WarmDownTimerExpired();

    	void NotifyRequestComplete();
    	
    	// from MLbsCallbackTimerObserver
		void OnTimerEventL(TInt aTimerId);
		TInt OnTimerError(TInt aError, TInt aTimerId);

		enum TEventId // timer event ID at request queue level
			{
			EPowerStandByTimerEvent = 0
			};
			
		enum TRequestHandlerPanic
			{
			KUnknownTimerEventId
			};

	protected:
		CRequestHandler();
		void ConstructL();

		virtual void IssueStatus(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::ENoPowerAdvice) = 0;
		virtual void IssueCancel(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::EPowerAdviceStandby) = 0;
		virtual void IssueMergedRequestL() = 0;
		void BuildUpdateRequest(TLbsLocRequestQuality& aRequest, const TTimeIntervalMicroSeconds& aTimeForFix, const TPositionQuality& aQuality);
		virtual TInt MergeRequest(const TRequestParameters* aRequestParameters,
							const TBool aTracking,
							TLbsPositionUpdateRequestBase::TPowerAdvice aPower = TLbsPositionUpdateRequestBase::ENoPowerAdvice) = 0;
		
		virtual void ResetMergedRequest() = 0;
	
	protected:	
		TLbsPositionUpdateRequest iMergedRequest;					// next potential position update request
		TLbsPositionUpdateRequest iCurrentRequest;					// current update request that's on the bus
		CPositionerQ* iPositionerQ;
		
	private:
		void SubmitRequestL();
		TBool MergedDifferent();

		void MergeAllRequests();
		TInt MergeNewRequest(TInt aNewRequest);
		void TimeOutTimerEvent();
		TInt TimeOutTimerError(TInt /*aError*/);

        TUint iTrackingCount;
		CLbsCallbackTimer* iPowerStandByTimer; // sends a standby if no requests in the queue
	};
	
#endif // __LBSREQUESTHANDLER_H__
