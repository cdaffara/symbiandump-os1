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



#ifndef CPOSPSYUPDATEINTERVALTEST_H
#define CPOSPSYUPDATEINTERVALTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"

// CONSTANTS
const TTimeIntervalMicroSeconds KTrackingTime(3000);

// CLASS DECLARATION
/**
* Class that handles the test case for
* testing partly that the PSY uses an internal 
* timer for update interval tracking and if so 
* evaluates the returned locations. 
* If not this test is aborted. 
*/
class CPosPSYUpdateIntervalTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYUpdateIntervalTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYUpdateIntervalTest();

    public: // Functions from base classes

        /**
        * From CPosPSYUpdateIntervalTest GetName
        * @param aName The name of the test
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

	public: // Functions from base classes
		
        /**
        * From MPosRequestObserver
        * @param aCompleteCode Complete code of position request.
        * @param aRequestTime Time for request to complete.
        */
        void HandlePositionCompleteL(
        /* IN */	TInt aCompleteCode, 
        /* IN */	TTimeIntervalMicroSeconds aRequestTime
        );

    private:
    
        /**
        * C++ default constructor.
        */
        CPosPSYUpdateIntervalTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        /**
        * ExecuteTestL
        */
        void ExecuteTestL();
        
        // By default, prohibit copy constructor
        CPosPSYUpdateIntervalTest( const CPosPSYUpdateIntervalTest& );
        
        // Prohibit assigment operator
        CPosPSYUpdateIntervalTest& operator= ( const CPosPSYUpdateIntervalTest& );

	private:

		TPositionInfo	iPosInfo;
		TInt			iNrOfUpdateIntervalRequests;

    };

#endif      // CPOSPSYUPDATEINTERVALTEST_H

// End of File
