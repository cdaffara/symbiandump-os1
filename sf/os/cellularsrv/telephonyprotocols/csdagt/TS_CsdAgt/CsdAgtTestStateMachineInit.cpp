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
// This contains CsdAgt TestCase Test 2.1 - 2.15
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
#include "CsdAgtTestStateMachineInit.h"

/**
 * Test case 2.1 CSD connect when ETel server LoadPhoneModule() fails
 *
 */
enum TVerdict CCsdAgtTest2_1::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10201);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrGeneral,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.2 CSD connect when ETel server EnumeratePhones() fails
 *
 */
enum TVerdict CCsdAgtTest2_2::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10202);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrNotFound,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.3 CSD connect when ETel server GetTsyName() fails
 *
 */
enum TVerdict CCsdAgtTest2_3::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10203);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrGeneral,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.4, CSD connect when ETel server GetTsyName() returns different name
 *
 */
enum TVerdict CCsdAgtTest2_4::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10204);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrNotFound,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.5 CSD connect when ETel server GetTsyName() contains . (dot)
 *
 */
enum TVerdict CCsdAgtTest2_5::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10205);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrNotFound,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.6 CSD connect when ETel server GetPhoneInfo() fails
 *
 */
enum TVerdict CCsdAgtTest2_6::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10206);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrNotSupported,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.7 CSD connect when ETel server OpenNewCall() fails
 *
 */
enum TVerdict CCsdAgtTest2_7::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10207);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrGeneral,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.8 CSD connect when ETel server GetMobileDataCallCaps() fails
 *
 */
enum TVerdict CCsdAgtTest2_8::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10208);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrGeneral,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.9 CSD connect when ETel server Dial() fails
 *
 */
enum TVerdict CCsdAgtTest2_9::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10209);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrGeneral,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.10 CSD connect when ETel server Dial() fails for HSCSD call
 *
 */
enum TVerdict CCsdAgtTest2_10::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10210);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrNotSupported,ECsdFinishedDialling);

	return iTestStepResult;
	}

/**
 * Test case 2.11 CSD connect when ETel server LoanDataPort() fails
 *
 */
enum TVerdict CCsdAgtTest2_11::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10211);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrGeneral,ECsdScannedScript);

	return iTestStepResult;
	}

/**
 * Test case 2.12 CSD connect when ETel server RecoverDataPort() fails
 *
 */
enum TVerdict CCsdAgtTest2_12::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10212);
	SetGSMModeL(AgtNotify);

	NormalConnectL();

	return iTestStepResult;
	}

/**
 * Test case 2.13 CSD connect when ETel server HangUp() fails
 *
 */
enum TVerdict CCsdAgtTest2_13::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10213);
	SetGSMModeL(AgtNotify);

	NormalConnectL();

	return iTestStepResult;
	}

/**
 * Test case 2.14 CSD connect when ETel server HangUp() succeeds
 *
 */
enum TVerdict CCsdAgtTest2_14::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10214);
	SetGSMModeL(AgtNotify);

	NormalConnectL();

	return iTestStepResult;
	}


/**
 * Test case 2.15 CSD connect when a call is already in progress
 *
 */
enum TVerdict CCsdAgtTest2_15::doCsdAgtTestStepL( void )
	{
	TRealAgtNotify AgtNotify;

	AgtNotify.SetTestCase(10215);
	SetGSMModeL(AgtNotify);

	ConnectExpectErrorL(KErrAccessDenied,ECsdFinishedDialling);

	return iTestStepResult;
	}
