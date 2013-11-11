// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES
#include "epos_cpospsytesthandler.h"

// FUNCTION PROTOTYPES
void StartPSYTesterL();

GLDEF_C void Panic(TInt aErrorCode)
	{
	_LIT(KPSYTester, "PSY Tester");
	User::Panic(KPSYTester, aErrorCode);
	}

// Main function called by E32
GLDEF_C TInt E32Main()
    {

#ifndef __WINS__
	
	_LIT(KPSYTesterProcName, "PSY Tester");
	User::RenameProcess(KPSYTesterProcName);

#endif	
	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	
	TRAPD(error, StartPSYTesterL());
	__ASSERT_ALWAYS(!error, Panic(error));
	delete cleanup; // destroy clean-up stack

	__UHEAP_MARKEND;
	return 0; // and return
	
    }

void StartPSYTesterL() 
	{
	__UHEAP_MARK;

	// Construct and install the active scheduler
	CActiveScheduler* scheduler = 
		new (ELeave) CActiveScheduler;
	// Push onto the cleanup stack
	CleanupStack::PushL(scheduler);

	// Install as the active scheduler
	CActiveScheduler::Install(scheduler);

	CPosPSYTestHandler* psyTestHandler = CPosPSYTestHandler::NewL();
	CleanupStack::PushL(psyTestHandler);

	// Start the active scheduler
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(psyTestHandler);
	
	CleanupStack::PopAndDestroy(scheduler);
	__UHEAP_MARKEND;
	}
// End of file
