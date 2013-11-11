/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSPOSITIONER_H
#define BTGPSPOSITIONER_H

//  INCLUDES

#include <lbs/epos_cpositioner.h>
#include "BTGPSPositionerExt.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class TBTGPSExtension;
class CBTGPSFix;
class CBTGPSRequestHandler;

// CLASS DECLARATION
/**
*  The instance of the PSY.
*  Routes the location request to the request handler
*  and handles the instance specific data.
*  
*/
class CBTGPSPositioner :public CPositioner, 
                        public MBTGPSPositionerExt
    {
    public:

        /**
         * Creates a new instance of a CPositioner implementation.
         *
         * @param aConstructionParameters Construction parameters for
         *                                CPositioner.
         * @return The positioner implementation.
         */
        static CBTGPSPositioner* NewL(TAny* aConstructionParameters);
    
        /**
         * Destructor.
         */
        virtual ~CBTGPSPositioner();

    public: // Functions from base classes

        /**
        * From CPositioner.
        * Requests position info asynchronously. 
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        *                 The position (TPosition) of this position info object
        *                 must be in WGS84 geodetic datum.
        * @param aStatus The request status
        *
        */
        void NotifyPositionUpdate(TPositionInfoBase& aPosInfo,
                                    TRequestStatus& aStatus);

        /**
        * From CPositioner.
        * Cancels position info request.
        */
        void CancelNotifyPositionUpdate();

        /**
        * From CPositioner. 
        * Indicate if the PSY has overridden tracking. The default
        * implementation returns EFalse.
        * @return ETrue if PSY has own timer, otherwise EFalse.
        */
        TBool TrackingOverridden() const;

        /**
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
        * Any outstanding requests will be cancelled.
        */
        void StopTracking();

        /**
        * From MBTGPSPositionerExt
        * @param aFix The retrieved fix.
		* @param aErr The error code if the request is failed.
		* @return ETrue if the request is completed successfully. EFalse
		*         if the fix doesn't fullfill the requirement of the
		*         client.
        */
        virtual TBool RequestCompleteNotify(
            const CBTGPSFix* aFix, 
            TInt aErr);

        /**
        * From MBTGPSPositionerExt
        * @param aStatus The new position module status
        */
        void ReportStatus(const TPositionModuleStatus& aStatus);
        
        /**
        * From MBTGPSPositionerExt
        */
        void SetInitialNmeaBufferBottom(TInt aIndex);

    protected:  // New functions
        /**
         * Clears the fields of the aPosInfo 
         * @param aPosInfo The position information to return
         */
        void ClearPositionInfo(TPositionInfoBase& aPosInfo);

    private:

        /**
        * C++ default constructor.
        */
        CBTGPSPositioner();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

    private:

        //The required member functions of the CBTGPSPositioner
        TBTGPSExtension* iBTGPSExtension;
        
        //Pointer to NMEA message buffer indicate the last
        //time that NMEA message is returned to client application.
        TInt iNmeaIndex;
        
        //Pointer to NMEA message buffer bottom that this 
        //positioner see
        TInt iNmeaBottom;
    };
 #endif
// End of File
