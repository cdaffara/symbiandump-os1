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


TInt MainL ()
	{
	CSysMonTestHelper* helper = CSysMonTestHelper::NewLC();

	RSysMonSession sysmon;
	CleanupClosePushL(sysmon);

	TRAPD(err, sysmon.OpenL());
	// If we fail to connect, pass the error code out to the test step and then re-leave
	helper->TestErrorcodeL(err); 

	TBuf<KMaxTestIdLength> testId;
	helper->GetTestId(testId);
	
	TBuf<KMaxTestIdLength + 5> args;	
	args.Append(testId);
	args.Append(_L(" "));
	args.Append(_L("300")); //The length of time, in milliseconds, for the process to wait before deregistering
	
	CStartupProperties* props = CStartupProperties::NewLC(KFilenameDeregTimeout, KNullDesC);
	props->SetMonitored(ETrue);
	props->SetStartupType(EStartProcess);
	props->SetStartMethod(EWaitForStart);
	props->SetNoOfRetries(1);
	props->SetTimeout(1000);
	props->SetRecoveryParams(EIgnoreOnFailure, 0);
	
	RProcess slave;
	CleanupClosePushL(slave);
	err = slave.Create(KFilenameDeregTimeout, args);
	// If we fail to create the process, pass the error code out to the test step and then leave
	helper->TestErrorcodeL(err); 
	slave.Resume();

	// Register with SysMon
	TRAP(err, sysmon.MonitorL(*props, slave));
	// If we fail to register, pass the error code out to the test step and then re-leave
	helper->TestErrorcodeL(err);
	
	TRAP(err, sysmon.MonitorL(*props, slave));
	helper->WriteResultL(err);
	
	for (int i = 0; slave.ExitType() == EExitPending && i < 100; i++ )
	// Wait for the process to end, or up to 1 second
		{
		User::After(10000);
		}
	
	if (slave.ExitType() == EExitPending)
		{
		slave.Terminate(KErrGeneral);
		}

	CleanupStack::PopAndDestroy(4, helper);
	return KErrNone;
	}


//  Global Functions

TInt E32Main()
	{
	// Create cleanup stack
	RDebug::Print(_L("slaveprocess: E32Main"));
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
