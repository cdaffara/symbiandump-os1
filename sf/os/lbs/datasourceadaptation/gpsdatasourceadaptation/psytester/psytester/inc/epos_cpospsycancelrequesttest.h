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



#ifndef CPOSPSYCANCELREQUESTTEST_H
#define CPOSPSYCANCELREQUESTTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"
#include "epos_cpospsymultirequester.h"

// CLASS DECLARATION
/**
*  Handles Cancel Request Test Cases
*/
class CPosPSYCancelRequestTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYCancelRequestTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYCancelRequestTest();
    
    public: // Functions from base classes

        /**
        * From CPosPSYTestBase GetName
        * @param TDes& aName the name of the test
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
        */
        void CancelTest();

	public: // Functions from base classes
		
        /**
        * From MPosRequestObserver
        * @param aCompleteCode, complete code of position request.
        * @param aRequestTime, time for request to complete.
        */
        void HandlePositionCompleteL(
        /* IN */	TInt aCompleteCode, 
        /* IN */	TTimeIntervalMicroSeconds aRequestTime
        );

	private:

        /**
        * C++ default constructor.
        */
        CPosPSYCancelRequestTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * Function executing all test cases in Cancel Request Test
        */
        void ExecuteTestL();

        /**
        * Function testing cancellation after 
        * having performed a location request
        */
        void TestCancelRequestL();

        /**
        * Function testing cancellation without 
        * having performed a location request
        */
        void TestCancelWithoutPreceedingRequestL();

        // By default, prohibit copy constructor
        CPosPSYCancelRequestTest( const CPosPSYCancelRequestTest& );
        
        // Prohibit assigment operator
        CPosPSYCancelRequestTest& operator= ( const CPosPSYCancelRequestTest& );

	private:

    };

#endif      // CPOSPSYCANCELREQUESTTEST_H

// End of File
