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
#include "testltsycallcontrolswapcalls.h"

CTestLtsyCallControlSwapCalls::CTestLtsyCallControlSwapCalls(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}


CTestLtsyCallControlSwapCalls::~CTestLtsyCallControlSwapCalls()
	{
	
	}

TVerdict CTestLtsyCallControlSwapCalls::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlSwapCalls::doTestStepL()"));
	
	if(TestStepResult() == EPass)
		{
		TRAPD(err,NotifyIncomingCallOneL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when notify incoming call"),err);
			return TestStepResult();
			}
		
		TRAP(err, AnswerIncomingCallOneL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when answer incoming call"),err);
			return TestStepResult();
			}
		
		User::After(1000*1000);
		
		TRAP(err,NotifyIncomingCallTwoL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when notify incoming call"),err);
			return TestStepResult();
			}		

		TRAP(err, AnswerIncomingCallTwoL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when answer incoming call"),err);
			return TestStepResult();
			}		
		
		SwapCallOne();
		
		NotifyCallTwo();
		
		SwapCallTwo();
		
		NotifyCallOne();
		
		TRAP(err, HangupCallOneL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when hang up call  one"),err);
			return TestStepResult();			
			}
		
		
		TRAP(err, HangupCallTwoL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when hang up call  two"),err);
			return TestStepResult();			
			}
		
		//Close call
		CloseCallOne();
		
		//Close call
		CloseCallTwo();
		}
	
	return TestStepResult();
	}

//End of file


