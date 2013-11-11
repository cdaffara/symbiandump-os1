// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PsdAgt TestCase Test 11.1 - 11.7
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>

// Test system includes
#include "networking/log.h"
#include "networking/teststep.h"
#include "TestStepPsdAgt.h"
#include "TestSuitePsdAgt.h"

// COMMDB Database Undo
#include "DbUndo.h"

// RPacketContext
#include <etelpckt.h> 

// EPsdStartingConfiguration
#include <psdprog.h>

// EConnectionOpen
#include <connectprog.h>

//#include <psdpanic.h>

// Class definitions
#include "PsdAgtTestOpen.h"


// Test case 11.1
enum TVerdict CPsdAgtTest11_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1101);

	ConnectAndWaitForTwentySecondsL();

	// Should have been incremented when DummyETEL simulated the Status changed
	TESTL(iStage == 2);  

	return iTestStepResult;
	}

void CPsdAgtTest11_1::AgentProgress(TInt aStage, TInt aError)
	{
	// Only interested in the 1st notification after we started waiting for
	// RPacketContext::NotifyStatusChange() to signal to PsdOpen
	// Subsequent notifications could be during disconnect etc.
	if(iStage == 1)
		{
		iStage++;
		if(aStage == KConnectionOpen && aError == KErrNone)
			return;
		else TESTE(EFalse,aError);  //We didn't get the notification we were after
		}
	}

// Test case 11.2
enum TVerdict CPsdAgtTest11_2::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 

	agtNotify.SetTestCase(1102);

	ConnectAndWaitForTwentySecondsL();

	// Should have been incremented when DummyETEL simulated the Status changed
	TESTL(iStage == 2);  

	return iTestStepResult;
	}

void CPsdAgtTest11_2::AgentProgress(TInt aStage, TInt aError)
	{
	// Only interested in the 1st notification after we started waiting for
	// RPacketContext::NotifyStatusChange() to signal to PsdOpen
	// Subsequent notifications could be during disconnect etc.
	if(iStage == 1)
		{
		iStage++;
		if(aStage == EPsdSuspended && aError == KErrNone)
			return;
		else TESTE(EFalse, aError); // We didn't get the notification we were after
		}
	}

// Test case 11.3
enum TVerdict CPsdAgtTest11_3::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1103);

	ConnectAndWaitForTwentySecondsL();

	// Should have been incremented when DummyETEL simulated the Status changed
	TESTL(iStage == 2);  

	return iTestStepResult;
	}

void CPsdAgtTest11_3::AgentProgress(TInt aStage, TInt aError)
	{
	// Only interested in the 1st notification after we started waiting for
	// RPacketContext::NotifyStatusChange() to signal to PsdOpen
	// Subsequent notifications could be during disconnect etc.
	if(iStage == 1)
		{
		iStage++;
		if(aStage == EPsdStartingDeactivation && aError == KErrNone)
			return;
		else TESTE(EFalse, aError); // We didn't get the notification we were after
		}
	}

// Test case 11.4
enum TVerdict CPsdAgtTest11_4::doPsdAgtTestStepL( void )
	{
	// This test is really only here for code coverage purposes
	// It's difficult to actually test for any particular result
	// aside from a successful connection / disconnection
	
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1104);

	ConnectAndWaitForTwentySecondsL();
	
	return iTestStepResult;
	}

// Test case 11.5
enum TVerdict CPsdAgtTest11_5::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 

	agtNotify.SetTestCase(1105);

	ConnectAndWaitForTwentySecondsL();

	// Should have been incremented when DummyETEL simulated the Status changed
	TESTL(iStage == 2);  
	
	return iTestStepResult;
	}

void CPsdAgtTest11_5::AgentProgress(TInt aStage, TInt aError)
	{
	// Only interested in the 1st notification after we started waiting for
	// RPacketContext::NotifyStatusChange() to signal to PsdOpen
	// Subsequent notifications could be during disconnect etc.
	if(iStage == 1)
		{
		iStage++;
		if(aStage == EPsdFinishedDeactivation && aError == KErrNone)
			return;
		else TESTE(EFalse,aError); //  We didn't get the notification we were after
		}
	}

// Test case 11.6
enum TVerdict CPsdAgtTest11_6::doPsdAgtTestStepL( void )
	{
	// This test is really only here for code coverage purposes
	// It's difficult to actually test for any particular result
	// aside from a successful connection / disconnection
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1106);

	ConnectAndWaitForTwentySecondsL();

	return iTestStepResult;
	}

// Test case 11.7
enum TVerdict CPsdAgtTest11_7::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1107);

	ConnectAndWaitForTwentySecondsL();

	return iTestStepResult;
	}

// Test case 11.8
// This is a test for the issue PDEF087346 in the TRP TSY's agent, which has a similar state machine.
// Connect, and on encountering an open connection immediately complete a high-priority AO to artificially
// set the progress. If the progress is then later reported by the agent to a lower value, then we have
// an out-of-order AO evaluation "bug" in the agent.

enum TVerdict CPsdAgtTest11_8::doPsdAgtTestStepL( void ) 
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1108);

	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check result codes
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	// Kick off a high priority AO and wait...
	Log(_L("Completing callback AO"));

	CAsyncCallBack* async;
	TCallBack cb(CPsdAgtTest11_8::Callback, static_cast<TAny*>(this));
	async = new(ELeave)CAsyncCallBack(cb, EPriorityHigh);	// must be high priority so that this AO runs before any AOs of the agent after it
															// has declared the connection open. This is to simulate other components completing
															// an AO which was created earlier than any completed AOs the agent may have pending.
	CleanupStack::PushL(async);
	async->CallBack();

	Log(_L("Waiting..."));
	DelayL(2000000);

	// Preserve the progress value at this point
	// If the agent had an AO scheduled to set the progress after it had reported
	// successful connection, then this will NOT be the value set by the callback.
	Log(_L("Preserving reported progress value..."));
	TInt openProgressValue = iProgress;
	

	// Disconnect
	Log(_L("Disconnecting"));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);
	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman and callback so delete them
	CleanupStack::PopAndDestroy(async);
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));

	// Check that the progress value was the value last set by the async call.
	Log(_L("Testing preserved progress value was KLinkLayerOpen..."));
	TESTEL(openProgressValue == KLinkLayerOpen, openProgressValue);

	return iTestStepResult;
	}

void CPsdAgtTest11_8::AgentProgress(TInt aStage, TInt /*aError*/) 
	{
	iProgress = aStage;
	}

TInt CPsdAgtTest11_8::Callback(TAny* aParam)
	{
	CPsdAgtTest11_8* obj = static_cast<CPsdAgtTest11_8*>(aParam);
	obj->AgentProgress(KLinkLayerOpen, KErrNone);		// as per defect, set progress to KLinkLayerOpen 
	
	return KErrNone;
	}
//
