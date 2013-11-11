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

#include "teststepinit.h"


CTestStepInit* CTestStepInit::New(CBluetoothUtilsTestServer &aTestServer)
	{
	return new CTestStepInit(aTestServer);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

CTestStepInit::CTestStepInit(CBluetoothUtilsTestServer &aTestServer)
: CBluetoothUtilsTestStepBase(aTestServer)
	{
	SetTestStepName(KTestStepInit);
	}

CTestStepInit::~CTestStepInit()
	{
	}

TVerdict CTestStepInit::doTestStepL()
	{
	User::LeaveIfError(iParent.iSocketServer.Connect());
	iParent.iLinks = CBluetoothPhysicalLinks::NewL(iParent, iParent.iSocketServer);
	return TestStepResult();
	}

