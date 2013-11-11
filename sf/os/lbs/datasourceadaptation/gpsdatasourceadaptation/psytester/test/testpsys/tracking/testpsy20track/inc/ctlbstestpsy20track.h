/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CT_LBS_TESTPSY20POSITIONER_H__
#define __CT_LBS_TESTPSY20POSITIONER_H__

//  INCLUDES
#include <lbs/epos_cpositioner.h>

// CONSTANTS

// FORWARD DECLARATIONS
class TPositionInfo;
class CT_LbsTestPsyBase;

// CLASS DECLARATION

/**
*  Used to get position info from an external GPS.
*
*/
class CT_LbsTestPsy20Positioner : public CPositioner
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy20Positioner* NewL(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy20Positioner* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsTestPsy20Positioner();


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
		
		virtual TBool TrackingOverridden() const;
		
		
		virtual void StartTrackingL(
		/* IN  */		const TTimeIntervalMicroSeconds& aInterval
        );

        /**
        * Stop a periodic update session.
        * Any outstanding requests will be cancelled.
        */
        virtual void StopTracking();

    private:

         /**
        * C++ default constructor.
        */
        CT_LbsTestPsy20Positioner();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

        // By default, prohibit copy constructor
        CT_LbsTestPsy20Positioner( const CT_LbsTestPsy20Positioner& );
        // Prohibit assigment operator
        CT_LbsTestPsy20Positioner& operator= ( const CT_LbsTestPsy20Positioner& );

	 private: // Data

		CT_LbsTestPsyBase*				iRequestHandler;
		TRequestStatus*					iStatus;

    };

#endif      // __CT_LBS_TESTPSY20POSITIONER_H__
            
// End of File
