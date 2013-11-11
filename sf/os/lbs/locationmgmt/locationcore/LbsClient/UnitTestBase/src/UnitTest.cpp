// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cons.h>
#include "testregistry.h"
#include "unittest.h"

GLDEF_D RTest test(KTestName);

void mainL()
	{

	RTestRegistry::CreateL();
	RTestRegistry::RunAllTests();
	}

void doMainL()
	{
	TRAPD(error, mainL())
	if (error)
		test.Printf(_L("Error setting up the tests: %d\n"), error);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Title();
	test.Start(KTestName);
		
	CActiveScheduler* rootScheduler = new CActiveScheduler;
	CActiveScheduler::Install(rootScheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());	
	test.Printf(_L("\nTest Completion code: %d\n"), ret);	
	test(ret==KErrNone);
	
	delete theCleanup;	
	delete rootScheduler;
	
	test.End();
	test.Close();
		
	__UHEAP_MARKEND;
	return(KErrNone);
	}
