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
#include "sysmon_patch.h"

LOCAL_C TInt MainL()
    {
    TInt runCount = 0;
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    runCount = CSysMonTestHelper::ReadRunCountL(args->Arg(0));
	CSysMonTestHelper::IncrementRunCountL(args->Arg(0));
    CleanupStack::PopAndDestroy(args);
	
	TBool keepRunning = EFalse;
    
	CSysMonTestHelper* helper = NULL;
	TDateTime datetime1;
	TTime time1;
	TInt millisecs = 0;
	
    switch (runCount)
        {
		case 1:
			{
			User::After(KWaitTime);
			time1.HomeTime();
			datetime1 = time1.DateTime();
			millisecs = (datetime1.MicroSecond() / 1000) + 
						(datetime1.Second() * 1000) + 
						(datetime1.Minute() * 60 * 1000) +
						(datetime1.Hour() * 60 * 60 * 1000);
	
			helper = CSysMonTestHelper::NewLC();
			helper->WriteResultL(millisecs);
			RProcess::Rendezvous(KErrNone);
			CleanupStack::PopAndDestroy(helper);
			break;
			}
		case 2:
			{
			time1.HomeTime();
			datetime1 = time1.DateTime();
			millisecs = (datetime1.MicroSecond() / 1000) + 
						(datetime1.Second() * 1000) + 
						(datetime1.Minute() * 60 * 1000) +
						(datetime1.Hour() * 60 * 60 * 1000);
											
			TInt secondRunTime;
			CSysMonTestHelper::GetResultIntL(_L("38"), secondRunTime);
			CSysMonTestHelper* helper = CSysMonTestHelper::NewLC();
			
			if ( (secondRunTime - millisecs)  < 2000 )
				{
				helper->WriteResultL(1);
				}
			else
				{
				helper->WriteResultL(0);
				}
			RProcess::Rendezvous(KErrNone);
			CleanupStack::PopAndDestroy(helper);
			keepRunning = ETrue;
			break;
			}
        default: //Run normally
			{
            RProcess::Rendezvous(KErrNone);
			keepRunning = ETrue;
            break;
			}
        }
	
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
	RDebug::Print(_L("appfwk-sysmon-0038_slave: E32Main"));
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
	RDebug::Print(_L("appfwk-sysmon-0038_slave: E32Main end"));
	return KErrNone;
	} //lint -e765 -e714 Suppress 'not referenced' and 'could be static'
