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

LOCAL_C TInt MainL()
    {
    TInt runCount = 0;
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    runCount = CSysMonTestHelper::ReadRunCountL(args->Arg(0));
	CSysMonTestHelper::IncrementRunCountL(args->Arg(0));
	CleanupStack::PopAndDestroy(args);
	
	TBool keepRunning = EFalse;
    
    switch (runCount)
        {
		case 1:
			RProcess::Rendezvous(KErrGeneral);
			User::After(5000000);
			break;
		default: //Run normally
            RProcess::Rendezvous(KErrNone);
			keepRunning = ETrue;
            break;
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
