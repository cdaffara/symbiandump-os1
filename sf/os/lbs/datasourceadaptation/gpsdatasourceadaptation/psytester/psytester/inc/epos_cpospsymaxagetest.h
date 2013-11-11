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



#ifndef CPOSPSYMAXAGETEST_H
#define CPOSPSYMAXAGETEST_H

//  INCLUDES
#include <lbssatellite.h>
#include "epos_cpospsytestbase.h"

// CONSTANTS
const TInt KMsgSize = 80;

/**
*  Class for PSY parameter validation tests
*/
class CPosPSYMaxAgeTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYMaxAgeTest* NewL();

        /**
        * Destructor.
        */
        ~CPosPSYMaxAgeTest();

    public: // New functions
   
   		/**
		* Checks if two positions have the same time stamp.
		*
		* @param aPos First position to be evaulated.
		* @param aSecondPos Position to be checked against.
		* @return TBool ETrue if time stamos are the same, otherwise EFalse.
		*/
		TBool IsSameTimeInPosition(
        /* IN */	TPositionInfo& aPos, 
        /* IN */	TPositionInfo& aSecondPos
        );
        
		/**
		* Checks if two positions are the same.
		*
		* @param aPos First position to be evaulated.
		* @param aSecondPos Position to be checked against.
		* @return TBool ETrue if positions are the same, otherwise EFalse.
		*/
		TBool IsSamePosition(
        /* IN */	TPositionInfo& aPos, 
        /* IN */	TPositionInfo& aSecondPos
        );

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
        */
        void CancelTest();

    public: // Functions from base classes

        /**
        * From MPosRequestObserver
		*
        * @param aCompleteCode Code that requested completed with.
		* @param aRequestTime Time to make request.
        */
        void HandlePositionCompleteL(
        /* IN */    TInt aCompleteCode,
        /* IN */    TTimeIntervalMicroSeconds aRequestTime
        );

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYMaxAgeTest();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

		void ExecuteTestL();

		void TestMaxAgeForPositionL(
		/* IN */	TPositionInfo& aPosInfo, 
		/* IN */	TPositionInfo& aPosInfo2, 
		/* IN */	const TDesC& aPosName
		);

		void TestMaxAgeGenericL();

		void CompleteMaxAgeTest(TInt aCompletionCode);
		
		static TInt PeriodicCallback(
            TAny* aAny
        );

		void StartSchedulerForAWhileL(
            TTimeIntervalMicroSeconds32 aTimeout
        );

		TBool IsEqual(
		/* IN */ TReal32 aReal1,
		/* IN */ TReal32 aReal2
		);

		TBool IsEqual(
		/* IN */ TReal64 aReal1,
		/* IN */ TReal64 aReal2
		);

		TBool IsEqual(
		/* IN */ TRealX aReal1,
		/* IN */ TRealX aReal2
		);

        // By default, prohibit copy constructor
        CPosPSYMaxAgeTest( const CPosPSYMaxAgeTest& );

        // Prohibit assigment operator
        CPosPSYMaxAgeTest& operator= ( const CPosPSYMaxAgeTest& );
		
	private:

		CPeriodic*				iTimer;
		TUint32					iClassesTested;

		TTime					iTime1;
		TTime					iTime2;

		TPositionInfo*			iPos;
		TPositionInfo*			iPos2;

		TPositionInfo			iPosInfo;
		TPositionInfo			iPosInfo2;
		TPositionCourseInfo		iPosCourseInfo;
		TPositionCourseInfo		iPosCourseInfo2;
		TPositionSatelliteInfo	iPosSatInfo;
		TPositionSatelliteInfo	iPosSatInfo2;
		HPositionGenericInfo*	iGenInfo;
		HPositionGenericInfo*	iGenInfo2;

		HBufC*					iPosName;

		TInt					iMaxAgeWaitTimeSeconds;
    };

#endif      // CPOSPSYMAXAGETEST_H

// End of File
