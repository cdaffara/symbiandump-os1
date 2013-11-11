// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <e32test.h>
#include <e32cmn.h>
#include "C32COMM.H"
#include "dummy.h"
#include "Te_C32MultiThreading.h"

//Test case id NET-C32-I-0029-HP
//Loading a CSY in which two CMI have identical WorkerID

TVerdict CTwoCMIsameWorkerID::doTestStepPreambleL( void )
	{
	return TestStepResult();
	}

TVerdict CTwoCMIsameWorkerID::doTestStepPostambleL( void )
	{

	return TestStepResult();
	}
/**
 * Constructor
 */
CTwoCMIsameWorkerID::CTwoCMIsameWorkerID()

	{
	SetTestStepName(_L("TwoCMIsameWorkerID"));
	}

/**
 * @see CTwoCMIsameWorkerID test case NET-C32-I-0029-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in Thread
 * close the connection to C32
 */
TVerdict CTwoCMIsameWorkerID::doTestStepL()
	{
//	User::After(10000000);
	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==========================================================================="));
	INFO_PRINTF1(_L("					Two CMI file having identical WorkerID					"));
	INFO_PRINTF1(_L("Test : THIS TEST CASE IS INTENDED TO PANIC C32 MULTI THREADING SERVER      "));
	INFO_PRINTF1(_L("==========================================================================="));

	RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret == KErrNotFound || ret == KErrServerTerminated)
		{
		INFO_PRINTF2(_L("Connect failed with %d Expected value"), ret);
		}
	else
		{
		INFO_PRINTF2(_L("Connect succeded ! not possible as server has died due to corrupt cmi"), ret);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

//Test case id NET-C32-I-0025-HP
//Loading a CSY which CMi file has missing CSY list parameter
//Mentioned CMI file in the testcase is available in the private dir
TVerdict CMissingCsyListinCMI::doTestStepPreambleL( void )
	{
	return TestStepResult();
	}

TVerdict CMissingCsyListinCMI::doTestStepPostambleL( void )
	{

	return TestStepResult();
	}
/**
 * Constructor
 */
CMissingCsyListinCMI::CMissingCsyListinCMI()

	{
	SetTestStepName(_L("MissingCsyListinCMI"));
	}

/**
 * @see CMissingCsyListinCMI test case NET-C32-I-0025-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading  csy in player thread
 * close the connection to C32
 */
TVerdict CMissingCsyListinCMI::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("================================================================================"));
	INFO_PRINTF1(_L("Loading csy in s player thread which has missing CSY list Parmeter in CMI file  "));
	INFO_PRINTF1(_L("Test : THIS TEST CASE IS INTENDED TO PANIC C32 MULTI THREADING SERVER      	 "));
	INFO_PRINTF1(_L("================================================================================"));

	RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret == KErrNotFound || ret == KErrServerTerminated)
		{
		INFO_PRINTF2(_L("Connect failed with %d Expected value"), ret);
		}
	else
		{
		INFO_PRINTF2(_L("Connect succeded ! not possible as server has died due to corrupt cmi"), ret);
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

// Test case id NET-C32-I-007-HP
// Loading a CSY  with broken CMI file Role parameter in the CMI file is missed.
TVerdict CTestMissingRoleParmCMI::doTestStepPreambleL( void )
	{
	return TestStepResult();
	}

TVerdict CTestMissingRoleParmCMI::doTestStepPostambleL( void )
	{

	return TestStepResult();
	}
/**
 * Constructor
 */
CTestMissingRoleParmCMI::CTestMissingRoleParmCMI()
	{
	SetTestStepName(_L("TestMissingRoleParmCMI"));
	}
/**
 * @see CTestMissingRoleParmCMI test case NET-C32-I-007-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading,CSY in which CMI file missing ROLE parameter
 * close the connection to C32
 */
TVerdict CTestMissingRoleParmCMI::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("==========================================================================================="));
	INFO_PRINTF1(_L(" New test case for C32V2: Loading a CSY in the player thread whose (Role Parameter) CMI file is Broken"));
	INFO_PRINTF1(_L(": 		        THIS TEST CASE IS INTENDED TO PANIC C32 MULTI THREADING SERVER      		"));
	INFO_PRINTF1(_L(": PANIC INFO: 2 Panics expected: Dealer thread's CMI file parsing when it sees problem; and"));
	INFO_PRINTF1(_L("              knock-on panic when auto-started USB C32 thread can't find Dealer on shutdown."));
	INFO_PRINTF1(_L("==========================================================================================="));

    RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret == KErrNotFound || ret == KErrServerTerminated)
		{
		INFO_PRINTF2(_L("Connect failed with %d Expected value"), ret);
		}
	else
		{
		INFO_PRINTF2(_L("Connect succeded ! not possible as server has died due to corrupt cmi"), ret);
		SetTestStepResult(EFail);
		}

    return TestStepResult();
	}


