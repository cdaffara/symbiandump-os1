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
// This contains PsdAgt TestCase Test 4.1
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
#include "PsdAgtTestNotificationDisabled.h"


// Test case 4.1,  Context Config change, Service Option reduced, disconnect
enum TVerdict CPsdAgtTest4_1::doPsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;
	TRealAgtNotify agtNotify; 

	agtNotify.SetTestCase(401);

	Log(_L("Loading DUMMYNIF.DLL"));
	// Construct a new Dummy Nifman Instance
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

	DelayL(3000000);

	// Tell the ETEL layer to send a QOS change to PSDAGX
	Log(_L("Notify PSDAGX of QoS change"));
	agtNotify.NotifyAgt(EReduce);

	// Make sure no dialog is presented
	DelayL(3000000);

	// Disconnect
	Log(_L("Disconnecting..."));
	agent->Disconnect();

	// Check result code
	agent->GetCompletionCode(err);
	TESTEL(err==KErrNone,err);

	Log(_L("Disconnect Successful"));
	
	// Finished with Dummy Nifman so delete it
	CleanupStack::PopAndDestroy(agent);
	Log(_L("DUMMYNIF.DLL Unloaded"));
	
	// Finished with database so undo any changes 
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

