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
#include "testltsycallcontrolnotifyincomingcallcase3.h"


CTestLtsyCallControlNotifyIncomingCallCase3::CTestLtsyCallControlNotifyIncomingCallCase3(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}


CTestLtsyCallControlNotifyIncomingCallCase3::~CTestLtsyCallControlNotifyIncomingCallCase3()
	{
	
	}


TVerdict CTestLtsyCallControlNotifyIncomingCallCase3::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlNotifyIncomingCallCase3::doTestStepL()"));
	
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
		
		User::After(1000*1000);
		
		HoldCallOne();
		
		ResumeCallOne();
		
		TRAP(err, HangupCallOneL());
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when Hang up call one"),err);
			return TestStepResult();
			}		
		
		//Close call one
		CloseCallOne();
		}
	
	return TestStepResult();
	}

//End of file
