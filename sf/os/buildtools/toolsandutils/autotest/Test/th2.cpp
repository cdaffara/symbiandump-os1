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
//


#include <e32std.h>
// Autotest library header file
#include <ss_std.h>
#include <autotest.h>

// Global pointer for use by the autotest.h macro's
CAutoTest* gAutoTest;
RSocketServ gServ;


// Initialise an RTest instance
// Autotest construction expects one
_LIT(KTestTitle,"Test Harness");
LOCAL_C RTest Test(KTestTitle);


LOCAL_C TInt ApiCall1()
// Function to represent an API call
	{
	// Do nothing but return an error to demonstrate error handling
	return KErrGeneral;
	}

LOCAL_C TInt ApiCall2()
// Function to represent an API call
	{
	// Do nothing but return an error to demonstrate error handling
	return KErrGeneral;
	}

LOCAL_C TInt ApiCall3()
// Function to represent an API call
	{
	// Do nothing but return 0
	return KErrNone;
	}

LOCAL_C void Tests1()
// Function that calls one or more API calls
	{
	_LIT(KStartMessage,"Begin Test 1");
	Test.Start(KStartMessage);
	// replace the RTest () function operator overloads with this call
	// If ApiCall1() does not return KErrNone, the error code is written to file
	// if the "keepgoing" command line argument is used.
	GLOBAL_CHECKPOINT_CODE(ApiCall1());
	_LIT(KNext1,"API Call 1 Ok");
	Test.Next(KNext1);
	Test.End();
	}


LOCAL_C void Tests2()
// Function that calls one or more API calls
	{
	_LIT(KStartMessage,"Begin Test 2");
	Test.Start(KStartMessage);

	// replace the RTest () function operator overloads with this call
	// If ApiCall2() does not return KErrNone, failure (without code) is written to file
	// if the "keepgoing" command line argument is used.
	GLOBAL_CHECKPOINT(ApiCall2() == KErrNone);
	_LIT(KNext2,"API Call 2 Ok");
	Test.Next(KNext2);
	Test.End();
	}

LOCAL_C void Tests3()
// Function that calls one or more API calls
	{
	_LIT(KStartMessage,"Begin Test 3");
	Test.Start(KStartMessage);
	_LIT(KMessage,"ApiCall2() value comparison");
	GLOBAL_CHECKPOINT_COMPARE(ApiCall1(),KErrNone,KMessage);
	_LIT(KNext1,"API Call 2 Ok");
	Test.Next(KNext1);
	Test.End();
	}


LOCAL_C void Tests4L()
// Function that calls one or more API calls
	{
	_LIT(KStartMessage,"Begin Test 4");
	Test.Start(KStartMessage);

	// Connect to socket server to 
	User::LeaveIfError(gServ.Connect());

	User::LeaveIfError(ApiCall3());
	_LIT(KNext1,"API Call 3 Ok");
	Test.Next(KNext1);
	Test.End();
	}


// Set up the array of test functions, which make the API calls
LOCAL_C const TAutoTestCase Cases[4] =
	{
	{Tests1,_S("Test One")},
	{Tests2,_S("Test Two")},
	{Tests3,_S("Test Three")},
	{Tests4L,_S("Test Four")}
	};


LOCAL_C void MainL()
	{
	Test.Title();
	Test.SetLogged(EFalse);

	_LIT(KDummyTests,"Dummy Tests.Txt");
	_LIT8(KComponentInfo,"Dummy Test - xxx");
	// Library defined macro
	// Uses a global pointer instance of CAutoTest
	GLOBAL_AUTOTEST_EXECUTE(Cases,KDummyTests,KComponentInfo,Test);
	gServ.Close();
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
	_LIT(KPanic,"Th2");
	__ASSERT_ALWAYS(!err, User::Panic(KPanic,err));
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }
//
