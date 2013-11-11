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
// This contains PsdAgx TestCase Test 3.1 - 3.31
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

//RPacketQoS
#include "etelQoS.h"

// COMMDB Database Undo
#include "DbUndo.h"

// Class definitions
#include "PsdAgtTestQoSChange.h"


// Test case 3.1,  Context Config change, Assured Mode changes from True to False, disconnect
enum TVerdict CPsdAgtTest3_1::doPsdAgtTestStepL( void )
	{
	// COMMDB must be setup in assured mode before running the test suite
	// Otherwise this test will fail

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(301);
	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_1::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_1::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.2,  Context Config change, Assured Mode changes from True to False, don't disconnect
enum TVerdict CPsdAgtTest3_2::doPsdAgtTestStepL( void )
	{
	// COMMDB must be setup in assured mode before running the test suite
	// Otherwise this test will fail

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(302);
	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_2::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.3,  Context Config change, Assured Mode changes from True to False before timeout
enum TVerdict CPsdAgtTest3_3::doPsdAgtTestStepL( void )
	{
	// COMMDB must be setup in assured mode before running the test suite
	// Otherwise this test will fail

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(303);
	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}


// Test case 3.4,  Context Config change, RLP mode changed from Transparent to NonTransparent
enum TVerdict CPsdAgtTest3_4::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(304);
	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	// Undo change to RLPMode
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

void CPsdAgtTest3_4::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_4::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.5,  Context Config change, RLP mode changed from Transparent to NonTransparent, don't disconnect
enum TVerdict CPsdAgtTest3_5::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(305);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	// Undo change to RLPMode
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

void CPsdAgtTest3_5::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.6,  Context Config change, RLP mode changed from Transparent to NonTransparent
enum TVerdict CPsdAgtTest3_6::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(306);


	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	// Undo change to RLPMode
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}


// Test case 3.7,  Context Config change, RLP mode changed from NonTransparent to Transparent, disconnect
enum TVerdict CPsdAgtTest3_7::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(307);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	// Undo change to RLPMode
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

void CPsdAgtTest3_7::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_7::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.8,  Context Config change, RLP mode changed from NonTransparent to Transparent, don't disconnect
enum TVerdict CPsdAgtTest3_8::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(308);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	// Undo change to RLPMode
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

void CPsdAgtTest3_8::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.9,  Context Config change, RLP mode changed from NonTransparent to Transparent
enum TVerdict CPsdAgtTest3_9::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(309);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	// Undo change to RLPMode
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}



// Test case 3.10,  Context Config change, Priority reduced, disconnect
enum TVerdict CPsdAgtTest3_10::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(310);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_10::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_10::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.11,  Context Config change, Priority reduced, don't disconnect
enum TVerdict CPsdAgtTest3_11::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(311);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_11::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.12,  Context Config change, Priority reduced
enum TVerdict CPsdAgtTest3_12::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(312);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}


// Test case 3.13,  Context Config change, UplinkRate reduced, disconnect
enum TVerdict CPsdAgtTest3_13::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(313);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_13::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_13::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.14,  Context Config change, UplinkRate reduced, don't disconnect
enum TVerdict CPsdAgtTest3_14::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(314);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_14::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.15,  Context Config change, UplinkRate reduced
enum TVerdict CPsdAgtTest3_15::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(315);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}

// Test case 3.16,  Context Config change, DownlinkRate reduced, disconnect
enum TVerdict CPsdAgtTest3_16::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(316);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_16::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_16::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.17,  Context Config change, DownlinkRate reduced, don't disconnect
enum TVerdict CPsdAgtTest3_17::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(317);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_17::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.18,  Context Config change, DownlinkRate reduced
enum TVerdict CPsdAgtTest3_18::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(318);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}


// Test case 3.19,  Context Config change, FwdDataLossRate increased, disconnect
enum TVerdict CPsdAgtTest3_19::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(319);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_19::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_19::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.20,  Context Config change, FwdDataLossRate increased, don't disconnect
enum TVerdict CPsdAgtTest3_20::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(320);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_20::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.21,  Context Config change, FwdDataLossRate increased
enum TVerdict CPsdAgtTest3_21::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(321);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}


// Test case 3.22,  Context Config change, RevDataLossRate increased, disconnect
enum TVerdict CPsdAgtTest3_22::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(322);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_22::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_22::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.23,  Context Config change, RevDataLossRate increased, don't disconnect
enum TVerdict CPsdAgtTest3_23::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(323);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_23::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.24,  Context Config change, RevDataLossRate increased
enum TVerdict CPsdAgtTest3_24::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(324);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}



// Test case 3.25,  Context Config change, FwdMaxDelay increased, disconnect
enum TVerdict CPsdAgtTest3_25::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(325);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_25::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_25::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.26,  Context Config change, FwdMaxDelay increased, don't disconnect
enum TVerdict CPsdAgtTest3_26::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(326);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_26::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.27,  Context Config change, FwdMaxDelay increased
enum TVerdict CPsdAgtTest3_27::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(327);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}

// Test case 3.28,  Context Config change, RevMaxDelay increased, disconnect
enum TVerdict CPsdAgtTest3_28::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(328);

	// Call QoSChange connection function
	QoSChangeDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_28::DialogHasAppeared()
	{
	// Should only display the dialog once before the disconnect request
	TEST(iStage==0);

	iStage++;
	}

void CPsdAgtTest3_28::PsdAgtDisconnectRequest()
	{
	// Should only receive a disconnect request immediately
	// after displaying the first dialog
	TEST(iStage==1);
	iStage++;

	// Unblock the call to QoSChangeDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}


// Test case 3.29,  Context Config change, RevMaxDelay increased, don't disconnect
enum TVerdict CPsdAgtTest3_29::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(329);

	// Call QoSChange connection function
	QoSChangeNoDisconnectL(agtNotify);

	return iTestStepResult;
	}

void CPsdAgtTest3_29::DialogHasAppeared()
	{
	// Should only display the dialog once 
	TEST(iStage==0);

	iStage++;

	// Unblock the call to QoSChangeNoDisconnectL(agtNotify);
	CActiveScheduler::Stop();
	}



// Test case 3.30,  Context Config change, RevMaxDelay increased
enum TVerdict CPsdAgtTest3_30::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(330);

	// Call QoSChange connection function
	QoSChangeWithinTimeoutL(agtNotify);

	return iTestStepResult;
	}

// Test case 3.31,  GPRS Context Config change, no effect
enum TVerdict CPsdAgtTest3_31::doPsdAgtTestStepL( void )
	{
	TInt err;
	TInt stage;
	CNifAgentRefN1 *agent;

	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(331);
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
	TESTEL(err==KErrNone, err);

	// Check ConnectComplete was reported at the correct stage in the state machine
	agent->GetProgressStage(stage);
	TESTEL(stage==KConnectionOpen,stage);

	Log(_L("Connect Successful"));

	// Predetermined QoS warning response
	iDlgSvr->SetQoSWarnResponse(EFalse);

	DelayL(3000000);

	// Tell the ETEL layer to send a QoS change to PSDAGX
	Log(_L("Notify PSDAGT of QoS change"));
	agtNotify.NotifyAgt(EReduce);

	// Wait to make sure that no dialog appears
	DelayL(3000000);

	// Disconnect
	Log(_L("Disconnecting..."));
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

