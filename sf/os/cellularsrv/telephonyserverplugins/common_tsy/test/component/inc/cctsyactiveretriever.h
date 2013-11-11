// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
*/

#ifndef CCTSYACTIVERETRIEVER_H
#define CCTSYACTIVERETRIEVER_H

#include <etelutils.h>

//-------------------------------------------------------------------------------
// CActiveRetriever is an Active Object(AO) class created for 
// testing Start function of CAsyncRetrieveVariableLengthBufferV2 derivied classes.
//
// CAsyncRetrieveVariableLengthBufferV2 is also an AO. To use it we
// need to install our active scheduler(AS) into the thread. We start our AS within
// the test step after start of request using an AO function, but we need to stop AS 
// at some moment to finish test step execution.
//
// Base CActiveRetriever class creation purpose is TO STOP active 
// scheduler, after completion of all AO requests.
//--------------------------------------------------------------------------------
class CActiveRetriever : public CActive
    {
    public:
       	enum TTestCase
       	{
       		ECaseNotSpecified,
       		
       		// cancel on completion cases
       		ECasePhase1Cancel,// we have to do 2 cancel test cases because 
       		                  // CAsyncRetrieveVariableLengthBufferV2 AO does 2 requests to CTSY
       		       		      // while processing our request
       		ECasePhase2Cancel,
       		
       		// general cancel test
       		ECaseGeneralCancelCase
       	};

    public:
    	static CActiveRetriever* NewLC(CAsyncRetrieveVariableLengthBufferV2& aAsynchRetrieveBuffer);
    	static CActiveRetriever* NewL(CAsyncRetrieveVariableLengthBufferV2& aAsynchRetrieveBuffer);

        CActiveRetriever(CAsyncRetrieveVariableLengthBufferV2& aAsynchRetrieveBuffer);
        virtual ~CActiveRetriever();

    public: // new functions
    	TBool IsRequestComplete();
    	void PerformCancelIfNeeded();
    	static TInt ResetRequestsNumber(); // resets pending requests number, STATIC 

		TRequestStatus& Status();
    	// AO's request function with test case parameter
        void Activate(TTestCase aTestCase = ECaseNotSpecified);

    protected: // from CActive
        void RunL();
        void DoCancel();

    protected: // new functions	
		CActive& AsynchRetrieveBuffer();
		void SetTestCase(TTestCase aCase);
		TTestCase TestCase();
		void SetPhase1Passed(TBool aPassed = ETrue);
		TBool Phase1Passed();

    	void DoComplete();

    protected: // data
		CAsyncRetrieveVariableLengthBufferV2& iAsynchRetrieveBuffer; //testing API AO from etelmm derieved from this class
		TTestCase iTestCase; // test case parameter
		TBool iPhase1Passed; // phase1 passing flag for ECasePhase2Cancel test case
		
		static TInt iSimultaneousRetrieveRequestsNumber;// STATIC 

	}; // class CActiveRetriever

//--------------------------------------------------------------------------------------------
// If we install standard Active scheduler into this thread, it will catch completion of all requests
// in the thread, causing E32USER-CBase 46 panic during test step execution.
// Base CFilteringActiveScheduler class creation purpose is to avoid E32USER-CBase 46 panic.
// It is achieved by filtering of request completion in CFilteringActiveScheduler::WaitForAnyRequest().
//
// Secondary purpose is to Cancel() RetrieveInfo request on
// receipt of completion of a request phase before invoking 
// AsynchRetrieveBuffer's RunL() function
//---------------------------------------------------------------------------------------------
class CFilteringActiveScheduler : public CActiveScheduler
	{
	public:
		virtual ~CFilteringActiveScheduler();
		
			// add retriever to use later for filtering of request completion in 
			// CFilteringActiveScheduler::WaitForAnyRequest()
		void AddRetrieverL(CActiveRetriever& aRetriever);
		void StartScheduler();
	protected:
		void WaitForAnyRequest();
		TBool IsOneOfMyRequestsComplete();
	private:
		RPointerArray<CActiveRetriever> iRetrievers;
	}; // class CFilteringActiveScheduler

#endif // CCTSYACTIVERETRIEVER_H
