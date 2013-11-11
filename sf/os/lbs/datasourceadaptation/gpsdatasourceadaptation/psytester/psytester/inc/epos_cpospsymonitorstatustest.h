/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSPSYMONITORSTATUSTEST_H
#define CPOSPSYMONITORSTATUSTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"

// CLASS DECLARATION

/**
* Class that handles the Monitor Status test case. 
*/
class CPosPSYMonitorStatusTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYMonitorStatusTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYMonitorStatusTest();
        
    public: // Functions from base classes

        /**
        * From CPosPSYMonitorStatusTest GetName
        * @param aName the name of the test
        */
        void GetName(
        /* OUT */    TDes& aName
        ) const;

        /**
        * From CPosPSYTestBase StartTest
        * @param aPSYInfo the information about the psy to be tested
        * @param aStatus status flag from calling object
		* @param aTestParameters test parameters specifying number 
		*		 of location requests to be done etc.
        */
        void StartTest(
        /* IN */        const TPosPSYInfo& aPSYInfo, 
        /* IN/OUT */    TRequestStatus& aStatus,
		/* IN */		const TPosPSYTestParameters& aTestParameters
        );

        /**
        * From CPosPSYTestBase CancelTest
		* Cancels an ongoing test.
        */
        void CancelTest();

	public:	// From base classes
		
        /**
        * From MPosRequestObserver
        * @param aCompleteCode Complete code of position request.
        * @param aRequestTime Time for request to complete.
        */
        void HandlePositionCompleteL(
        /* IN */ TInt aCompleteCode,
        /* IN */ TTimeIntervalMicroSeconds aRequestTime
        );

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYMonitorStatusTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        void CheckStatusL();

        // By default, prohibit copy constructor
        CPosPSYMonitorStatusTest( const CPosPSYMonitorStatusTest& );
        // Prohibit assigment operator
        CPosPSYMonitorStatusTest& operator= ( const CPosPSYMonitorStatusTest& );

	private:

		TPositionInfo	iPosInfo;
		TInt			iNumberOfRequests;

    };

#endif      // CPOSPSYMONITORSTATUSTEST_H

// End of File
