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
	RProcess::Rendezvous(KErrNone);
	CSysMonTestHelper* helper = CSysMonTestHelper::NewLC();

	TInt timeout = 0;
	helper->GetTimeoutArgL(timeout);
	
	RSysMonSession sysmon;
	CleanupClosePushL(sysmon);

	TRAPD(err, sysmon.OpenL());
	// If we fail to connect, pass the error code out to the test step and then re-leave	
	if (err != KErrNone)
		{
		helper->WriteDeregResultL(err);
		User::Leave(err);
		}

	User::After(timeout * 1000);
	
	TRAP(err,sysmon.CancelMonitorSelfL());
	helper->WriteDeregResultL(err);

	CleanupStack::PopAndDestroy(2, helper);

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
	} //lint -e714 Suppress 'not referenced'
