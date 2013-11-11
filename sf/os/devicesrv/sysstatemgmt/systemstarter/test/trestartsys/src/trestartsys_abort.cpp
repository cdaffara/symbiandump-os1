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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "restartsys.h"
#include "trestartsys_abort.h"
#include "tpowerdownclient.h"

CRestartSysAbort::CRestartSysAbort(const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	}

CRestartSysAbort::~CRestartSysAbort()
	{
	delete iNotifier;
	}

/**
Old Test CaseID 		APPFWK-RESTARTSYS-0001
New Test CaseID 		DEVSRVS-SYSSTART-RESTARTSYS-0001
 */

TVerdict CRestartSysAbort::doTestStepL()
	{
 	INFO_PRINTF1(_L("APPFWK-RESTARTSYS-0001"));

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	iNotifier = CSaveNotifier::NewL(*this);

	iShutdownInitiated.UniversalTime();
	TDateTime dt = iShutdownInitiated.DateTime();
	INFO_PRINTF4(_L("RestartSys::RestartSystem() called at %d:%d.%d"), dt.Minute(), dt.Second(), dt.MicroSecond());
	TESTL(KErrNone == RestartSys::RestartSystem()); // leaving TESTL to avoid starting the ActiveScheduler on error

	// Now expect CRestartTestStepPreReboot::SaveL() to be called to indicate restart event was sent successfully
	CActiveScheduler::Start(); 

	// Now call CPowerdownClient::QueryServerPowerState() which will in turn call the method 'ServerPowerState()'
	// to stop the shutdown. This function call will cause the shutdown server in syslibs/pwrcli to cancel power off.
	INFO_PRINTF1(_L("Aborting shutdown."));
	TBool powerOff = EFalse;
	CPowerdownClient* powerdownclient = CPowerdownClient::NewL();
	CleanupStack::PushL(powerdownclient);
	TEST(KErrNone == powerdownclient->QueryServerPowerState(iNotifier,powerOff));
	TEST(powerOff);
	CleanupStack::PopAndDestroy(powerdownclient);
	CleanupStack::PopAndDestroy(sched);
	
	//Assert the callback has arrived (this test should be redundant)	
	TESTE(iShutdownPerformed, iShutdownPerformed);
		
	return TestStepResult();
	}

/**
Called by the shutdown server in syslib/pwrcli when device is initiating a shut down.
*/
void CRestartSysAbort::SaveL(TSaveType aSaveType)
	{
	iShutdownPerformed = ETrue;
	
	//Make sure the shutdown was initiated close in time to our request 
	TTime now;
	now.UniversalTime();
	const TTimeIntervalMicroSeconds elapsedTime = now.MicroSecondsFrom(iShutdownInitiated);
	const TDateTime dt = now.DateTime();
	const TInt64 value = elapsedTime.Int64();
	
	INFO_PRINTF5(_L("Shutdown callback occured at time %d:%d.%d (after %d microseconds)"), dt.Minute(), dt.Second(), dt.MicroSecond(), value);
	TEST((value >= 0) && (value < 1000000)); // within 1 second

	INFO_PRINTF1(_L("Asserting expected paramter aSaveType."));
	TESTE(aSaveType == ESaveAll, aSaveType);// ESaveAll is expected from laf shutdown server.
											// If anything else comes here, we should investigate what changes
											// has been done in the shutdownserver and probably add some new tests.
	iNotifier->DelayRequeue();
	CActiveScheduler::Stop();
	}
