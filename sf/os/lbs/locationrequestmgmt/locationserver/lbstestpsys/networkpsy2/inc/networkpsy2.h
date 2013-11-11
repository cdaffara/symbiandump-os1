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



#ifndef NetworkPsy2_H
#define NetworkPsy2_H

#include <lbs/epos_cpositioner.h>

#include <lbs/test/intgpspsy1publics.h>
#include <lbs/test/psyconfig.h>

class CNetworkPsy2: public CPositioner
{
    public:
	    static CNetworkPsy2* NewL(TAny* aConstructionParameters);
	    ~CNetworkPsy2();

    protected:
	    void ConstructL(TAny* aConstructionParameters);

    public: // from CPositioner

	    void NotifyPositionUpdate(TPositionInfoBase& aPosInfo, 
							      TRequestStatus& aStatus);

	    void CancelNotifyPositionUpdate();

        TBool TrackingOverridden() const;
        void StartTrackingL(const TTimeIntervalMicroSeconds& aInterval);
        void StopTracking();


    private:

	    /**
	    * Config the PSY
	    */
	    void ConfigPsyL(const HPositionGenericInfo& aGenInfo);

        /**
        * Start timer if needed
        */
        void StartTimerIfNeeded();
        
        /**
        * Complete request with default positioninfo
        */
        void CompleteRequestByDefault();
        
        /**
        * Completes request 
        */
        void CompleteRequest(TInt aCompletionCode);

        //Static timer callback
        static TInt TimerCallback(TAny* aAny);
        
        //Timer callback
	    void TimerCompleted();

    private: // member variables
        //Request status
        TRequestStatus*         iRequestStatus;
        
        //Position Info base
        TPositionInfoBase*      iPositionInfoBase;
        
        //Timer
        CPeriodic* iTimer;
        
        //Config array
        RArray<TPsyConfig> iPsyConfigArray;
        
        //current index in config array
        TInt iCurrentIndex;
        
        //Uid
        TUid iUid;
};
#endif

// End of file
