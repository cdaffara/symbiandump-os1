// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cteststepbtromconfigrbtphylnkAdapt001.h"
#include <bt_sock.h>

/**
   Unnamed namespaces are a superior replacement for the static
   declaration of variables. They allow variables and functions to be
   visible within an entire translation unit, yet not visible
   externally. Although entities in an unnamed namespace might have
   external linkage, they are effectively qualified by a name unique
   to their translation unit and therefore can never be seen from any
   other translation unit.
*/
namespace
	{
	/** 
		Panic string if an invalid test subcase is specified Panic
		number is the invalid subcase
	*/
	_LIT(KInvalidTestPanicString, "Invalid Test subcase specified");
	_LIT(KErrDisconnectedLit, "KErrDisconnected");
	}

CTestStepBtRomConfigRBtPhyLnkAdapt001::
CTestStepBtRomConfigRBtPhyLnkAdapt001(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameRBtPhyLnkAdapt001);
	}

CTestStepBtRomConfigRBtPhyLnkAdapt001::~CTestStepBtRomConfigRBtPhyLnkAdapt001()
	{
	if (iSocketServ.Handle())
		{
		iSocketServ.Close();
		}
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory
will return NULL;
*/
CTestStepBtRomConfigRBtPhyLnkAdapt001* 
CTestStepBtRomConfigRBtPhyLnkAdapt001::New(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigRBtPhyLnkAdapt001(aParent); 
	}

/**
See BT_ROMCONFIG_RBTPHYLNKADAPT_001.script
*/
TVerdict CTestStepBtRomConfigRBtPhyLnkAdapt001::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CTestStepBtRomConfigRBtPhyLnkAdapt001::doTestStepL()"));
	
	CActiveScheduler* sched(new (ELeave)CActiveScheduler);
	CActiveScheduler::Install(sched);
	CleanupStack::PushL(sched);

	User::LeaveIfError(iSocketServ.Connect());

	// if BT is included, we will be 'disconnected' as we will specify
	// an invalid BT address
	const TInt expectedError(iBtExcluded ? KErrBadName : KErrDisconnected);

	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr(iBtExcluded ? 
									 &KErrBadNameLit : &KErrDisconnectedLit);

	const TInt err(TestPhyLnkAdapt(1));

	if (expectedError != err)
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling RBTPhysicalLinkAdapter::Open"), 
			expectedErrorDesPtr, expectedError, err);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(sched);
	
	INFO_PRINTF1(_L("&lt;&lt;CTestStepBtRomConfigRBtPhyLnkAdapt001::doTestStepL()"));
	CheckAndSetTestResult();
	return TestStepResult();
	}

TInt CTestStepBtRomConfigRBtPhyLnkAdapt001::TestPhyLnkAdapt(TInt aTestSubCase)
	{
	TInt ret = KErrNone;
	
	switch (aTestSubCase)
		{
	case 1:
		{
		TBTDevAddr addr(0x000e07966a4); // dummy BT address
		RBTPhysicalLinkAdapter pla;
		ret = pla.Open(iSocketServ, addr);
		pla.Close();
		}
		break;
	
	default:
		SetTestStepResult(EFail);
		User::Panic(KInvalidTestPanicString, aTestSubCase);
		break;
		}
		
	return ret;
	}

// EOF
