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



#ifndef CPOSPSYMULTIPLESESSIONTEST_H
#define CPOSPSYMULTIPLESESSIONTEST_H

// INCLUDES
#include <lbspositioninfo.h>
#include "epos_cpospsytestbase.h"
#include "epos_cpospsymultirequester.h"

// CONSTANTS
const TInt KMessageBufSize = 150;

// DATA TYPES
enum _TestState 
	{
	EMultipleSessions = 0,
	EMultipleSessionsCancel,
	EStressTest,
	EAllTested
	};

// CLASS DECLARATION

/**
* Class that handles the Multiple session test case. 
*/
class CPosPSYMultipleSessionTest : public CPosPSYTestBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPosPSYMultipleSessionTest* NewL();

        /**
        * Destructor.
        */
        virtual ~CPosPSYMultipleSessionTest();
               
    public: // Functions from base classes

        /**
        * From CPosPSYMultipleSessionTest GetName
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
        CPosPSYMultipleSessionTest(); 
        
        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        /**
        * ExecuteTestL()
        */
        void ExecuteTestL();
        
        /**
        * TestMultipleSessionsL()
        */
        void TestMultipleSessionsL();

        /**
        * CancelTestL()
        */
        void CancelTestL();    
        
        /**
        * CheckTimeToFixL()
        */
        void CheckTimeToFixL();

        /**
        * CheckStatusesL()
        */
        void CheckStatusesL();

        /**
        * CheckBundlingQueuingL()
        */
        void CheckBundlingQueuingL();

        /**
        * CheckUidL()
        */
        void CheckUidL();

        /**
        * CheckAndClearResultsL()
        */
        void CheckAndClearResultsL();

		/**
		* GetResultL()
		*/
		void GetResultL();

		/**
		* CheckCanceledL()
		*/
		void CheckCanceledL();

        // By default, prohibit copy constructor
        CPosPSYMultipleSessionTest( const CPosPSYMultipleSessionTest& );
        // Prohibit assigment operator
        CPosPSYMultipleSessionTest& operator= ( const CPosPSYMultipleSessionTest& );

     private: // Data
        
        RArray<TTimeIntervalMicroSeconds>	iTimeToFixArray;
        RPointerArray<TPositionInfo>		iPositionArray;
        RArray<TRequestStatus>				iStatusArray;

		TInt								iNumberOfSessions;
		TInt								iTestState;
		TInt								iNrOfInstances;
		TInt								iNrOfCancelingInstances;
		TInt								iNrOfStressInstances;
		TInt								iTimeToFixThreshold;
    
    };

#endif      // CPOSPSYMULTIPLESESSIONTEST_H

// End of File
