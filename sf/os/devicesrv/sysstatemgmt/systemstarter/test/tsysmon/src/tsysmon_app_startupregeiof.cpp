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

void MainL()
	{
	RProcess::Rendezvous(KErrNone);
	User::After(500000); // 0.5 seconds
	CSysMonTestHelper* helper = CSysMonTestHelper::NewLC();	


	helper->WriteResultL(0);

	CleanupStack::PopAndDestroy(helper);
	
	for (;;)
		{
		User::After(5000000); // 5 seconds
		}	

	}


//  Global Functions

GLDEF_C TInt E32Main()
	{
	// Create cleanup stack
	RDebug::Print(_L("appfwk-sysmon-0043: E32Main"));
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
