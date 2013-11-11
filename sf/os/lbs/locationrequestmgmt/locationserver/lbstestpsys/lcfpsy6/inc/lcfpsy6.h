/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef _LCFPSY6_H_
#define _LCFPSY6_H_

#include <lbs/epos_cpositioner.h>

#include "lcfpsy6publics.h"

#include "delaytimer.h"

class CLcfPsy6: public CPositioner, public MDelayTimerUser
{
    public:
	    static CLcfPsy6* NewL(TAny* aConstructionParameters);
	    ~CLcfPsy6();

    protected:
	    void ConstructL(TAny* aConstructionParameters);

    public: // from CPositioner

	    void NotifyPositionUpdate(TPositionInfoBase& aPosInfo, 
							      TRequestStatus& aStatus);

	    void CancelNotifyPositionUpdate();

        TBool TrackingOverridden() const;
        void StartTrackingL(const TTimeIntervalMicroSeconds& aInterval);
        void StopTracking();


    protected:

        struct TRequest             
        {                           
            TRequestStatus*         iRequestStatus;
            TPositionInfoBase*      iPositionInfoBase;
            TBool                   iPostponed;

            TBool                   iDelayed;
            CDelayTimer*            iDelayTimer;
        };                          

	    void                        ProcessRequest();

        /** Logs new Event, Update request, or Cancel */
        void                        NewEvent(EEventType aEventType);

        /** Stores request status of a request by QueryId as find key */
        void                        NewRequest(TPositionInfoBase* aPosInfo, 
                                               TRequestStatus* aRequestStatus);

        /** Completes request and removes it from list */
        void                        CompleteRequest(TInt aCompletionCode);

	    void                        GetBasicPositionInfo(TPositionInfoBase& aPosInfo);
	    void                        GetMandatoryPositionInfo(TPositionInfoBase& aPosInfo);

    private: // callbacks
	    void                        TimerCompleted(TInt aTimerId);

    private: // member variables

        RArray<TQueryEvent>         iEvents;
        TRequest                    iRequest;
        TUid                        iUid;
        TBool                       iLeaveOnStartTracking;
};
#endif

// End of file
