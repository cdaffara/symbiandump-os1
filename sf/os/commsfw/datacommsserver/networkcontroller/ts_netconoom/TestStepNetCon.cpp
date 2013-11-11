// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestStepNetCon.h"
#include <networking/log.h>
#include "OomTests.h"

CTestStepNetCon::CTestStepNetCon(TInt aTestNumber) 
//
// C'tor
//
: iTestNumber(aTestNumber)
	{
	
	_LIT(KTestStepNameFormat, "NC-5-%d");
	iTestStepName.AppendFormat(KTestStepNameFormat(), iTestNumber);
	}

CTestStepNetCon::~CTestStepNetCon()
//
// D'tor
//
	{
	}

enum TVerdict CTestStepNetCon::doTestStepL()
//
// Run a netcon test step
//
	{

	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	TInt allocation(1);
	MNetConTest::TResult result = MNetConTest::EFailed;
	
	while(result != MNetConTest::EPassed)
		{
		__UHEAP_MARK;

		CNetConTestBase* test = GetTestLC();

		__UHEAP_SETFAIL(RHeap::EDeterministic, allocation);
		test->Start();
		__UHEAP_SETFAIL(RHeap::ENone, 1);

		++allocation;
		result = test->Result();

		CleanupStack::PopAndDestroy(test);
		test = NULL;

		__UHEAP_MARKEND;
		}
	
	CleanupStack::PopAndDestroy(scheduler);
	return EPass;
	}

CNetConTestBase* CTestStepNetCon::GetTestLC()
//
// Return the test
//
	{

	CNetConTestBase* test = NULL;

	switch(iTestNumber)
		{
		case 1:
			test = CNetConTest0501::NewLC();
			break;
		case 2:
			test = CNetConTest0502::NewLC();
			break;
		default:
			User::Leave(KErrArgument);
		}

	return test;
	}

