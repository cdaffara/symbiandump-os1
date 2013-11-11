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

#include <hal.h>
#include <hal_data.h>


LOCAL_C TInt MainL()
    {
	RProcess delayer;
	delayer.Create(_L("tsysmon_app_delayshutdown.exe"),KNullDesC);
	delayer.Resume();
	delayer.Close();
	
    TInt runCount = 0;
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    runCount = CSysMonTestHelper::ReadRunCountL(args->Arg(0));
	
	CSysMonTestHelper::IncrementRunCountL(args->Arg(0));
    CleanupStack::PopAndDestroy(args);
	
	TBool keepRunning = EFalse;
    
	CSysMonTestHelper* helper = NULL;
	
    switch (runCount)
        {
        case 0: 
			{
			RProcess::Rendezvous(KErrNone);
			helper = CSysMonTestHelper::NewLC();
			
			RSysMonSession sysmon;
			sysmon.OpenL();
			CleanupClosePushL(sysmon);
			
			TBuf<255> args;
			TBuf<255> testId;
			helper->GetTestId(testId);
			RDebug::Print(testId);
			args.Append(testId);
			args.Append(_L(" "));
			args.Append(_L("5000"));
					
			CStartupProperties* props = CStartupProperties::NewLC(KFilenameDeregTimeout, args);
			props->SetMonitored(ETrue);
			props->SetStartupType(EStartProcess);
			props->SetStartMethod(EWaitForStart);
			props->SetNoOfRetries(0);
			props->SetTimeout(0);
			props->SetRecoveryParams(ERestartOSWithMode, 1);
			
			CStartupProperties* props2 = CStartupProperties::NewLC(_L("tsysmon_app_donothing.exe"), KNullDesC);
			props2->SetMonitored(ETrue);
			props2->SetStartupType(EStartProcess);
			props2->SetStartMethod(EWaitForStart);
			props2->SetNoOfRetries(1);
			props2->SetTimeout(0);
			props2->SetRecoveryParams(ERestartOS, 0);
			
			RProcess slave1;
			slave1.Create(KFilenameDeregTimeout, args);
			CleanupClosePushL(slave1);
			slave1.Resume();
			
			RProcess slave2;
			slave2.Create(_L("tsysmon_app_donothing.exe"), KNullDesC);
			CleanupClosePushL(slave2);
			slave2.Resume();
			
			// Register with SysMon
			TInt err = 0;
			TRAP(err, sysmon.MonitorL(*props, slave1));
			TRAP(err, sysmon.MonitorL(*props2, slave2));
			slave1.Terminate(KErrNone);
			slave2.Terminate(KErrNone);
			
			CleanupStack::PopAndDestroy(5, helper);
            break;
			}
		case 1:
			{
			RProcess::Rendezvous(KErrNone);
			keepRunning = ETrue;
			helper = CSysMonTestHelper::NewLC();
			TInt startupMode;
			HAL::Get(HALData::EPersistStartupModeKernel, startupMode);
			helper->WriteResultL(startupMode);
			CleanupStack::PopAndDestroy(helper);
			break;
			}
        default: //Run normally
			{
            RProcess::Rendezvous(KErrNone);
			keepRunning = ETrue;
            break;
			}
        }
    
    CSysMonTestHelper::IncrementRunCountL(args->Arg(0));
    CleanupStack::PopAndDestroy(args);
	
	while (keepRunning)
		{
		User::After(5000000); // 5 seconds
		}
	return 0;
    }



//  Global Functions

GLDEF_C TInt E32Main()
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
	RDebug::Print(_L("slaveprocess: E32Main end"));
	return KErrNone;
	}
