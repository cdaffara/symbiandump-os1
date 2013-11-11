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
// trestartsys_stepwithmode.cpp
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include <hal.h>
#include <e32uid.h>
#include <e32property.h>
#include <s32file.h>
#include "restartsys.h"
#include "trestartsys_withmode.h"

_LIT(KDataFile, "c:\\trestartsys.dat");

CRestartSysWithMode::CRestartSysWithMode(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

CRestartSysWithMode::~CRestartSysWithMode()
	{
	delete iScheduler;
	delete iNotifier;
	}

TVerdict CRestartSysWithMode::doTestStepPreambleL()
	{ 	
	// Assert that the test was initialised with a restartmode supported by the hardware
	TInt KMaxStartupModeSupportedByHardware = 0;
	HAL::Get(HALData::EMaximumRestartStartupModes, KMaxStartupModeSupportedByHardware);
	TESTE(KNonDefaultBootMode <= KMaxStartupModeSupportedByHardware, KMaxStartupModeSupportedByHardware);

	return TestStepResult();
	}

/**
Old Test CaseID 		APPFWK-RESTARTSYS-0002
New Test CaseID 		DEVSRVS-SYSSTART-RESTARTSYS-0002
 */

TVerdict CRestartSysWithMode::doTestStepL()
	{
	INFO_PRINTF1(_L("APPFWK-RESTARTSYS-0002 ---- Restarting system in a specified mode."));
	
	TInt usedBootMode;
	TEST(KErrNone == RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, usedBootMode));
	
	switch(usedBootMode)
		{
		case KDefaultBootMode:
			{
			// Test part 1 (prereboot)
			InitRebootL();
			SaveVariablesToFileL();
			AssertRestartIsPreemptiveL();
			INFO_PRINTF1(_L("End of part 1"));
			break;
			}
		case KNonDefaultBootMode:
		    {
		    // Test part 2 (postreboot)
		    INFO_PRINTF2(_L("Test PASS - Bootmode used for this startup was: %d"), usedBootMode);		    
		    AssertExpectationsL(); 
			break;	
		    }
		default:
			{
			ERR_PRINTF3(_L("Used bootMode = %d, but we were expecting: %d"), usedBootMode, KNonDefaultBootMode);	
			TEST(EFalse); // fail test-step
			break;
			}	
		}
		
	return TestStepResult();
	}

/**
Initiate a restart in KNonDefaultBootMode
*/	
void CRestartSysWithMode::InitRebootL()
	{	
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	iNotifier = CSaveNotifier::NewL(*this);

	// Initiate shutdown sequence
	INFO_PRINTF1(_L("Requesting restart"));
	iShutdownInitiated.UniversalTime();
	TESTL(KErrNone == RestartSys::RestartSystem(KNonDefaultBootMode)); // leaving TESTL to avoid starting the ActiveScheduler on error

	// Now expect CRestartTestStepPreReboot::SaveL() to be called to indicate restart event was sent successfully
	CActiveScheduler::Start(); 	
	}

/** 
Log results from this pre-reboot part of the test to a temporary file.
Because the Tef-logfile will be overwritten when this test is re-run after the reboot,
there is no point in logging to Tef before we reach the post-reboot phase.  
*/
void CRestartSysWithMode::SaveVariablesToFileL()
	{
	INFO_PRINTF1(_L("Saving variables to file."));
	RFs fs;
	User::LeaveIfError(fs.Connect());	
	CleanupClosePushL(fs);
	RFileWriteStream writeStream;
	CleanupClosePushL(writeStream);
	User::LeaveIfError(writeStream.Replace(fs, KDataFile, EFileWrite));	  	
	writeStream.WriteInt32L(iShutdownInitiated.Int64());
	writeStream.WriteInt32L(iShutdownPerformed.Int64());
	writeStream.WriteInt8L(iSaveType);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(&fs);		
	}

/**
If the restart-sequence works as is should, a forced restart will now take place
aborting this test while our thread is suspended by the User::After call. 

But a defect in the initial design of the GT0328 changes to the shutdown-server/client 
made it possible for a shutdownserver-client to indefinitely delay a restart. 

This function will try to delay the restart for 5 seconds and then update the temporary
file with a new shutdown time.
 */
