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
#include "testltsycallcontrolnotifyincomingcallcase1.h"


CTestLtsyCallControlNotifyIncomingCallCase1::CTestLtsyCallControlNotifyIncomingCallCase1(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}


CTestLtsyCallControlNotifyIncomingCallCase1::~CTestLtsyCallControlNotifyIncomingCallCase1()
	{
	
	}

TVerdict CTestLtsyCallControlNotifyIncomingCallCase1::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlNotifyIncomingCallCase1::doTestStepL()"));
	
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
		
		//Close call
		CloseCallOne();
		}
	
	return TestStepResult();
	}

//End of file
