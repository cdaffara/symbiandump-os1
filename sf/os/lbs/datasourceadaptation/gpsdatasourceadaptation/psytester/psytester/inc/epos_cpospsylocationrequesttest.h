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



#ifndef CPOSPSYLOCATIONREQUESTTEST_H
#define CPOSPSYLOCATIONREQUESTTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"

// CONSTANTS
const TInt KLocationRequestBufSize = 100;

_LIT(KTestPositionInfo, "==== Testing capabilities with TPositionInfo ====");
_LIT(KTestGenericInfo, "==== Testing capabilities with HPositionGenericInfo ====");
_LIT(KTestCourseInfo, "==== Testing capabilities with TPositionCourseInfo ====");
_LIT(KTestSatelliteInfo, "==== Testing capabilities with TPositionSatelliteInfo ====");

// CLASS DECLARATION

/**
* Class that handles the Location Request test case. 
* Includes checking of a number of parameters and 
* methods associated with location requests.
*/
class CPosPSYLocationRequestTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYLocationRequestTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYLocationRequestTest();
    
    public: // Functions from base classes

        /**
        * From CPosPSYLocationRequestTest GetName
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
        * From CPosPSYLocationRequestTest CancelTest
        * Cancels an ongoing test.
        */
        void CancelTest();
		
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
        CPosPSYLocationRequestTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
    
        /**
        * CheckAddressCapabilityL
        */
        void CheckAddressCapabilityL();
        
        /**
        * CheckBuildingCapabilityL
        */
        void CheckBuildingCapabilityL();

        /**
        * CheckCompassCapabilityL
        */
        void CheckCompassCapabilityL();
        
        /**
        * CheckDirectionCapabilityL
        */
        void CheckDirectionCapabilityL();
        
        /**
        * CheckHorizontalAccuracyL
        */
        void CheckHorizontalAccuracyL();
        
        /**
        * CheckMediaCapabilityL
        */
        void CheckMediaCapabilityL();
        
        /**
        * CheckNMEACapabilityL
        */
        void CheckNMEACapabilityL();
        
        /**
        * CheckSatelliteCapabilityL
        */
        void CheckSatelliteCapabilityL();
        
        /**
        * CheckSpeedCapabilityL
        */
        void CheckSpeedCapabilityL();
        
        /**
		* CheckTimeL
		*/
		void CheckTimeL();

		/**
        * CheckTimeToFirstFixL
        * @param aInfo the Time to First Fix value to check
        */
        void CheckTimeToFirstFixL(
        /* IN */ TTimeIntervalMicroSeconds& aInfo
        );

        /**
        * CheckTimeToFixL
        */
        void CheckTimeToFixL();
 
        /**
        * CheckUidL
        */
        void CheckUidL();

        /**
        * CheckVerticalAccuracyL
        */
        void CheckVerticalAccuracyL();

        /**
        * CheckNrOfCapabilities
        */
        TInt CheckNrOfCapabilities();

        /**
        * ExecuteTestL
        */
        void ExecuteTestL();
 
        /**
        * PerformTestL
        */
        void PerformTestL();

        /**
        * SetRequestedFieldIds
        * @param aInfo the position info object to set the requested fields to
        */
        void SetRequestedFieldIds(
        /* IN/OUT */ HPositionGenericInfo* aInfo
        );

        /**
        * Completes the ongoing test.
        * @param aCompleteCode System error code to complete test with.
        */
		void CompleteLocationRequestTest(
		/* IN */ TInt aCompletionCode	
		);
		
		/**
        * Appends position data to array if completion code is valid
		* @param aCompleteCode the completion code of the location request
		* @param aRequestTime the time the request took
        */
		void AppendIfValidL(
		/* IN */ TInt aCompleteCode,
		/* IN */ TTimeIntervalMicroSeconds aRequestTime
		);

		/**
		* Checks test status and makes new request if neccessary otherwise checks results
		* @param aTestStatus the status of the test
		* @param aRequestTime time for request to complete.
		*/
		void CheckTestStatusL(
		/* IN */ TInt aTestStatus,
		/* IN */ TTimeIntervalMicroSeconds aRequestTime
		);

        // By default, prohibit copy constructor
        CPosPSYLocationRequestTest( const CPosPSYLocationRequestTest& );
        
        // Prohibit assigment operator
        CPosPSYLocationRequestTest& operator= ( const CPosPSYLocationRequestTest& );

    private: // Data
        
        RArray<TTimeIntervalMicroSeconds>	iTimeToFixArray;
        RPointerArray<TPositionInfo>		iPositionArray;
		TUint32								iCurrentTestClass;
		TUint32								iClassesTested;
		TInt								iNrOfCapabilities;
		TPositionInfo*						iPosInfo;
		TPosPSYTestParameters				iTestParameters;

    };

#endif      // CPOSPSYLOCATIONREQUESTTEST_H

// End of File
