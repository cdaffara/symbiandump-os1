// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <startupproperties.h>

#include "startsafe.h"
#include "testapps.h"
#include "testprocgoodsession.h"
#include "tstartsafe_procmonsysrestart.h"



CAppFwkStartSafeTestStepProcMonSysRestart::CAppFwkStartSafeTestStepProcMonSysRestart()
	{
	SetTestStepName(KCTestCaseProcMonSysRestart);
	}

CAppFwkStartSafeTestStepProcMonSysRestart::~CAppFwkStartSafeTestStepProcMonSysRestart()
	{
	delete iNotifier;
	}

/**
Old Test CaseID 		APPFWK-STARTSAFE-0014
New Test CaseID 		DEVSRVS-SHMA-STARTSAFE-0014
 */

TVerdict CAppFwkStartSafeTestStepProcMonSysRestart::doTestStepL()
	{
	
#ifdef __WINSCW__
	ASSERT(0);
#endif	

	CActiveScheduler* sched = new(ELeave) CActiveScheduler; //lint !e527 Unreachable
	CleanupStack::PushL(sched); //lint !e527 Unreachable
	CActiveScheduler::Install(sched);	
	
	iNotifier = CSaveNotifier::NewL(*this);

	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );

	CStartupProperties* prop = CStartupProperties::NewLC();
	prop->SetFileParamsL(KTestProcGood, KNullDesC);	
	prop->SetStartMethod(EWaitForStart);
	prop->SetMonitored(ETrue);
	prop->SetRecoveryParams(ERestartOS, 0);
	
	TInt tried;
	RProcess proc;
	CleanupClosePushL(proc);
	TRAPD(err, startSafe->StartAndMonitorL(*prop, proc, tried));
	TEST(KErrNone == err);
	CleanupStack::Pop(&proc);
	CleanupStack::PopAndDestroy(prop);
	CleanupStack::PopAndDestroy(startSafe);
	INFO_PRINTF1(_L("Process monitoring setup with TRecoveryMethod=ERestartOS"));

	proc.Kill(KErrNone);
	proc.Close();
	
	TTime init;
	init.UniversalTime();
	INFO_PRINTF4( _L("Process killed at time %d:%d.%d"), init.DateTime().Minute(), init.DateTime().Second(), init.DateTime().MicroSecond());
	INFO_PRINTF1(_L("Starting scheduler (expecting an OS restart)"));
	CActiveScheduler::Start(); //Wait for MSaveObserver callback
	
	//resume here after callback has stopped the scheduler
	TESTE(iCallbackReceived, iCallbackReceived);
	if(iCallbackReceived)
		{
		const TTimeIntervalMicroSeconds elapsedTime = iShutdownPerformed.MicroSecondsFrom(init);
		const TDateTime dt = iShutdownPerformed.DateTime();
		const TInt64 value = elapsedTime.Int64();
		INFO_PRINTF5(_L("Shutdown callback received at time %d:%d.%d (after %d microseconds)"), dt.Minute(), dt.Second(), dt.MicroSecond(), value);
		TESTE((value >= 0) && (value < 1000000), value); // within 1 second	
		}
	else
		{
		INFO_PRINTF1( _L("Restart didn't take place :-("));
		TEST(EFalse);	
		}
	
	//abort restart
	iNotifier->HandleError(KErrNone); 	
	
	CleanupStack::PopAndDestroy(sched);
	return TestStepResult();	
	}

void CAppFwkStartSafeTestStepProcMonSysRestart::SaveL(TSaveType /*aSaveType*/)
	{ 
	INFO_PRINTF1(_L("Callback arrived"));
	
	// Do assertions in the doTestStepL function, just save values for now
	iCallbackReceived = ETrue;
	iShutdownPerformed.UniversalTime();	

	iNotifier->DelayRequeue(); 

	CActiveScheduler::Stop(); //resume doTestStepL
	}

