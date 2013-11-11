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
// This contains PsdAgt TestCase Test 2.1 - 2.4
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

// RPacketContext
#include <etelpckt.h> 

// EConnectionOpen
#include <connectprog.h>

// COMMDB Database Undo
#include "DbUndo.h"

// Class definitions
#include "Psdagttestcontextconfigchange.h"


// Test case 2.1,  Context Config change, Service Option reduced, disconnect
enum TVerdict CPsdAgtTest2_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(201);
	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

void CPsdAgtTest2_1::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest2_1::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
//	TEST(iStage==1);
	TEST(iStage==0);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL();
	CActiveScheduler::Stop();
	}


// Test case 2.2,  Context Config change, Service Option reduced, don't disconnect
enum TVerdict CPsdAgtTest2_2::doPsdAgtTestStepL( void )
	{

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(202);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

void CPsdAgtTest2_2::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL();
	CActiveScheduler::Stop();
	}


// Test case 2.3,  Context Config change, Service Option reduced
enum TVerdict CPsdAgtTest2_3::doPsdAgtTestStepL( void )
	{
	
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(203);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

// Test case 2.4,  GPRS Context Config change, no effect
enum TVerdict CPsdAgtTest2_4::doPsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;
	TRealAgtNotify agtNotify; 

	agtNotify.SetTestCase(204);
	SetGPRSModeL(agtNotify);

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

	// Predetermined QoS warning response
	iDlgSvr->SetQoSWarnResponse(EFalse);

	DelayL(3000000);

	// Tell the ETEL layer to send a context config change to PsdAgt
	Log(_L("Notify PsdAgt of Context Config change"));
	agtNotify.NotifyAgt(EReduce);

	// Wait to make sure that no dialog appears
	DelayL(3000000);

	// Disconnect
	Log(_L("Disconnecting"));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone, err);

	Log(_L("Disconnect Successful"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));

	return iTestStepResult;
	}

