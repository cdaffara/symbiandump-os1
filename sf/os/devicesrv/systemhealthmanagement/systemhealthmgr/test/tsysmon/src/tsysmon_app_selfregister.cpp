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
	
	RSysMonSession sysmon;
	CleanupClosePushL(sysmon);
	
	TRAPD(err, sysmon.OpenL());
	// If we fail to connect, pass the error code out to the test step and then re-leave
	helper->TestErrorcodeL(err); 
	
	CStartupProperties* props = CStartupProperties::NewLC(_L("tsysmon_app_selfregister.exe"), KNullDesC);
	props->SetMonitored(ETrue);
	props->SetStartupType(EStartProcess);
	props->SetStartMethod(EWaitForStart);
	props->SetNoOfRetries(1);
	props->SetTimeout(1000);
	props->SetRecoveryParams(recoveryMethod, recoveryMode);

	// Register with SysMon
	TRAP(err, sysmon.MonitorSelfL(*props));
	
	CleanupStack::PopAndDestroy(props);

	helper->WriteResultL(err);

	TRAP(err, sysmon.CancelMonitorSelfL());
	helper->WriteDeregResultL(err);
	
	CleanupStack::PopAndDestroy(2, helper);
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
	} //lint -e714 Suppress 'not referenced'
