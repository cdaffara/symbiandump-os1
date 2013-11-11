// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32property.h>

const TUint32 KRestartExeCount = 38;

TInt MainL()
	{
	RDebug::Print(_L("appfwk-sysmon-0038: MainL"));
	
	TInt err = RProperty::Define(KTestSysMon, KRestartExeCount, RProperty::EInt);
    RDebug::Printf("Defining P&S key with key %d returns with err %d", err, KRestartExeCount);
	TInt runCount = 0;
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    runCount = CSysMonTestHelper::ReadRunCountL(args->Arg(0));
	CSysMonTestHelper::IncrementRunCountL(args->Arg(0));
    CleanupStack::PopAndDestroy(args);

    
    switch (runCount)
        {
		case 0:
			{
			RProcess::Rendezvous(KErrNone);
			RSysMonSession sysmon;
			sysmon.OpenL();
			CleanupClosePushL(sysmon);
			TTime time1();

			CStartupProperties* props = CStartupProperties::NewLC(_L("tsysmon_app_failcount2failwait_slave.exe"), _L("38"));
			props->SetMonitored(ETrue);
			props->SetStartupType(EStartProcess);
			props->SetStartMethod(EWaitForStart);
			props->SetNoOfRetries(1);
			props->SetTimeout(0);
			props->SetRecoveryParams(EIgnoreOnFailure, 0);

			RProcess slave1;
			slave1.Create(_L("tsysmon_app_failcount2failwait_slave.exe"), _L("APPFWK-SYSMON-0038"));
			CleanupClosePushL(slave1);
            TRequestStatus status;
            slave1.Rendezvous(status);
            slave1.Resume();
            User::WaitForRequest(status);

			// Register with SysMon
			sysmon.MonitorL(*props, slave1);
			slave1.Terminate(KErrNone);

			CleanupStack::PopAndDestroy(3, &sysmon);
			break;
			}
		default: //Run normally
			{
            RProcess::Rendezvous(KErrNone);
            break;
			}
        }
	RDebug::Print(_L("appfwk-sysmon-0038: End MainL"));
	return KErrNone;
	}


//  Global Functions

GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	RDebug::Print(_L("appfwk-sysmon-0038: E32Main"));
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
	RDebug::Print(_L("appfwk-sysmon-0038: End E32Main"));
	return KErrNone;
	} //lint -e714 Suppress 'not referenced'
