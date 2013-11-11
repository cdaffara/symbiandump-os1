// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TE_EtelBase.h"
#include "TE_Etelloaddrv.h"


CTestEtelLoadDrv::CTestEtelLoadDrv()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestEtelLoadDrv"));
	}

enum TVerdict CTestEtelLoadDrv::doTestStepL()
{
    TInt  res = iCommServer.Connect();
    TESTCHECK(res, KErrNone);

	res = iCommServer.LoadCommModule(_L("ECUART.CSY"));
    TESTCHECKCONDITION(res==KErrNone || res==KErrAlreadyExists);

	return TestStepResult();
}

CTestEtelUnLoadDrv::CTestEtelUnLoadDrv()
/** Each test step initialises it's own name
*/
	{
	SetTestStepName(_L("TestEtelUnLoadDrv"));
	}

enum TVerdict CTestEtelUnLoadDrv::doTestStepL()
{
	return TestStepResult();
}
