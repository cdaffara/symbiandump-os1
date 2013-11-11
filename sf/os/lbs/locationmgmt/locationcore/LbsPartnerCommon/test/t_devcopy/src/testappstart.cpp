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
#include "testapproot.h"

_LIT(KTestName, "LBS DevLogger Copy utility");
LOCAL_D RTest test(KTestName);

/**
The doMainL method  One long function that starts a test
Some useful info is output to the debug window
*/
void doMainL()
	{
	TBuf<200>	args;
	TBuf<100>	from;
	TBuf<100>	to;
	
	User::CommandLine(args);
	TLex argsLex(args);
	
	
	from.Copy(argsLex.NextToken());
	to.Copy(argsLex.NextToken());
	
	test.Printf(_L("\nCopy from: %S\n"), &from);
	test.Printf(_L("Copy to:   %S\n"), &to);

	CTestAppRoot* mRoot = CTestAppRoot::NewL(test, from, to);
	CleanupStack::PushL(mRoot);

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();
	
	test.Printf(_L("\nCopy complete. Press any key.\n"));
	test.Getch();
	}


/**
Program entry point
@return KErrNone
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
