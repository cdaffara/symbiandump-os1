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

#include <e32property.h>
#include "sysmontesthelper.h"
const TUint32 KRestartExeCount = 34;

TInt MainL()
	{
	RDebug::Print(_L("appfwk-sysmon-0034: MainL"));

	TInt runCount = 0;
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    TInt err = RProperty::Define(KTestSysMon, KRestartExeCount, RProperty::EInt);
    RDebug::Printf("Defining P&S key with key %d returns with err %d", err, KRestartExeCount);
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

			CStartupProperties* props = CStartupProperties::NewLC(_L("tsysmon_app_dontrendezvous.exe"), _L("34"));
			props->SetMonitored(ETrue);
			props->SetStartupType(EStartProcess);
			props->SetStartMethod(EWaitForStart);
			props->SetNoOfRetries(1);
			props->SetTimeout(0);
			props->SetRecoveryParams(EIgnoreOnFailure, 0);
			
			RProcess slave1;
			slave1.Create(_L("tsysmon_app_dontrendezvous.exe"), _L("5000"));
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
    
	
	RDebug::Print(_L("appfwk-sysmon-0034: End MainL"));
	return KErrNone;
	}


//  Global Functions

GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	RDebug::Print(_L("appfwk-sysmon-0034: E32Main"));
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
	RDebug::Print(_L("appfwk-sysmon-0034: End E32Main"));
	return KErrNone;
	} //lint -e714 Suppress 'not referenced'
