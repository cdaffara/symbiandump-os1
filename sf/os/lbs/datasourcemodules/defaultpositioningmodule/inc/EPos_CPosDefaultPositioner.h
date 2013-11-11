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



#ifndef CPOSDEFAULTPOSITIONER_H
#define CPOSDEFAULTPOSITIONER_H

//  INCLUDES
#include <lbs/epos_mposmodulesobserver.h>
#include "EPos_CProxyPositioner.h"

// FORWARD DECLARATIONS
class CPosModules;
class CPosRequestController;

// CLASS DECLARATION

/**
* The CPosDefaultPositioner is a proxy positioner that delegates all
* incoming requests to the positioner in the modules list with highest priority.
* This is done by using a CPosRequestController object which handles the 
* position request functionality.
*/
class CPosDefaultPositioner : public CProxyPositioner
    {
    public: // Constructors and destructor
              
        /**
        * Two-phased constructor.
        *
        * @param aConstructionParameters Construction parameters.
        */
        static CPosDefaultPositioner* NewL(
            TAny* aConstructionParameters
        );

        /**
        * Destructor.
        */
        ~CPosDefaultPositioner();

    public: // Functions from base classes

        /**
        * From CPositioner.
        * Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        *
        */
        void NotifyPositionUpdate(
            TPositionInfoBase& aPosInfo,
            TRequestStatus& aStatus
        );

        /**
        * From CPositioner.
        * Cancels position info request.
        */
        void CancelNotifyPositionUpdate();

        /**
        * From CPositioner.
        * Cancels position info request with error code
        */        
        void CancelNotifyPositionUpdate(TInt aError);  
        
        /**
        * From CPositioner.
        * Indicates that the PSY has overridden tracking.
        *
        * @return ETrue.
        */
        TBool TrackingOverridden() const;

        /**
        * From CPositioner.
        * Initiate a tracking session.
        *
        * @param aInterval Interval for position requests.
        */
        void StartTrackingL(
            const TTimeIntervalMicroSeconds& aInterval
        );

        /**
        * From CPositioner.
        * Stop a periodic update session.
        */
        void StopTracking();

        /**
        * Returns current interval for an tracking session.
        *
        * @return Tracking interval in microseconds. 0 if no tracking session.
        */
        TTimeIntervalMicroSeconds TrackingInterval() const;
        
    private:

        /**
        * C++ default constructor.
        */
        CPosDefaultPositioner();

        /**
        * EPOC constructor.
        */
        void ConstructL(TAny* aConstructionParameters);

        // By default, prohibit copy constructor
        CPosDefaultPositioner(const CPosDefaultPositioner&);
        // Prohibit assigment operator
        CPosDefaultPositioner& operator= (const CPosDefaultPositioner&);

    private: // Data

        CPosRequestController*      iRequestController;
        TTimeIntervalMicroSeconds   iTrackingInterval;

    };

#endif      // CPOSDEFAULTPOSITIONER_H  
            
// End of File
