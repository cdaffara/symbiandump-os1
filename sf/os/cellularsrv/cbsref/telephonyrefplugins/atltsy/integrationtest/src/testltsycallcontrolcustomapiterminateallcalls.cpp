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

#include "testltsycallcontrolcustomapiterminateallcalls.h"

CTestLtsyCallControlCustomAPITerminateAllCalls::CTestLtsyCallControlCustomAPITerminateAllCalls(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}

CTestLtsyCallControlCustomAPITerminateAllCalls::~CTestLtsyCallControlCustomAPITerminateAllCalls()
	{
	
	}

TVerdict CTestLtsyCallControlCustomAPITerminateAllCalls::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlCustomAPITerminateAllCalls::doTestStepL()"));
	
	if(TestStepResult() == EPass)
		{
		TRAPD(err,NotifyIncomingCallOneL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when notify incoming call one"),err);
			return TestStepResult();
			}
		
		TRAP(err, AnswerIncomingCallOneL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when answer incoming call one"),err);
			return TestStepResult();
			}
		
		TRAP(err, CustomAPITerminateAllCallsL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when terminat all calls"),err);
			return TestStepResult();
			}
		
		//Close call
		CloseCallOne();
		}
	
	return TestStepResult();
	}

//End of file
