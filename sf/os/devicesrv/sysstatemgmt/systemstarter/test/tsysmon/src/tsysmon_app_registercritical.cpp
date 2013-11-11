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

#include "sysmontesthelper.h"

TInt MainL()
	{
	CSysMonTestHelper* helper = CSysMonTestHelper::NewLC();
	
	TRecoveryMethod recoveryMethod = EIgnoreOnFailure;
	TInt recoveryMode = 0;
	helper->GetRecoveryMethodArgsL(recoveryMethod, recoveryMode);

	TBuf<KMaxTestIdLength> testId;
	helper->GetTestId(testId);
	
	RSemaphore sem;
	sem.CreateGlobal(_L("TSysMon_Critical"),0);
	CleanupClosePushL(sem);
	
	RProcess slave;
	CleanupClosePushL(slave);
	
	_LIT(KFilenameCritical, "tsysmon_app_critical.exe");
	TInt err = slave.Create(KFilenameCritical, KNullDesC);
	// If we fail to create the process, pass the error code out to the test step and then leave
	helper->TestErrorcodeL(err);
	
	TRequestStatus myRendezvousStatus;
	slave.Rendezvous(myRendezvousStatus);
	slave.Resume();
	

	CStartupProperties* props = CStartupProperties::NewLC(KFilenameCritical, KNullDesC);
	props->SetMonitored(ETrue);
	props->SetStartupType(EStartProcess);
	props->SetStartMethod(EWaitForStart);
	props->SetNoOfRetries(1);
	props->SetTimeout(1000);
	props->SetRecoveryParams(recoveryMethod, recoveryMode);
	
	RSysMonSession sysmon;
	CleanupClosePushL(sysmon);
	
	TRAP(err, sysmon.OpenL());
	// If we fail to connect, pass the error code out to the test step and then re-leave
	helper->TestErrorcodeL(err); 

	User::WaitForRequest(myRendezvousStatus);
	if(myRendezvousStatus==KErrNone)
		{
		// Register with SysMon
		TRAP(err, sysmon.MonitorL(*props, slave));

		helper->WriteResultL(err);
		sem.Signal();
		sem.Wait();
		}
	else
		{
		helper->WriteResultL(myRendezvousStatus.Int());
		}
		
	CleanupStack::PopAndDestroy(2,props);	
	
	if (slave.ExitType() == EExitPending)
		{
		slave.Terminate(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(3, helper);
	return KErrNone;
	}


//  Global Functions

TInt E32Main()
	{
	// Create cleanup stack
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	// Run application code inside TRAP harness
	TRAPD(testError, MainL());
	if (testError)
		{
		User::Panic(_L("Test failure"), testError);
		}

	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}
