// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class definition for the base class of all etel3rd party
// capabilility test cases.
// 
//

/**
 @file 
 @internalTechnology
*/

#ifndef __TE_CAPSTEST_STEP_BASE__
#define __TE_CAPSTEST_STEP_BASE__
#include <test/testexecutestepbase.h>
#include <etel3rdparty.h>
#include <simtsy.h>

/**
The class defines the internal Caps testing timer.
*/
class CCapsTimer : public CTimer
	{

public:
	static CCapsTimer* NewL();
	
	virtual void RunL();
	
private:
	CCapsTimer();
	void ConstructL();
	};

/**
The class defines the common, shared between requests AO.
*/
	
class CAOCommon : public CActive
	{
public:
	CAOCommon(CCapsTimer& aCapsTimer);
	virtual void RunL();
	virtual void DoCancel();
	void SetupAO();
	TRequestStatus& GetInternalStatusObject();
		
private:
	// Reference to test case timer.
	// Required to cancel the timer. 
	CCapsTimer& iCapsTimer;
	};

/**
The class defines the base class for each of capability test.
*/
class CTe_CapsTestSuiteStepBase : public CTestStep
	{
public:
	CTe_CapsTestSuiteStepBase() {CActiveScheduler::Install(&iScheduler);};
	
	// From CTestStep
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	
protected:
	CTelephony* iTelephony;
	CActiveScheduler iScheduler;

private:
	virtual TInt IssueRequestL(TRequestStatus& aStatus) = 0;
	virtual TInt CancelRequestL() = 0;

	TBool IsCapabilitySufficient(RArray<TCapability> *aReqCap = NULL);
	TBool IsCapNetworkServicesPresent();
	
	void ParseCapabilities(TPtrC aString);
	void AddCapability(TPtrC aCapability);
	void InvokeCallsInternallyL();
	void InvokeCallsExternallyL();

private:
	// Property to set up simtsy correct config file number
	RProperty iTestNumberProperty;	
	
	// Property to inform call runner to hangup calls
	RProperty iCallRunnerProperty;
	
	// The simtsy configuration number used with iTestNumberProperty
	// Read from TEF config file
	TInt iSimTsyConfigNumber;
	
	// Number of required calls as a prerequisite to the test step
	// Read from TEF config file
	TInt iNoOfCallsRequired;
	
	// Array of Capabilities for particular, tested API.
	// Read from TEF config file
	RArray<TCapability> iReqCap;  

	// Specifies if tested API is asynchronous
	// Read from TEF config file
	TBool iAsyncRequeset;  
	
	// Specifies if the required calls need to be established in separate process
	// Read from TEF config file
	TBool iExternalCalls;
	
	// Specifies if tested API is a request for notification purposes
	// Read from TEF config file
	TBool iNotifier;
	
	// External process handle
	// Used with iExternalCalls
	RProcess iProcess;
	
	// Timer for notification API test
	CCapsTimer* iTimer;
	};

#endif
