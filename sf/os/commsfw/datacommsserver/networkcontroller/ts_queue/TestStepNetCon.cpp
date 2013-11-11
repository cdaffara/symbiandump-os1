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
#include "QueueTests.h"

CTestStepNetCon::CTestStepNetCon(TInt aTestNumber) 
//
// C'tor
//
: iTestNumber(aTestNumber)
	{
	
	_LIT(KTestStepNameFormat, "NC-1-%d");
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
	CActiveScheduler::Install(scheduler);

	CNetConTestBase* test = NULL;

	switch(iTestNumber)
		{
		case 1:
			test = CNetConTest0101::NewLC();
			break;
		case 2:
			test = CNetConTest0102::NewLC();
			break;
		case 3:
			test = CNetConTest0103::NewLC();
			break;
		case 4:
			test = CNetConTest0104::NewLC();
			break;
		case 5:
			test = CNetConTest0105::NewLC();
			break;
		case 6:
			test = CNetConTest0106::NewLC();
			break;
		case 7:
			test = CNetConTest0107::NewLC();
			break;
		case 8:
			test = CNetConTest0108::NewLC();
			break;
		case 9:
			test = CNetConTest0109::NewLC();
			break;
		case 10:
			test = CNetConTest0110::NewLC();
			break;
		case 11:
			test = CNetConTest0111::NewLC();
			break;
		case 12:
			test = CNetConTest0112::NewLC();
			break;
		case 13:
			test = CNetConTest0113::NewLC();
			break;
		default:
			User::Leave(KErrArgument);
		}

	test->Start();
	
	if(test->Result() == MNetConTest::EPassed)
		{
		iTestStepResult = EPass;
		}
	else
		{
		iTestStepResult = EFail;
		}

	CleanupStack::PopAndDestroy(test);

	delete scheduler;
	scheduler = NULL;

	// test steps return a result
	return iTestStepResult;
	}

