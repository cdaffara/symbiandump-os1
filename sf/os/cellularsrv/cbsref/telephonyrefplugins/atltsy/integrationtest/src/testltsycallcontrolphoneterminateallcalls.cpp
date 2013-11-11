// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// 

//user include
#include "testltsycallcontrolphoneterminateallcalls.h"

CTestLtsyCallControlPhoneTerminateAllCalls::CTestLtsyCallControlPhoneTerminateAllCalls(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}


CTestLtsyCallControlPhoneTerminateAllCalls::~CTestLtsyCallControlPhoneTerminateAllCalls()
	{
	
	}

TVerdict CTestLtsyCallControlPhoneTerminateAllCalls::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlPhoneTerminateAllCalls::doTestStepL()"));
	
	TRAPD(err,OpenCallOneL());
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("The error was %d when opne call one"),err);
		return TestStepResult();
		}			
	
	DialCallOne();

	TRAP(err,OpenCallTwoL());
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("The error was %d when opne call two"),err);
		return TestStepResult();
		}		
	
	DialCallTwo();

	TRAP(err, PhoneTerminateAllCallsL());
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("The error was %d when phone terminate all calls"),err);
		return TestStepResult();			
		}	
	
	CloseCallOne();
	
	CloseCallTwo();
	
	return TestStepResult();
	}	
	
//End of file
