// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// AUTOTEST.CPP
// Demonstration code for the use of Autotest.Dll.
// 
//

/**
 @file Th1.cpp
*/

#include <e32std.h>
// Autotest library header file
#include <autotest.h>

// Initialise an RTest instance
// Autotest construction expects one
_LIT(KTestTitle,"Test Harness");
LOCAL_C RTest Test(KTestTitle);

LOCAL_C TInt ApiCall1()
// Function to represent an API call
	{
	// Do nothing but return ok
	return KErrNone;
	}

LOCAL_C TInt ApiCall2()
// Function to represent an API call
	{
	// Do nothing but return an error to demonstrate error handling
	return KErrGeneral;
	}

LOCAL_C void Tests1L()
// Function that calls one or more API calls
	{
	_LIT(KStartMessage,"Begin Test 1");
	Test.Start(KStartMessage);
	User::LeaveIfError(ApiCall1());
	_LIT(KNext1,"API Call 1 Ok");
	Test.Next(KNext1);
	Test.End();
	}

LOCAL_C void Tests2L()
// Function that calls one or more API calls
	{
	_LIT(KStartMessage,"Begin Test 2");
	Test.Start(KStartMessage);
	User::LeaveIfError(ApiCall2());
	_LIT(KNext2,"API Call 2 Ok");
	Test.Next(KNext2);
	Test.End();
	}
// Set up the array of test functions, which make the API calls
LOCAL_C const TAutoTestCase Cases[2] =
	{
	{Tests1L,_S("Test One")},
	{Tests2L,_S("Test Two")}
	};

LOCAL_C void MainL()
	{
	Test.Title();
	Test.SetLogged(EFalse);

	_LIT(KDummyTests,"Dummy Tests.Txt");
	_LIT8(KComponentInfo,"Dummy Test - xxx");
	// Library defined macro
	AUTOTEST_EXECUTE(Cases,KDummyTests,KComponentInfo,Test);

	Test.Close();
	}

GLDEF_C TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	_LIT(KPanic,"Th1");
	__ASSERT_ALWAYS(!err, User::Panic(KPanic,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
//
