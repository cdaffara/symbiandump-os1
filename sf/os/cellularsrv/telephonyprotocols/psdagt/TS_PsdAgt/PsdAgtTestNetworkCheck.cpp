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
// This contains PsdAgt TestCase Test 7.1 - 7.11
// 
//

// EPOC includes
#include <e32base.h>
#include <e32std.h>
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

//Agent Errors
#include <agenterrors.h>

// EPsdStartingConfiguration
#include <psdprog.h>
//#include <psdpanic.h>

// Class definitions
#include "PsdAgtTestNetworkCheck.h"


// Test case 7.1
enum TVerdict CPsdAgtTest7_1::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(701);

	NormalConnectL();

	return iTestStepResult;
	}

// Test case 7.2
enum TVerdict CPsdAgtTest7_2::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(702);

	ConnectExpectErrorL(KErrGeneral, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 7.3
enum TVerdict CPsdAgtTest7_3::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(703);

	NormalConnectL();

	return iTestStepResult;
	}

// Test case 7.4
enum TVerdict CPsdAgtTest7_4::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(704);

	NormalConnectL();

	return iTestStepResult;
	}

// Test case 7.5
enum TVerdict CPsdAgtTest7_5::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(705);

	NormalConnectL();

	return iTestStepResult;
	}

// Test case 7.6
enum TVerdict CPsdAgtTest7_6::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(706);

	NormalConnectL();

	return iTestStepResult;
	}

// Test case 7.7
enum TVerdict CPsdAgtTest7_7::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(707);

	ConnectExpectErrorL(KErrAgentNoGPRSNetwork, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 7.8
enum TVerdict CPsdAgtTest7_8::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(708);

	ConnectExpectErrorL(KErrAgentNoGPRSNetwork, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 7.9
enum TVerdict CPsdAgtTest7_9::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(709);

	ConnectExpectErrorL(KErrAgentNoGPRSNetwork, EPsdStartingConfiguration);

	return iTestStepResult;
	}

// Test case 7.10
enum TVerdict CPsdAgtTest7_10::doPsdAgtTestStepL( void )
	{
	// This test is really only here for code coverage purposes
	// It's difficult to actually test for any particular result
	// aside from a successful connection / disconnection
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(710);

	NormalConnectL();

	return iTestStepResult;
	}

// Test case 7.11
enum TVerdict CPsdAgtTest7_11::doPsdAgtTestStepL( void )
	{
	TRealAgtNotify agtNotify; 
	agtNotify.SetTestCase(711);

	NormalConnectL();
	return iTestStepResult;
	}

