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



#ifndef CPOSPSYPARTIALUPDATETEST_H
#define CPOSPSYPARTIALUPDATETEST_H

//  INCLUDES
#include <lbspositioninfo.h>
#include "epos_cpospsytestbase.h"

// CLASS DECLARATION
class MPosPositionObserver;

/**
* Class for testing partial updates in PSY 
*/
class CPosPSYPartialUpdateTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYPartialUpdateTest* NewL();

        /**
        * Destructor.
        */
        ~CPosPSYPartialUpdateTest();

	public:

		TBool IsFullPositionFix(TPositionInfoBase& aPosInfo);

    public: // Functions from base classes

        /**
        * From CPosPSYTestBase
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
		* Cancels an ongoing test.
        */
        void CancelTest();

    public: // Functions from base classes

		void HandlePositionCompleteL(
			TInt aCompleteCode, 
			TTimeIntervalMicroSeconds aRequestTime
			);

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYPartialUpdateTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

		void ExecuteTestL();

        // By default, prohibit copy constructor
        CPosPSYPartialUpdateTest( const CPosPSYPartialUpdateTest& );
        
        // Prohibit assigment operator
        CPosPSYPartialUpdateTest& operator= ( const CPosPSYPartialUpdateTest& );
		
	private:

		TInt					iTotalNumberOfRequests;
		TInt					iNumberOfRequests;
		TInt					iPartialUpdates;
		
		TPositionInfo			iPosInfo;

    };

#endif      // CPOSPSYPARTIALUPDATETEST_H

// End of File
