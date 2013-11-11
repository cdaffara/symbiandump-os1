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
// This contains PsdAgt TestCase Test 1.1 - 1.14
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

// Class definitions
#include "Psdagttestcommdbparameters.h"


// Test case 1.1,  CDMA connect and disconnect
enum TVerdict CPsdAgtTest1_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(101);

	// Call basic connect / disconnect function from base class
	NormalConnectL();
	return iTestStepResult;
	}

// Test case 1.2 
enum TVerdict CPsdAgtTest1_2::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(102);

	ConnectExpectErrorL(KErrNotSupported,EPsdStartingConfiguration);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

// Test case 1.3 
enum TVerdict CPsdAgtTest1_3::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(103);

	ConnectExpectErrorL(KErrNotSupported,EPsdStartingConfiguration);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

// Test case 1.4 
enum TVerdict CPsdAgtTest1_4::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(104);

	ConnectExpectErrorL(KErrNotSupported,EPsdStartingConfiguration);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}


// Test case 1.5,  GPRS connect and disconnect
enum TVerdict CPsdAgtTest1_5::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(105);
	SetGPRSModeL(agtNotify);

	// Call basic connect / disconnect function from base class
	NormalConnectL();
	return iTestStepResult;
	}

// Test case 1.6 
enum TVerdict CPsdAgtTest1_6::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(106);
	SetGPRSModeL(agtNotify);

	Log(_L("COMMDB changing GPRS_ANONYMOUS_ACCESS to True"));
	CDbGPRSOutgoingTable *gprs = CDbGPRSOutgoingTable::NewL();
	CleanupStack::PushL(gprs);
	gprs->SetColumnBoolL(TPtrC(GPRS_ANONYMOUS_ACCESS),ETrue);

	ConnectExpectErrorL(KErrNotSupported,EPsdStartingConfiguration);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));
	gprs->UndoDatabaseChangesL();
	CleanupStack::PopAndDestroy(gprs);
	return iTestStepResult;
	}

// Test case 1.7 
enum TVerdict CPsdAgtTest1_7::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(107);
	SetGPRSModeL(agtNotify);

	Log(_L("COMMDB changing GPRS_DATA_COMPRESSION to True"));
	CDbGPRSOutgoingTable *gprs = CDbGPRSOutgoingTable::NewL();
	CleanupStack::PushL(gprs);
	gprs->SetColumnBoolL(TPtrC(GPRS_DATA_COMPRESSION),ETrue);

	ConnectExpectErrorL(KErrNotSupported,EPsdStartingConfiguration);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));
	gprs->UndoDatabaseChangesL();
	CleanupStack::PopAndDestroy(gprs);

	return iTestStepResult;
	}

// Test case 1.8 
enum TVerdict CPsdAgtTest1_8::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(108);
	SetGPRSModeL(agtNotify);

	Log(_L("COMMDB changing GPRS_HEADER_COMPRESSION to True"));
	CDbGPRSOutgoingTable *gprs = CDbGPRSOutgoingTable::NewL();
	CleanupStack::PushL(gprs);
	gprs->SetColumnBoolL(TPtrC(GPRS_HEADER_COMPRESSION),ETrue);

	ConnectExpectErrorL(KErrNotSupported,EPsdStartingConfiguration);

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));
	gprs->UndoDatabaseChangesL();
	CleanupStack::PopAndDestroy(gprs);

	return iTestStepResult;
	}

enum TVerdict CPsdAgtTest1_9::doPsdAgtTestStepL( void )
	{
	// This test is really only here for code coverage purposes
	// It's difficult to actually test for any particular result
	// aside from a successful connection / disconnection
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(109);

	CNifAgentRefN1 *agent;

	// Construct a new Dummy Nifman Instance
	agent = CNifAgentRefN1::NewL(this,EFalse);
	CleanupStack::PushL(agent);

	agent->WaitForIncoming();

	DelayL(8000000);
	CleanupStack::PopAndDestroy(agent);
	
	return iTestStepResult;
	}


// Test case 1.10 
enum TVerdict CPsdAgtTest1_10::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(110);

	NormalConnectL();

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}


// Test case 1.11 
enum TVerdict CPsdAgtTest1_11::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(111);

	NormalConnectL();

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

// Test case 1.12 
enum TVerdict CPsdAgtTest1_12::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(112);

	NormalConnectL();

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

// Test case 1.13 
enum TVerdict CPsdAgtTest1_13::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(113);

	NormalConnectL();

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

// Test case 1.14 
enum TVerdict CPsdAgtTest1_14::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(114);

	NormalConnectL();

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}

// Test case 1.15 set GPRS use edge 
enum TVerdict CPsdAgtTest1_15::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(115);
	SetGPRSModeL(agtNotify);

	Log(_L("COMMDB changing GPRS_USE_EDGE to TRUE"));
	CDbGPRSOutgoingTable *gprs = CDbGPRSOutgoingTable::NewL();
	CleanupStack::PushL(gprs);
	gprs->SetColumnBoolL(TPtrC(GPRS_USE_EDGE),ETrue);

	NormalConnectL();

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));
	gprs->UndoDatabaseChangesL();
	CleanupStack::PopAndDestroy(gprs); 

	return iTestStepResult;
	}

// Test case 1.16 set use edge 
enum TVerdict CPsdAgtTest1_16::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(116);

	NormalConnectL();

	// Close database object, forcing it to undo all changes that were made
	Log(_L("Reverting COMMDB changes"));

	return iTestStepResult;
	}
