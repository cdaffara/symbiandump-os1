// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "teststepdisconnectall.h"


CTestStepDisconnectAll* CTestStepDisconnectAll::New(CBluetoothUtilsTestServer &aTestServer)
	{
	return new CTestStepDisconnectAll(aTestServer);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

CTestStepDisconnectAll::CTestStepDisconnectAll(CBluetoothUtilsTestServer &aTestServer)
: CBluetoothUtilsTestStepBase(aTestServer)
	{
	SetTestStepName(KTestStepDisconnectAll);
	}

CTestStepDisconnectAll::~CTestStepDisconnectAll()
	{
	}

TVerdict CTestStepDisconnectAll::doTestStepL()
	{
	TInt err = iParent.iLinks->DisconnectAll();
	if (err != KErrNone)
		{
		SetTestStepError(err);
		return TestStepResult();
		}
	CActiveScheduler::Current()->Start();
	
	SetTestStepError(iParent.iLastError);
	if (iParent.iLastEvent != EDisconnectAllComplete)
		{
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}

