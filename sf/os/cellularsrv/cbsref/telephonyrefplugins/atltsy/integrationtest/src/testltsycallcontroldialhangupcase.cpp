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
#include "testltsycallcontroldialhangupcase.h"

CTestLtsyCallControlDialHangupCase::CTestLtsyCallControlDialHangupCase(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}

CTestLtsyCallControlDialHangupCase::~CTestLtsyCallControlDialHangupCase()
	{
	
	}

TVerdict CTestLtsyCallControlDialHangupCase::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlDialHangupCase::doTestStepL()"));
	
	if(TestStepResult() == EPass)
		{
		//Open Call
		TRAPD(err,OpenCallTwoL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when opne call two"),err);
			return TestStepResult();
			}
		
		DialCallTwo();
		
		User::After(1000*1000);
		
		//Hang up call
		TRAP(err, HangupCallTwoL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when hang up call two"),err);
			return TestStepResult();
			}
		
		//Close call
		CloseCallTwo();
		}
	
	return TestStepResult();
	}

//End of file
