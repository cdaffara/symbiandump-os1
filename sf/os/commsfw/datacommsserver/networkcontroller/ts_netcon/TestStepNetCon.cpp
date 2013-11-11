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
#include "SelectionTests.h"
#include "ReconnectTests.h"
#include "BearerTests.h"

CTestStepNetCon::CTestStepNetCon(TInt aTestNumber) 
//
// C'tor
//
: iTestNumber(aTestNumber)
	{
	
	TInt major(0);
	TInt minor(iTestNumber);
	
	if(iTestNumber <= 24)
		{
		major = 2;
		}
	else if (iTestNumber > 24 && iTestNumber <= 28)
		{
		major = 3;
		minor -= 24;
		}
	else
		{
		major = 4;
		
		// deal with the fact that minor numbers 7, 8, 10, 12, 14, 16 are missing
		if(iTestNumber <= 34)
			{
			minor -= 28;
			}
		else if (iTestNumber >= 39)
			{
			minor -= 22;
			}
		else if (iTestNumber == 35)
			{
			minor = 9;
			}
		else if (iTestNumber == 36)
			{
			minor = 11;
			}
		else if (iTestNumber == 37)
			{
			minor = 13;
			}
		else if (iTestNumber == 38)
			{
			minor = 15;
			}
		}

	_LIT(KTestStepNameFormat, "NC-%d-%d");
	iTestStepName.AppendFormat(KTestStepNameFormat(), major, minor);
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
			test = CNetConTest0201::NewLC();
			break;
		case 2:
			test = CNetConTest0202::NewLC();
			break;
		case 3:
			test = CNetConTest0203::NewLC();
			break;
		case 4:
			test = CNetConTest0204::NewLC();
			break;
		case 5:
			test = CNetConTest0205::NewLC();
			break;
		case 6:
			test = CNetConTest0206::NewLC();
			break;
		case 8:
			test = CNetConTest0208::NewLC();
			break;
		case 9:
			test = CNetConTest0209::NewLC();
			break;
		case 10:
			test = CNetConTest0210::NewLC();
			break;
		case 11:
			test = CNetConTest0211::NewLC();
			break;
		case 12:
			test = CNetConTest0212::NewLC();
			break;
		case 13:
			test = CNetConTest0213::NewLC();
			break;
		case 14:
			test = CNetConTest0214::NewLC();
			break;
		case 15:
			test = CNetConTest0215::NewLC();
			break;
		case 16:
			test = CNetConTest0216::NewLC();
			break;
		case 17:
			test = CNetConTest0217::NewLC();
			break;
		case 18:
			test = CNetConTest0218::NewLC();
			break;
		case 19:
			test = CNetConTest0219::NewLC();
			break;
		case 20:
			test = CNetConTest0220::NewLC();
			break;
		case 21:
			test = CNetConTest0221::NewLC();
			break;
		case 22:
			test = CNetConTest0222::NewLC();
			break;
		case 23:
			test = CNetConTest0223::NewLC();
			break;
		case 24:
			test = CNetConTest0224::NewLC();
			break;
		case 25:
			test = CNetConTest0301::NewLC();
			break;
		case 26:
			test = CNetConTest0302::NewLC();
			break;		
		case 27:
			test = CNetConTest0303::NewLC();
			break;		
		case 28:
			test = CNetConTest0304::NewLC();
			break;
		case 29:
			test = CNetConTest0401::NewLC();
			break;
		case 30:
			test = CNetConTest0402::NewLC();
			break;
		case 31:
			test = CNetConTest0403::NewLC();
			break;
		case 32:
			test = CNetConTest0404::NewLC();
			break;
		case 33:
			test = CNetConTest0405::NewLC();
			break;
		case 34:
			test = CNetConTest0406::NewLC();
			break;
		case 35:
			test = CNetConTest0409::NewLC();
			break;
		case 36:
			test = CNetConTest0411::NewLC();
			break;
		case 37:
			test = CNetConTest0413::NewLC();
			break;
		case 38:
			test = CNetConTest0415::NewLC();
			break;
		case 39:
			test = CNetConTest0417::NewLC();
			break;
		case 40:
			test = CNetConTest0418::NewLC();
			break;
		case 41:
			test = CNetConTest0419::NewLC();
			break;
		case 42:
			test = CNetConTest0420::NewLC();
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

