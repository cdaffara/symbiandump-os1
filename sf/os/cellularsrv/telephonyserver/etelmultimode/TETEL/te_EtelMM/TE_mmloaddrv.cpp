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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmloaddrv.h"
#include "testdef.h"

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

CTestMMLoadDrv::CTestMMLoadDrv()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMLoadDrv"));
	}

enum TVerdict CTestMMLoadDrv::doTestStepL()
{
	TInt r=User::LoadPhysicalDevice(PDD_NAME);
	TEST(r==KErrNone || r==KErrAlreadyExists);
#if defined PDD2_NAME
	r=User::LoadPhysicalDevice(PDD2_NAME);
	TEST(r==KErrNone || r==KErrAlreadyExists);
#endif
	r=User::LoadLogicalDevice(LDD_NAME);
	TEST(r==KErrNone|| r==KErrAlreadyExists);

 	// When bootstrapping C32 we have to avoid the PhBkSyncServer being started, since
 	// it needs a different CommDB
 	_LIT(KPhbkSyncCMI, "phbsync.cmi");
	r = StartC32WithCMISuppressions(KPhbkSyncCMI);
	if((r!=KErrNone)&&(r!=KErrAlreadyExists))
		INFO_PRINTF2(_L("Failed to start C32 %d"),r);
	
	TEST(iCommServer.Connect()==KErrNone);
	TInt res=iCommServer.LoadCommModule(_L("ECUART.CSY"));
	TEST(res==KErrNone || res==KErrAlreadyExists);
	INFO_PRINTF1(_L("TE_ETELMM - Testing Multimode Cellular API"));

//	TInt ret = server.Connect();
//	TEST(ret==KErrNone);
//	ret=server.LoadPhoneModule(DMMTSY_MODULE_NAME);
//	TEST(ret==KErrNone);

	return TestStepResult();
}

CTestMMUnLoadDrv::CTestMMUnLoadDrv()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestMMUnLoadDrv"));
	}

enum TVerdict CTestMMUnLoadDrv::doTestStepL()
{
//	server.UnloadPhoneModule(DMMTSY_MODULE_NAME);
//	server.Close();

//	test.End();

	iCommServer.Close();

	return TestStepResult();
}
