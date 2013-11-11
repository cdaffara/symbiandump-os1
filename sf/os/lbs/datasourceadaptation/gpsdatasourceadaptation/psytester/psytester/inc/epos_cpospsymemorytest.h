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



#ifndef CPOSPSYMEMORYTEST_H
#define CPOSPSYMEMORYTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"

// CONSTANTS

/**
*  Class for testing memory consumption in PSY
*/
class CPosPSYMemoryTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYMemoryTest* NewL();

        /**
        * Destructor.
        */
        ~CPosPSYMemoryTest();
   
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
        /* IN */		const TPosPSYInfo& aPSYInfo, 
        /* IN/OUT */	TRequestStatus& aStatus,
		/* IN */		const TPosPSYTestParameters& aTestParameters
        );

		/**
        * From CPosPSYTestBase CancelTest
        */
        void CancelTest();

    public: // Functions from base classes
		
        /**
        * From MPosPositionObserver
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
        CPosPSYMemoryTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

		void ExecuteTestL();

		void GetMemoryFootPrint(
            TInt& aHeapSize, 
            TInt& aUsedHeapSize, 
            TInt& aHeapMin, 
			TInt& aHepaMax, 
            TInt& aStackSize
        );

        // By default, prohibit copy constructor
        CPosPSYMemoryTest( const CPosPSYMemoryTest& );
        
        // Prohibit assigment operator
        CPosPSYMemoryTest& operator= ( const CPosPSYMemoryTest& );

	private:	// Data

		TInt	iNumberOfRequests;
		TInt	iErrorMemoryPercentLevel;
		TInt	iSizeAfterStart;

		TInt	iHeapAlloc;
		TInt	iBeforeHeapUsed;
		TInt	iLocalUsedHeap;
		TInt	iHeapUsed;
		TInt	iHeapMin;
		TInt	iHeapMax;
		TInt	iStackSize;

		TPositionInfo			iPosInfo;
		
    };

#endif      // CPOSPSYMEMORYTEST_H

// End of File