// Test case id NET-C32-I-008-HP
// Loading a CSY in which CMI file has missing WorkerID parameter.

TVerdict CTestMissingWorkerIDParm::doTestStepPreambleL( void )
	{
	return TestStepResult();
	}

TVerdict CTestMissingWorkerIDParm::doTestStepPostambleL( void )
	{

	return TestStepResult();
	}

/**
 * Constructor
 */
CTestMissingWorkerIDParm::CTestMissingWorkerIDParm()
	{
	SetTestStepName(_L("TestMissingWorkerIDParm"));
	}
/**
 * @see CTestMissingRWorkerIDParmCMI test case NET-C32-I-008-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Loading,CSY in which CMI file had missing Worker ID parameter
 * close the connection to C32
 */
	TVerdict CTestMissingWorkerIDParm::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("====================================================================================================================="));
	INFO_PRINTF1(_L("Test : New test case for C32V2: Loading a CSY in the player thread(Missing WorkerID parmeter)whose CMI file is Broken"));
	INFO_PRINTF1(_L("Test :      		THIS TEST CASE IS INTENDED TO PANIC C32 MULTI THREADING SERVER                                    "));
	INFO_PRINTF1(_L("====================================================================================================================="));

	INFO_PRINTF1(_L("Loading CSY27010 CSY in C32 component"));

	//Creating a RcommSession, connecting to the C32 server
    RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret == KErrNotFound || ret == KErrServerTerminated)
		{
		INFO_PRINTF2(_L("Connect failed with %d Expected value"), ret);
		}
	else
		{
		INFO_PRINTF2(_L("Connect succeded ! not possible as server has died due to corrupt cmi"), ret);
		SetTestStepResult(EFail);
		}

    return TestStepResult();
	}

//Test case id NET-C32-I-0026-HP
//Loading a CSY both Dealer and player thread, mentioned CMI files in tescase should be available in private dir
//new cmi files mentioned Role=Dealer and Role=Player

TVerdict CAllCMIFileRolePlayer::doTestStepPreambleL( void )
	{
	return TestStepResult();
	}

TVerdict CAllCMIFileRolePlayer::doTestStepPostambleL( void )
	{

	return TestStepResult();
	}

/**
 * Constructor
 */
CAllCMIFileRolePlayer::CAllCMIFileRolePlayer()

	{
	SetTestStepName(_L("AllCMIFileRolePlayer"));
	}

/**
 * @see CAllCMIFileRolePlayer test case NET-C32-I-0026-HP
 *
 * doTestStep function does the below action
 * Connect to a C32 server
 * Load the csy in player and Dealer thread
 * close the connection to C32
 */
TVerdict CAllCMIFileRolePlayer::doTestStepL()
	{

	SetTestStepResult(EPass);

	INFO_PRINTF1(_L("================================================================"));
	INFO_PRINTF1(_L("					All the CMI file had Role=Player             "));
	INFO_PRINTF1(_L("THIS TEST CASE IS INTENDED TO PANIC C32 MULTI THREADING SERVER  "));
	INFO_PRINTF1(_L("================================================================"));

	RCommServ commServ;
	TInt ret = commServ.Connect();
	if(ret == KErrNotFound || ret == KErrServerTerminated)
		{
		INFO_PRINTF2(_L("Connect failed with %d Expected value"), ret);
		}
	else
		{
		INFO_PRINTF2(_L("Connect succeded ! not possible as server has died due to corrupt cmi"), ret);
		SetTestStepResult(EFail);
		}

	return TestStepResult();

	}