void CRestartSysWithMode::AssertRestartIsPreemptiveL()
	{
	INFO_PRINTF1(_L("Going to sleep for 5 seconds, waiting for restart..."));
	User::After(5000000);
#ifndef SYSLIBS_TEST
	INFO_PRINTF1(_L("Woke up, no restart took place. Fail the test..."));
	TEST(EFalse);
#else
	WARN_PRINTF1(_L("SYSLIBS_TEST Macro is defined, so restart is not possible."));
	SetTestStepResult(EPass); //Does not shutdown when SYSLIBS_TEST is defined, we have to look into this later when syslibs/pwrcli will be moved to AFSS.
#endif
	//Log new shutdown-time
	INFO_PRINTF1(_L("Update the logged time for shutdown."));
	iShutdownPerformed.UniversalTime();
	SaveVariablesToFileL();
	
	INFO_PRINTF1(_L("Requeue shutdown-server client."));
	ASSERT(iNotifier);
	iNotifier->Queue(); //This will cause the restart-sequence to continue
	}

/**
Asserts expectations against details saved in a logfile prior to the restart.
*/
void CRestartSysWithMode::AssertExpectationsL()
	{
	RFs fs;	
	User::LeaveIfError(fs.Connect());	
	CleanupClosePushL(fs);
	RFileReadStream readStream;
	CleanupClosePushL(readStream);
	INFO_PRINTF1(_L("Will open the datafile containg the pre-reboot results."));
	const TInt err = readStream.Open(fs,KDataFile, EFileRead);
	TESTE(KErrNone == err, err); //fail test fs logfile is missing
	if(KErrNone == err)
		{
		iShutdownInitiated = readStream.ReadInt32L();
		iShutdownPerformed = readStream.ReadInt32L();
		iSaveType = (TSaveType)readStream.ReadInt8L();
		
		// Log when test was initiated
		TDateTime init = iShutdownInitiated.DateTime();
		INFO_PRINTF5(_L("Shutdown sequence using RestartSys::RestartSystem(%d) was initiated at time %d:%d.%d"), KNonDefaultBootMode, init.Minute(), init.Second(), init.MicroSecond());

		// Log when shutdown notification was received
		const TDateTime exec = iShutdownPerformed.DateTime();
		const TTimeIntervalMicroSeconds elapsedTime = iShutdownPerformed.MicroSecondsFrom(iShutdownInitiated);
		const TInt64 execValue = elapsedTime.Int64();
		//Make sure the shutdown was initiated close in time to our request
		INFO_PRINTF1(_L("Asserting that our shutdownserver client failed to delay the restart for 5 seconds.")); // see AssertRestartIsPreemptiveL
		TEST((execValue >= 0) && (execValue < 1000000)); // within 1 second
		INFO_PRINTF5(_L("Shutdown callback occured at time %d:%d.%d (after %d microseconds)"), exec.Minute(), exec.Second(), exec.MicroSecond(), execValue);

		TESTE(iSaveType == ESaveAll, iSaveType);// ESaveAll is expected from laf shutdown server
		}
	CleanupStack::PopAndDestroy(&readStream);
	TEST(KErrNone == fs.Delete(KDataFile));
	CleanupStack::PopAndDestroy(&fs);
	}

/**
 Called by the shutdown server in syslib/pwrcli when device is initiating a shut down.
 Logs the aSaveType parameter and the time the restart notification arrives.
*/
void CRestartSysWithMode::SaveL(TSaveType aSaveType)
	{
	iSaveType = aSaveType;
	iShutdownPerformed.UniversalTime();

	//Make sure the shutdown was initiated close in time to our request 
	const TTimeIntervalMicroSeconds elapsedTime = iShutdownPerformed.MicroSecondsFrom(iShutdownInitiated);
	const TDateTime now = iShutdownPerformed.DateTime();
	const TInt64 value = elapsedTime.Int64();
	
	INFO_PRINTF5(_L("Shutdown callback occured at time %d:%d.%d (after %d microseconds)"), now.Minute(), now.Second(), now.MicroSecond(), value);
	
	iNotifier->DelayRequeue();
	CActiveScheduler::Stop();
	}
