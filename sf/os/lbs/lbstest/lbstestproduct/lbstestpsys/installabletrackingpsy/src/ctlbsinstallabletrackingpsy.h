/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CT_LBS_TEST_TRACKING__PSY_H__
#define __CT_LBS_TEST_TRACKING__PSY_H__

//  INCLUDES
#include "epos_cpositioner.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CPositionInfo;

// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsInstallableTrackingPsy : public CPositioner
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsInstallableTrackingPsy* NewL(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsInstallableTrackingPsy* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsInstallableTrackingPsy();

    protected:  // Functions from base classes

		/**
        * From CPositioner. Requests position info asynchronously.
        *
        * @param aPosInfo A reference to a position info object. This object
        *                 must be in scope until the request has completed.
        * @param aStatus The request status
        */
		void NotifyPositionUpdate(
		/* IN/OUT */	TPositionInfoBase& aPosInfo,
		/* OUT	  */	TRequestStatus& aStatus
		);

		/**
        * From CPositioner. Cancels position info request.
        */
        void CancelNotifyPositionUpdate();
		
		void StartTrackingL(const TTimeIntervalMicroSeconds& aInterval);

		void StopTracking();

        virtual TBool TrackingOverridden() const;

    private:

         /**
        * C++ default constructor.
        */
        CT_LbsInstallableTrackingPsy();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

		static TInt CompleteRequest(
		/* IN/OUT */	TAny *aThis
		);
        
		// By default, prohibit copy constructor
        CT_LbsInstallableTrackingPsy( const CT_LbsInstallableTrackingPsy& );
        // Prohibit assigment operator
        CT_LbsInstallableTrackingPsy& operator= ( const CT_LbsInstallableTrackingPsy& );
	
	private: //Data
		MPositionerStatus* iPositioner;
		CPeriodic* iPeriodicTimer;
		TTimeIntervalMicroSeconds32 iInterval;
		TRequestStatus* iStatus;
		TBool iSucceed;
		TBool iFirst;
		TBool iTrackingEnabled;
    };

#endif      // __CT_LBS_TEST_TRACKING__PSY_H__
            
// End of File
