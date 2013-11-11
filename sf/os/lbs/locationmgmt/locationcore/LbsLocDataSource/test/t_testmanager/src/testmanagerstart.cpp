// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LbsLocManagerStart.cpp
// 
//

#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include "lbsdevloggermacros.h"
#include "testmanagerroot.h"

_LIT(KTestName, "Test Manager");
LOCAL_D RTest test(KTestName);

/**
The doMainL method  One long function that starts a test

Some useful info is output to the debug window

@internalTechnology
*/
void doMainL()
	{
	LBSNEWLOG(">>>>>>>>>>>>>>>>> Gps Location Manager Thread Started >>>>>>>>>>>>>>>>>");
	
	TBuf<100>	args;
	User::CommandLine(args);
	args.LowerCase();
	TLex argsLex(args);
	
	TInt count = -1;
	argsLex.Val(count);
	test.Printf(_L("****************************************\n"));
	test.Printf(_L("* Gps Location Manager Service Created *\n"));
	test.Printf(_L("* Loops: %d\n"), count);
	test.Printf(_L("****************************************"));
	
	CLocationManagerRoot* mRoot = CLocationManagerRoot::NewL(test, count);
	CleanupStack::PushL(mRoot);

	LBSLOG(ELogP5, "----------------- Gps Location Manager Service Created -------------------");
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy();
	LBSLOG(ELogP5, "<<<<<<<<<<<<<<<<< Gps Location Manager Thread Terminating <<<<<<<<<<<<<<<<<");
	}



/**
Program entry point

@return KErrNone
@internalTechnology
*/
GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Title();
	test.Start(KTestName);
		
	CActiveScheduler* rootScheduler = new CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());	
	test.Printf(_L("\nCompletion code: %d\n"), ret);	
	test(ret==KErrNone);
	
	delete theCleanup;	
	delete rootScheduler;
	
	test.End();
	test.Close();
		
	__UHEAP_MARKEND;
	return(KErrNone);
	}
