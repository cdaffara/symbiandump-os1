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



#ifndef __CT_LBS_TESTPSY2POSITIONER_H__
#define __CT_LBS_TESTPSY4POSITIONER_H__

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
class CT_LbsTestPsy2Positioner : public CPositioner
    {
   public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy2Positioner* NewL(TAny* aConstructionParameters);

        /**
        * Two-phased constructor.
        */
        static CT_LbsTestPsy2Positioner* NewLC(TAny* aConstructionParameters);

        /**
        * Destructor.
        */
        ~CT_LbsTestPsy2Positioner();


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

    private:

         /**
        * C++ default constructor.
        */
        CT_LbsTestPsy2Positioner();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL(TAny* aConstructionParameters);

        // By default, prohibit copy constructor
        CT_LbsTestPsy2Positioner( const CT_LbsTestPsy2Positioner& );
        // Prohibit assigment operator
        CT_LbsTestPsy2Positioner& operator= ( const CT_LbsTestPsy2Positioner& );

	 private: // Data

		CT_LbsTestPsyBase*	iRequestHandler;
		TRequestStatus*		iStatus;
		TUint				iRequests;


    };

#endif      // __CT_LBS_TESTPSY4POSITIONER_H__
            
// End of File
