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
// This contains CsdAgt TestCase Test 3.1 - 3.2
// 
//

// EPOC includes
#include <e32base.h>
#include <in_sock.h>
#include <dummyetel.h> // include here to avoid picking up the real etel.h
#include <etelmm.h>

// Test system includes
#include "log.h"
#include "teststep.h"
#include "TestStepCsdAgt.h"
#include "TestSuiteCsdAgt.h"

// COMMDB Database Undo
#include "DbUndo.h"

// EFinishedDialling
#include <csdprog.h>

// Class definitions
#include "CsdAgtTestReconnect.h"


/**
 * Test case 3.1, CSD reconnect after open state
 *
 */
enum TVerdict CCsdAgtTest3_1::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify; 

	AgtNotify.SetTestCase(10301);
	SetGSMModeL(AgtNotify);

	// Call ReConnect() utility function
	ReConnectL();

	return iTestStepResult;
	}

/**
 * Test case 3.2 CSD reconnect fails
 *
 */
enum TVerdict CCsdAgtTest3_2::doCsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	TRealAgtNotify AgtNotify; 
	AgtNotify.SetTestCase((TInt)10302);
	SetGSMModeL(AgtNotify);

	// Construct a new Dummy Nifman Instance
	// Log(_L("Loading DUMMYNIF.DLL"));
	agent = CNifAgentRefN1::NewL(this);
	CleanupStack::PushL(agent);

	// Attempt connection
	Log(_L("Connecting..."));
	agent->Connect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==ECsdConnectionOpen,stage);

	Log(_L("Connect Succeeded"));

	DelayL(2000000);

	AgtNotify.SetTestCase((TInt)10303);

	// Attempt connection
	Log(_L("ReConnecting..."));
	agent->ReConnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrGeneral,err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==ECsdScannedScript,stage);

	DelayL(2000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check for an error code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect Succeeded"));

	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	// Log(_L("DUMMYNIF.DLL Unloaded"));

	return iTestStepResult;
	}
