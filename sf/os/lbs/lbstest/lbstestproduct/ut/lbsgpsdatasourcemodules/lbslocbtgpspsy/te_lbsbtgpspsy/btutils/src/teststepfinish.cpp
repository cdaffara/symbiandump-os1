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

#include "teststepfinish.h"


CTestStepFinish* CTestStepFinish::New(CBluetoothUtilsTestServer &aTestServer)
	{
	return new CTestStepFinish(aTestServer);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

CTestStepFinish::CTestStepFinish(CBluetoothUtilsTestServer &aTestServer)
: CBluetoothUtilsTestStepBase(aTestServer)
	{
	SetTestStepName(KTestStepFinish);
	}

CTestStepFinish::~CTestStepFinish()
	{
	}

TVerdict CTestStepFinish::doTestStepL()
	{
	delete iParent.iLinks;
	iParent.iLinks = NULL;
	iParent.iSocketServer.Close();
	return TestStepResult();
	}

