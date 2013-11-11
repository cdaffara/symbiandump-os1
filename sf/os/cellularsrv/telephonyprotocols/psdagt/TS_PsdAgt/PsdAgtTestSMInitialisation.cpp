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
// This contains PsdAgt TestCase Test 6.1 - 6.12
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
#include "PsdAgtTestSMInitialisation.h"


// Test case 6.1
enum TVerdict CPsdAgtTest6_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(601);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}


// Test case 6.2
enum TVerdict CPsdAgtTest6_2::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(602);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}


// Test case 6.3
enum TVerdict CPsdAgtTest6_3::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(603);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}


// Test case 6.4
enum TVerdict CPsdAgtTest6_4::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(604);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 6.5
enum TVerdict CPsdAgtTest6_5::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(605);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 6.6
enum TVerdict CPsdAgtTest6_6::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(606);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrNotFound, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 6.7
enum TVerdict CPsdAgtTest6_7::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(607);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 6.8
enum TVerdict CPsdAgtTest6_8::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(608);
	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrNotFound, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 6.9
enum TVerdict CPsdAgtTest6_9::doPsdAgtTestStepL( void )
	{
	_LIT(KTsyName,"NAME.TSY");

	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(609);
	Log(_L("COMMDB changing MODEM_TSY_NAME to NAME.TSY"));
	CDbModemTable *modem = CDbModemTable::NewL();
	CleanupStack::PushL(modem);
	modem->SetColumnTextL(TPtrC(MODEM_TSY_NAME),KTsyName);

	// We expect DummyEtel to simulate an error during connection
	ConnectExpectErrorL(KErrNotFound, KStartingSelection);

	Log(_L("Reverting COMMDB changes"));
	modem->UndoDatabaseChangesL();
	CleanupStack::PopAndDestroy(modem);

	return iTestStepResult;
	}

// Test case 6.10
enum TVerdict CPsdAgtTest6_10::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(610);
	// We expect the connection attempt to fail
	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}


// Test case 6.11
enum TVerdict CPsdAgtTest6_11::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(611);
	// We expect the connection attempt to be successful
	NormalConnectL();

	return iTestStepResult;
	}


// Test case 6.12
enum TVerdict CPsdAgtTest6_12::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify;
	agtNotify.SetTestCase(612);
	// We expect the connection attempt to be successful
	NormalConnectL();

	return iTestStepResult;
	}


