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
// This contains PsdAgt TestCase Test 14.1 - 14.5
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

// Class definitions
#include "PsdAgtTestReconnect.h"

// Test case 14.1
enum TVerdict CPsdAgtTest14_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 

	agtNotify.SetTestCase(1401);
	// CTestStepPsdAgx base class untilty function
	ReConnectL();

	return iTestStepResult;
	}

// Test case 14.2
enum TVerdict CPsdAgtTest14_2::doPsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1402);

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Attempt connection
	Log(_L("ReConnecting..."));
	agent->ReConnect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrGeneral,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==EPsdFinishedActivation,stage);

	Log(_L("Reconnect fails as expected"));
	
	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));

	return iTestStepResult;
	}


// Test case 14.3
enum TVerdict CPsdAgtTest14_3::doPsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1403);

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Attempt connection
	Log(_L("ReConnecting..."));
	agent->ReConnect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("ReConnect Successful"));
	Log(_L("Waiting for EAgentToNifEventTypeGetDataTransfer event"));

	DelayL(2000000);

	iStage++;

	// Wait for log request to arrive
	CActiveScheduler::Start();

	DelayL(2000000);

	// Disconnect
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnected"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);

	// GetDataVolumeRequest should only have been called twice
	// ie. iStage == 3
	TESTL(iStage == 3);
	
	Log(_L("Check LogDataTransferred values in GENCONN.TXT"));

	return iTestStepResult;
	}

void CPsdAgtTest14_3::PsdAgtGetDataVolumeRequest(TUint32 &aSent, TUint32 &aReceived)
	{
	// Function should only be called twice
	TEST(iStage==1 || iStage == 2);

	// First time data volume is requested
	if(iStage == 1)
		{
		iStage++;
		aSent = 20;     // Sent size
		aReceived = 26; // Rcv size

		// Unblock ConnectAndWaitForAsyncronousRequest()
		CActiveScheduler::Stop();
		}
	// Second time data volume is requested during the Disconnect
	else if(iStage == 2)
		{
		iStage++;
		aSent = 35;     // Sent size
		aReceived = 39; // Rcv size
		}
	}


// Test case 14.4
enum TVerdict CPsdAgtTest14_4::doPsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1404);

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Attempt connection
	Log(_L("ReConnecting..."));
	agent->ReConnect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("ReConnect Successful"));
	// Predetermined QoS warning response
	iDlgSvr->SetQoSWarnResponse(EFalse);

	DelayL(5000000);

	// Tell the ETEL layer to send a QOS change to PSDAGX
	Log(_L("Notify PSDAGT of Context Config change"));
	agtNotify.NotifyAgt(EReduce);

	// Block until we receive dialog notification of status change
	CActiveScheduler::Start();

	DelayL(2000000);

	// Tell the ETEL layer to restore QOS and notify PSDAGX
	Log(_L("Notify PSDAGT of Restore Context Config"));
	agtNotify.NotifyAgt(ERestore);

	// Check to make sure a second dialog is not presented
	DelayL(5000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));

	return iTestStepResult;
	}

void CPsdAgtTest14_4::DialogHasAppeared()
	{
	TEST(iStage==0);
	iStage++;

	// Unblock the call to doPsdAgtTestStepL();
	CActiveScheduler::Stop();
	}


// Test case 14.5
enum TVerdict CPsdAgtTest14_5::doPsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(1405);

	// Construct a new Dummy Nifman Instance
	Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	DelayL(2000000);

	// Attempt connection
	Log(_L("Reconnecting..."));
	agent->ReConnect();
	
	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Reconnect Successful"));
	
	// Predetermined QoS warning response
	iDlgSvr->SetQoSWarnResponse(EFalse);

	DelayL(5000000);

	// Tell the ETEL layer to send a QOS change to PSDAGX
	Log(_L("Notify PSDAGT of QoS change"));
	agtNotify.NotifyAgt(EReduce);

	// Block until we receive dialog notification of status change
	CActiveScheduler::Start();

	DelayL(2000000);

	// Tell the ETEL layer to restore QOS and notify PSDAGX
	Log(_L("Notify PSDAGT of restore QoS value"));
	agtNotify.NotifyAgt(ERestore);

	// Check to make sure a second dialog is not presented
	DelayL(5000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));

	return iTestStepResult;
	}

void CPsdAgtTest14_5::DialogHasAppeared()
	{
	// Only one dialog should be presented
	TEST(iStage==0);
	iStage++;

	// Unblock the call to doPsdAgtTestStepL();
	CActiveScheduler::Stop();
	}
