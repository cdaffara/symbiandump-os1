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
// Example code to using the RPS request macros.
// Remove the #ifdef/#endif lines to compile
// 
//

#include "RPS_Usage_Example.h"
#include "te_trpglobaltestdata.h"

RPSUsageExampleStep::RPSUsageExampleStep(CGlobalTestData& aGlobalTestData)
/**
 * Constructor
 */
 : CTrpBasicCallStepBase(aGlobalTestData)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRPSUsageExampleDescId);
	}
	
RPSUsageExampleStep::~RPSUsageExampleStep()
/**
 * Destructor
 */
 	{
 	}
	
TVerdict RPSUsageExampleStep::doTestStepL()
/**
 @SYMTestCaseID		RPS_Usage_Example
 @SYMFssID			TRP/TSY/CLRS
 @SYMTestCaseDesc	Demonstrate usage of the macros offered by RPS master. It is assumed RPS Master is connected to RPS Slave.
 @SYMTestPriority	Medium
 @SYMTestStatus		Implemented
 @SYMTestType		CIT
 @SYMTestActions	//TODO: fill in
 @SYMTestExpectedResults	//TODO: fill in
 @SYMTestCaseDependencies	Automatic 
 
 @return - TVerdict code
 Override of base class pure virtual 
 */
	{

	PrintStepTitle(KRPSUsageExampleDesc);
	
	// Open line and call resources
	RMobilePhone& gphone = iGlobalData.Phone();
	RMobileLine lline1;
	RMobileCall lcall1;
	
	TESTL(OpenVoiceLine1(gphone, lline1) == KErrNone);
	CleanupClosePushL(lline1);
	TESTL(OpenCall(lline1, lcall1) == KErrNone);
	CleanupClosePushL(lcall1);
	
	// Request RPS slave to expect a call (that we will make to it),
	// that it is to answer, 
	// wait for 3 sec 
	// then (the RPS slave is to) put the call on hold.
	ANSWERNEXTCALL_THEN_HOLDL(_L("Answer the next call then put on hold"), 3);
	
	// Call RPS slave
	lcall1.Dial(SLAVETELNUM);
	
	// Allow time for the RPS slave's hold operation to complete
	User::After(10000000);
	
	// You may now check the state of the call to ensure it is on hold.
	
	// Request RPS slave to Resume the call (with no delay)
	RESUMEL(_L("Resume the call that is on Hold"), 0);
	
	// Allow time for the RPS slave's resume operation to complete
	User::After(5000000);
	
	// Request RPS slave to hang up.
	HANGUPL(_L("Hang up the call"), 0);
	
	CloseCallRps(lcall1);
	CleanupStack::PopAndDestroy(2);  

	return TestStepResult();
	}

