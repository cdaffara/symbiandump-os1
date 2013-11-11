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



#ifndef CPOSPSYUIDECOMTEST_H
#define CPOSPSYUIDECOMTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"

// CONSTANTS
const TInt KUidEComTestBufSize = 100;

// CLASS DECLARATION
/**
* Class that handles the UID and ECom test case. 
* Opens a PSY and fetches the UID. 
* Reports error if UID or something else is wrong. 
*/
class CPosPSYUidEComTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYUidEComTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYUidEComTest();
     
    public: // Functions from base classes

        /**
        * From CPosPSYUidEComTest GetName
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
        * CancelTest
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
        CPosPSYUidEComTest();
        
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        void CheckUidL();    

        // By default, prohibit copy constructor
        CPosPSYUidEComTest( const CPosPSYUidEComTest& );
        
        // Prohibit assigment operator
        CPosPSYUidEComTest& operator= ( const CPosPSYUidEComTest& );

	private:

		TPositionInfo	iPosInfo;

    };

#endif      // CPOSPSYUIDECOMTEST_H

// End of File
