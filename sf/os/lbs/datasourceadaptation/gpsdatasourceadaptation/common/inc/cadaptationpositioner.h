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

#ifndef ADAPTATIONPOSITIONER_H
#define ADAPTATIONPOSITIONER_H

#include <lbs/epos_cpositioner.h>
#include "cpositionerenvironment.h"
#include "mpositioner.h"
#include "lbstimer.h" // for MLbsCallbackTimerObserver & CLbsCallbackTimer

class CRequestHandler;
class TRequestParameters;

class CAdaptationPositioner : public CPositioner,
							  public MLbsCallbackTimerObserver,
                       		  public MPositioner
    {
    public:
        virtual ~CAdaptationPositioner();

    public:
        // From CPositioner.
        virtual void NotifyPositionUpdate(TPositionInfoBase& aPosInfo, TRequestStatus& aStatus) = 0;
        virtual void CancelNotifyPositionUpdate();
        virtual void CancelNotifyPositionUpdate(TInt aError);
        virtual TBool TrackingOverridden() const;
        virtual void StartTrackingL(const TTimeIntervalMicroSeconds& aInterval);
        virtual void StopTracking();

        // From MPositioner
        virtual void RequestCompleteNotify(const TPositionInfo& aPositionInfo, TInt aError, TTime& aActualTime);
        virtual void ReportStatus(const TPositionModuleStatus& aStatus);
		virtual TBool IsActive();
		virtual void UpdateFailed(TInt aError);		
		virtual TBool IsWarmingDown();
		TUint InactivityTimeout();
		
    	// from MLbsCallbackTimerObserver
		void OnTimerEventL(TInt aTimerId);
		TInt OnTimerError(TInt aError, TInt aTimerId);

		enum TEventId // timer event ID at request queue level
			{
			EMaxFixTimerEvent = 0,
			EWarmDownTimerEvent
			};

    private:
		void UpdateTrackingTime(TTime& aTimeNow);
		void SetTrackingTime();
        void MaxFixTimerEvent();
        TInt MaxFixTimerError(TInt /*aError*/);
        void WarmDownTimerEvent();
        TInt WarmDownTimerError(TInt /*aError*/);
        TBool IsAccurate(const TPositionInfo& aPositionInfo);
    protected:
        CAdaptationPositioner();
        
        virtual void SetModuleId(TPositionInfoBase* aPosInfo) = 0;
        
		void SetupTrackingRequestL();
		TBool UseLastLocation(TPositionInfoBase& aPosInfo);
        void StartMaxFixTimerL();
        void StopMaxFixTimer();
        void StartWarmDownTimerL();
        void StopWarmDownTimer();
 		TBool IsTracking();
        void ClearPositionInfo(TPositionInfoBase& aPosInfo);
        TBool CompleteRequest(const TPositionInfo& aPositionInfo, TInt aError);
       
	protected:
		enum TTrackingState
			{
			ERequestStatePreTracking	= 0x01,
			ERequestStateTracking		= 0x02
			};
			
		enum TRequestState
			{
			ERequestStateInitial		= 0x01,
			ERequestStatePostInitial	= 0x02
			};

        TPositionInfoBase*			iPosInfo;
        TRequestStatus*				iClientStatus;
		CRequestHandler*			iRequestHandler;
		CPositionerEnvironment*		iEnvironment;
		
		TBool						iActive;
		TTimeIntervalMicroSeconds	iTrackingInterval;
		TTime 						iTargetTime;
		TTimeIntervalMicroSeconds	iTimeOutInterval;
		TPositionCriteria			iCriteria;
		TRequestState 				iState;
		TUint						iInactivityTimeout;
		
    private:
    	CLbsCallbackTimer* iMaxFixTimer;
    	CLbsCallbackTimer* iWarmDownTimer;
    };

#endif

// End of File
