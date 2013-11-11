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

#include "cteststepbtromconfigSdpAgent001.h"
#include <btsdp.h>

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
	}

CTestStepBtRomConfigSdpAgent001::
CTestStepBtRomConfigSdpAgent001(CTestServer& aParent) 
	: CTestStepBtRomConfigBase(aParent)
	{
	SetTestStepName(KTestNameSdpAgent001);
	}

/**
Static Constructor
Note the lack of ELeave. This means that having insufficient memory
will return NULL;
*/
CTestStepBtRomConfigSdpAgent001* 
CTestStepBtRomConfigSdpAgent001::New(CTestServer& aParent)
	{
	return new CTestStepBtRomConfigSdpAgent001(aParent); 
	}


/**
See BT_ROMCONFIG_SDPAGENT_001.script
*/
TVerdict CTestStepBtRomConfigSdpAgent001::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CTestStepBtRomConfigSdpAgent001::doTestStepL()"));
	
	CActiveScheduler* sched(new (ELeave)CActiveScheduler);
	CActiveScheduler::Install(sched);
	CleanupStack::PushL(sched);

	const TInt expectedError(iBtExcluded ? KErrBadName : KErrNone);
	// this is only used for logging readability purposes
	const TDesC* expectedErrorDesPtr(iBtExcluded ? 
									  &KErrBadNameLit : &KErrNoneLit);	
	TRAPD(err1, TestSdpAgentL(1));

	if (expectedError != err1)
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling CSdpAgent::NextRecordRequestL"), 
			expectedErrorDesPtr, expectedError, err1);
		SetTestStepResult(EFail);
		}
	
	TRAPD(err2, TestSdpAgentL(2));
	if (expectedError != err2)
		{
		INFO_PRINTF4(\
			_L("Failed: Expected %S(%d) and got %d calling CSdpAgent::NextRecordRequestL"), 
			expectedErrorDesPtr, expectedError, err2);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(sched);
	
	INFO_PRINTF1(_L("&lt;&lt;CTestStepBtRomConfigSdpAgent001::doTestStepL()"));

	CheckAndSetTestResult();
	return TestStepResult(); 
	}

void CTestStepBtRomConfigSdpAgent001::TestSdpAgentL(TInt aTestSubCase)
	{

	const TBTDevAddr Kaddr(0x000e07966a4);
	const TUUID KdummyUUID(1);

	//Create a search pattern so we only see SDP records for our application
	CSdpSearchPattern *pattern(CSdpSearchPattern::NewL());
	CleanupStack::PushL(pattern);
	User::LeaveIfError(pattern->AddL(KdummyUUID));

	switch (aTestSubCase)
		{
	case 1:
		{
		CSdpAgent* agent(CSdpAgent::NewL(*this, Kaddr));
		CleanupStack::PushL(agent);

		agent->SetRecordFilterL(*pattern);
		agent->NextRecordRequestL();		

		CleanupStack::PopAndDestroy(agent);
		CleanupStack::PopAndDestroy(pattern);
		}
		break;
		
	case 2:
		{
		CSdpAgent* agent(CSdpAgent::NewLC(*this, Kaddr));
		
		agent->SetRecordFilterL(*pattern);
		agent->NextRecordRequestL();				

		CleanupStack::PopAndDestroy(agent);
		CleanupStack::PopAndDestroy(pattern);
		}
		break;
		
	default:
		SetTestStepResult(EFail);
		User::Panic(KInvalidTestPanicString, aTestSubCase);
		break;
		}
	}

// stubs
void CTestStepBtRomConfigSdpAgent001::NextRecordRequestComplete(TInt, 
																TSdpServRecordHandle, 
																TInt)
	{}

void CTestStepBtRomConfigSdpAgent001::AttributeRequestResult(TSdpServRecordHandle,
															 TSdpAttributeID,
															 CSdpAttrValue *)
	{}

void CTestStepBtRomConfigSdpAgent001::AttributeRequestComplete(TSdpServRecordHandle, 
															   TInt)
	{}
// EOF
