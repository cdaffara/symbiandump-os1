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
// This process will rendezvous then keep running until terminated by an external intervention. 
// Count is kept of the number of times the process has been run (based on the name of the executable the process is launched from.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code   
*/

#include "sysmontesthelper.h"

void MainL()
    {
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    CSysMonTestHelper::IncrementRunCountL(args->Arg(0));
    CleanupStack::PopAndDestroy(args);
	
    RProcess::Rendezvous(KErrNone);
	while (ETrue)
		{
		User::After(5000000); // 5 seconds
		}

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
	RDebug::Print(_L("slaveprocess: E32Main end"));
	return KErrNone;
	} //lint -e714 Suppress 'not referenced'
