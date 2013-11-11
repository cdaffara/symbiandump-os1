// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#include <e32debug.h>
#include "t_wservcapchecksteptsizemode.h"

/************************************************************************/
/* This test step is designed test the result of a call to the API      */
/* CWsScreenDevice::SetCurrentScreenModeAttributes() by means of a      */
/* window server client. The client must have platsec capabilitity      */
/* "WriteDevideData" to call this API successfully.                     */
/* Test script graphics-wserv-capcheck-tsizemode.script creates a       */
/* window server application with defined capabilities, either "None"   */
/* or "WriteDeviceData". This test step is capable of running two types */
/* of test:                                                             */
/* 1. A positive test - client application has the correct permissions  */
/*    and the call produces no error                                    */
/* 2. A negative tests- client application doesn't have the permissions */
/*    and the call will panic                                           */
/* The window server application name and expected exit reason are read */
/* in from the ini file under the appropriate test step section         */
/*                                                                      */
/* The differences between running the positive and negative test are   */
/* 1. Different test process launched.                                  */
/* 2. Appropriate debug information is printed.                         */
/* 3. Appropriate exit reason is checked.                               */
/*                                                                      */
/************************************************************************/

/**
 CT_WServCapCheckStepTSizeMode constructor.
 @return N/A
 @pre None
 @post CT_WServCapCheckStepTSizeMode 1st stage constructor
*/
CT_WServCapCheckStepTSizeMode::CT_WServCapCheckStepTSizeMode()
	{
	}

enum TVerdict CT_WServCapCheckStepTSizeMode::doTestStepPostambleL()
	{
	return TestStepResult();
	}

enum TVerdict CT_WServCapCheckStepTSizeMode::doTestStepL()
	{
	return TestStepResult();
	}

/**
Starts test step
@internalComponent
@return TVerdict pass / fail
@pre N/A
@post N/A
*/
enum TVerdict CT_WServCapCheckStepTSizeMode::runTestCaseL(const TDesC& aExecutable, const TInt& aExitReason)
	{
	__UHEAP_MARK; // mark the heap state

	//Launch window server client process read fron ini file
	RProcess process;
	User::LeaveIfError(process.Create(aExecutable, KNullDesC));
	process.SetPriority(EPriorityHigh);
	TEST(process.Priority() == EPriorityHigh);
	INFO_PRINTF3(_L("Process Priority: Actual: %d, Expected: %d"), process.Priority(), EPriorityHigh);

	//Wait for the client process to terminate
	TRequestStatus status;
	process.Logon(status);
	process.Resume();
	User::WaitForRequest(status);

	//Check the window server client exit reason
	INFO_PRINTF3(_L("Process Check: Actual: %d, Expected: %d"), process.ExitReason(), aExitReason);
	TEST(process.ExitReason() == aExitReason);

	__UHEAP_MARKEND; // check for no memory leak

	return TestStepResult();
	}

/**
 CT_WServCapCheckStepTSizeModePos constructor.
 @return N/A
 @pre None
 @post CT_WServCapCheckStepTSizeModePos 1st stage constructor
*/
CT_WServCapCheckStepTSizeModePos::CT_WServCapCheckStepTSizeModePos()
	{
	SetTestStepName(KT_WServCapCheckStepTSizeModePos);
	}

enum TVerdict CT_WServCapCheckStepTSizeModePos::doTestStepL()
	{
	//Print out the test step name in epocwind.out
	RDebug::Print(_L("******************************************************************"));
	RDebug::Print(_L("*** Running Test Step %S"), &KT_WServCapCheckStepTSizeModePos);
	RDebug::Print(_L("******************************************************************"));

	TVerdict verdict = runTestCaseL(KExecutablePos, EWservNoPanic);

	//Print out the test step name in epocwind.out
	RDebug::Print(_L("******************************************************************"));
	RDebug::Print(_L("*** Finished Test Step %S"), &KT_WServCapCheckStepTSizeModePos);
	RDebug::Print(_L("******************************************************************"));

	return verdict;
	}

/**
 CT_WServCapCheckStepTSizeModeNeg constructor.
 @return N/A
 @pre None
 @post CT_WServCapCheckStepTSizeModeNeg 1st stage constructor
*/
CT_WServCapCheckStepTSizeModeNeg::CT_WServCapCheckStepTSizeModeNeg()
	{
	SetTestStepName(KT_WServCapCheckStepTSizeModeNeg);
	}

enum TVerdict CT_WServCapCheckStepTSizeModeNeg::doTestStepL()
	{
	//Print out the test step name in epocwind.out
	RDebug::Print(_L("******************************************************************"));
	RDebug::Print(_L("*** Running Test Step %S"), &KT_WServCapCheckStepTSizeModeNeg);
	RDebug::Print(_L("******************************************************************"));

	RDebug::Print(_L("******NEGATIVE PLATSEC TEST START******"));
	TVerdict verdict = runTestCaseL(KExecutableNeg, EWservPanicPermissionDenied);
	RDebug::Print(_L("******NEGATIVE PLATSEC TEST FINISH*****"));

	//Print out the test step name in epocwind.out
	RDebug::Print(_L("******************************************************************"));
	RDebug::Print(_L("*** Finished Test Step %S"), &KT_WServCapCheckStepTSizeModeNeg);
	RDebug::Print(_L("******************************************************************"));

	return verdict;
	}
