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



#ifndef CPOSPSYMONITORTHREADTEST_H
#define CPOSPSYMONITORTHREADTEST_H

//  INCLUDES
#include "epos_cpospsytestbase.h"

// DATA TYPES
enum _ThreadTestState
	{
	EFirstVerification = 0,
	EMiddleVerification,
	ELastVerification
	};

// CONSTANTS
_LIT(KPSYTesterProcName, "PSY Tester");

// CLASS DECLARATION
/**
* Class that handles the test case for monitoring 
* process parameters as number of threads etc.
*/
class CPosPSYMonitorThreadTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYMonitorThreadTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYMonitorThreadTest();
     
    public: // Functions from base classes

        /**
        * From CPosPSYTestBase GetName
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
        */
        void CancelTest();

		/**
        * From MPosPositionObserver
        * @param aCompleteCode complete code of position request.
        * @param aRequestTime time for request to complete.
        */
        void HandlePositionCompleteL(
        /* IN */	TInt aCompleteCode, 
        /* IN */	TTimeIntervalMicroSeconds aRequestTime
        );

    private:

        /**
        * C++ default constructor.
        */
        CPosPSYMonitorThreadTest();
        
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

#ifndef __WINS__

		void ExecuteTestL();	

		void MakeRequestL();	

		void VerifyProcessParametersL();

		TInt CheckProcessParametersL();

		void GetProcessNameAndPriorityL(
		/* OUT */ TFullName& aProcessName,
		/* OUT */ TProcessPriority& aProcessPriority,
		/* IN */  const TFullName& aThreadName
		);

		void ResetResults();

		void CheckResultsL();

#endif

        // By default, prohibit copy constructor
        CPosPSYMonitorThreadTest( const CPosPSYMonitorThreadTest& );
        
        // Prohibit assigment operator
        CPosPSYMonitorThreadTest& operator= ( const CPosPSYMonitorThreadTest& );

	private:
		
		TPositionInfo iPosInfo;
		TFullName iProcessName;
		TInt iNumberOfThreads;
		TInt iVerificationType;
		TProcessPriority iProcessPriority;
		TBool iProcessNameChanged;
		TBool iThreadsCreated;
		TBool iThreadsNotClosed;
		TBool iProcessPriorityChanged; 

    };

#endif // CPOSPSYMONITORTHREADTEST_H

// End of File
