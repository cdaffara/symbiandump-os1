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



#ifndef CPOSPSYCLEARPOSITIONDATATEST_H
#define CPOSPSYCLEARPOSITIONDATATEST_H

//  INCLUDES
#include <lbssatellite.h>
#include "epos_cpospsytestbase.h"
#include "epos_psytesterconstants.h"

// CONSTANTS
const TInt KClearPositionDataBufSize = 150;

// CLASS DECLARATION

/**
* Class that handles the Clear postion date test case. 
* Includes checking of that PSY clears position data for
* all supported classtypes.
*/
class CPosPSYClearPositionDataTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYClearPositionDataTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYClearPositionDataTest();
    
    public: // Functions from base classes

        /**
        * From CPosPSYClearPositionDataTest GetName
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
        * From CPosPSYClearPositionDataTest CancelTest
        * Cancels an ongoing test.
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
        CPosPSYClearPositionDataTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        /**
		* CheckGenericInfoClearsL
		*/
		void CheckGenericInfoClearsL();

		/**
        * CheckSatelliteInfoClearsL
        */
        void CheckSatelliteInfoClearsL();

        /**
        * CheckCourseInfoClearsL
        */
        void CheckCourseInfoClearsL(
		/* IN */ const TDesC& aPositionType	
		);
 
        /**
        * CheckPositionClearsL
        */
        void CheckPositionClearsL(
		/* IN */ const TDesC& aPositionType
		);

        /**
        * ExecuteTestL
        */
        void ExecuteTestL();
 
        /**
        * PerformTestL
        */
        void PerformTestL(
		/* IN */TInt aCompleteCode	
		);

        /**
        * Completes the ongoing test.
        * @param aCompleteCode System error code to complete test with.
        */
		void CompleteClearPositionDataTest(
		/* IN */ TInt aCompletionCode	
		);

		/**
		* SetHPositionGenericInfoDataL
		*/
		void SetHPositionGenericInfoDataL(
		/* IN/OUT */ HPositionGenericInfo& aGenericInfo
		);
		
		void AddMessageL(
		/* IN */ const TDesC& aFirstMessage, 
		/* IN */ const TDesC& aSecondMessage,
		/* IN */ TInt aMessageType
		);

        // By default, prohibit copy constructor
        CPosPSYClearPositionDataTest( const CPosPSYClearPositionDataTest& );
        
        // Prohibit assigment operator
        CPosPSYClearPositionDataTest& operator= ( const CPosPSYClearPositionDataTest& );

    private: // Data
        
		TUint32								iCurrentTestClass;
		TUint32								iClassesTested;
		TCourse								iCourse;
		TPosition							iPosition;
		TPositionInfo*						iPosInfo;
		TPosPSYTestParameters				iTestParameters;

    };

#endif      // CPOSPSYCLEARPOSITIONDATATEST_H

// End of File
