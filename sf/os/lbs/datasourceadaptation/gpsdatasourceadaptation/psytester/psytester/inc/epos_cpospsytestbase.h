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



#ifndef CPOSPSYTESTBASE_H
#define CPOSPSYTESTBASE_H

//  INCLUDES
#include "epos_tpospsyinfo.h"
#include "epos_tpospsytestparameters.h"
#include "epos_cpospsyresults.h"
#include "epos_cpospsyrequester.h"
#include "epos_cpospsymultirequester.h"
#include "epos_mposrequestobserver.h"

// CONSTANTS
const TInt KBufSize = 150;

// CLASS DECLARATION
/**
*  Abstract subclass to be implemented by all psy tests.
*
*  This class is subclassed by all classes functioning as test case classes
*/
class CPosPSYTestBase : public CBase, public MPosRequestObserver
    {
     public:  // Constructors
				
		/**
        * Destructor.
        */
        virtual ~CPosPSYTestBase();

	 public: // New methods
       
        /**
        * Called to set up the result array for the test.
        */
        void BaseConstructL();
        
        /**
        * Called to get the name of the test.
        * @param aName the name of the test
        */
        virtual void GetName(
        /* OUT */    TDes& aName
        ) const = 0;

        /**
        * Function starting a test case
        * @param aPSYInfo the information about the psy to be tested
        * @param aStatus status flag from calling object
		* @param aTestParameters test parameters specifying number 
		*		 of location requests to be done etc.
        */
        virtual void StartTest(
        /* IN */        const TPosPSYInfo& aPSYInfo, 
        /* IN/OUT */    TRequestStatus& aStatus,
		/* IN */		const TPosPSYTestParameters& aTestParameters
        ) = 0;

		/**
        * Function to be called when cancelling tests
        */
        virtual void CancelTest() = 0;

		/**
        * Function to be called when a test completes normaly
        */
        virtual void CompleteTest();

		/**
        * Function to be called when a test completes normaly
		* @param aCompleteCode the request completion code
        */
        virtual void CompleteTest(
		/* IN */	TInt aCompleteCode
		);

		/**
        * Function to be called when a test receives a unexpected completion code
		* @param aCompleteCode the request completion code
        */
        void UnexpectedCompletionCode(
			TInt aCompleteCode);

		/**
        * Function to be called when a test receives a unexpected completion code
		* @param aCompleteCode the request completion code
		* @param aRequestNumber the request number of th completed request
		* @param aMessageType the message type to be logged
        */
        void UnexpectedCompletionCode(
		/* IN */	TInt aCompleteCode, 
		/* IN */	TInt aRequestNumber, 
		/* IN */	TInt aMessageType = EWarningMessage);

        /**
        * Function for clearing test results
        */
        void ClearResults();

        /**
        * TestResults
        * @return CPosPSYResults* the result array
        */
        CPosPSYResults* TestResults();

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

		/**
        * Called when a request went wrong.
        * @param aErrorCode The error code for which type of error that occured.
        */
        void HandleRequestError( 
		/* IN */ TInt aErrorCode
		);

		/**
        * Add test result to log
        * @param aMsg the message the should be handled.
		* @param aResultType the type of result to be logged (warning, error, info, display)
        */
		void AddTestResultL(
        /* IN */   const TDesC& aMessage, 
        /* IN */   const TInt aResultType
        );

	 protected:

		CPosPSYTestBase();

		void AddTestPSYResultL(
        /* IN */ const CPosPSYResult& aTestResult
        );

	 protected: // Data
		
		TRequestStatus*			iRequestStatus;
		TInt					iTestStatus;
        
		TPosPSYInfo				iPSYInfo;

		CPosPSYRequester*		iRequester;
		CPosPSYMultiRequester*	iMultiRequester;
		TInt					iRequestNumber;

        CPosPSYResults*			iTestResults;
    };

#endif      // CPOSPSYTESTBASE_H  
            
// End of File
