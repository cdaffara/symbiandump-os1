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
#include "tstartsafe_procfailsysrestart.h"

CAppFwkStartSafeTestStepProcFailSysRestart::CAppFwkStartSafeTestStepProcFailSysRestart()
	{
	SetTestStepName(KCTestCaseProcFailSysRestart);
	}

CAppFwkStartSafeTestStepProcFailSysRestart::~CAppFwkStartSafeTestStepProcFailSysRestart()
	{
	delete iNotifier;
	}	

static TInt StartApplication(TAny* aSelf)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TInt r=KErrNoMemory;
	if (cleanup)
		{
		CAppFwkStartSafeTestStepProcFailSysRestart* self = (CAppFwkStartSafeTestStepProcFailSysRestart*)aSelf;
		TRAP(r, self->DoStartApplicationL());
		delete cleanup;
		}
	return r;
	}

/**
Old Test CaseID 		APPFWK-STARTSAFE-0013
New Test CaseID 		DEVSRVS-SHMA-STARTSAFE-0013
 */

TVerdict CAppFwkStartSafeTestStepProcFailSysRestart::doTestStepL()
	{

#ifdef __WINSCW__
	ASSERT(0);
#endif	

	//create scheduler
	//create savenotifier
	CActiveScheduler* sched = new(ELeave) CActiveScheduler; //lint !e527 Unreachable
	CleanupStack::PushL(sched); //lint !e527 Unreachable
	CActiveScheduler::Install(sched);
	iNotifier = CSaveNotifier::NewL(*this);

	//Create a new thread and resume it
	RThread thread;
	CleanupClosePushL(thread);
	User::LeaveIfError(thread.Create(_L("StarterThread"), StartApplication, KDefaultStackSize, NULL, this));
	TRequestStatus stat;	
	thread.Rendezvous(stat);
	if (stat!=KRequestPending)
		{
		thread.Kill(0);	// abort startup
		TEST(EFalse);	// fail test
		return TestStepResult();
		}
	else
		{
		thread.Resume();	// logon OK - run DoStartApplicationL	
		}
	User::WaitForRequest(stat);	// wait for rendevouz
	//Some sanity checks to verify our new thread is healthy
	TInt r=(thread.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
	CleanupStack::PopAndDestroy(&thread); //only this handle
	User::LeaveIfError(r);

	iShutdownInitiated.UniversalTime();
	INFO_PRINTF1( _L("Attempt to start the failing Test Application will cause an OS Restart") );
	INFO_PRINTF4( _L("StartL called at time %d:%d.%d"), iShutdownInitiated.DateTime().Minute(), iShutdownInitiated.DateTime().Second(), iShutdownInitiated.DateTime().MicroSecond());
	INFO_PRINTF1(_L("Starting scheduler"));
	CActiveScheduler::Start(); //Wait for MSaveObserver callback
		
	//resume here after callback has stopped the scheduler
	INFO_PRINTF2( _L("Asserting that the shutdown was at all initiated: %d"), iCallbackReceived );
	TESTE(iCallbackReceived, iCallbackReceived);
	
	//assert the thread calling StartL is still blocked
	TESTE(KErrNone == iStartSafeError, iStartSafeError);
	
	if(iCallbackReceived)
		{
		const TTimeIntervalMicroSeconds elapsedTime = iShutdownPerformed.MicroSecondsFrom(iShutdownInitiated);
		const TDateTime dt = iShutdownPerformed.DateTime();
		const TInt64 value = elapsedTime.Int64();
		INFO_PRINTF5(_L("Shutdown callback received at time %d:%d.%d (after %d microseconds)"), dt.Minute(), dt.Second(), dt.MicroSecond(), value);
		TESTE((value >= 0) && (value < 1000000), value); // within 1 second	
		// Now call iNotifier->HandleError() to abort the shutdown.
		INFO_PRINTF1(_L("Aborting shutdown sequence"));
		iNotifier->HandleError(KErrNone);
		CleanupStack::PopAndDestroy(sched);	
		}
	else
		{
		TEST(EFalse);	
		}
	
	return TestStepResult();	
	}



void CAppFwkStartSafeTestStepProcFailSysRestart::DoStartApplicationL()
	{	
	// warning: the Tef framework can't do logging from this thread
	CStartSafe* startSafe = CStartSafe::NewL();	
	CleanupStack::PushL( startSafe );
	
	CStartupProperties* prop = CStartupProperties::NewLC(KTestAppNoRv, KNullDesC);
	prop->SetStartupType( EStartApp );
	prop->SetStartMethod( EWaitForStart );
	prop->SetTimeout( 1 );
	prop->SetNoOfRetries( 0 );
	prop->SetRecoveryParams( ERestartOS, 0 );
	
	TInt tried = 0;
	RProcess proc;
	// We are expecting StartL to leave because KTestAppNoRv will fail to start, so any items 
	// on the CleanupStack would be deleted/closed on this call. 
	CleanupStack::Pop(2, startSafe);
	RThread::Rendezvous(KErrNone);
	TRAP( iStartSafeError, startSafe->StartL(*prop, proc, tried) ); // Will initiate ERestartOS and then freeze this thread for 5 seconds	
	
	proc.Close();
	delete prop;
	delete startSafe;
	}
	
/**
Called by the shutdown server in syslib/pwrcli when device is initiating a shut down.
*/
void CAppFwkStartSafeTestStepProcFailSysRestart::SaveL(TSaveType /*aSaveType*/)
	{
	// Do assertions in the doTestStepL function, just save values for now
	iCallbackReceived = ETrue;
	iShutdownPerformed.UniversalTime();	

	iNotifier->DelayRequeue();
	CActiveScheduler::Stop();
	}
