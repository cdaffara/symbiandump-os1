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
#include "testltsycallcontroldialemergencycallstopallcalls.h"

_LIT16(KLtsyEmergencyTelNum, "58848157");

CTestLtsyCallControlDialEmergencyCallStopAllCall::CTestLtsyCallControlDialEmergencyCallStopAllCall(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}


CTestLtsyCallControlDialEmergencyCallStopAllCall::~CTestLtsyCallControlDialEmergencyCallStopAllCall()
	{
	
	}

TVerdict CTestLtsyCallControlDialEmergencyCallStopAllCall::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlDialEmergencyCallStopAllCall::doTestStepL()"));
	
	TRAPD(err,OpenCallOneL());
	if (err != KErrNone)
		{
		SetTestStepResult(EFail);
		INFO_PRINTF2(_L("The error was %d when opne call one"),err);
		return TestStepResult();
		}			
	
	DialCallOne();
	
	User::After(1000*2000);

	if(TestStepResult() == EPass)
		{
		TRAPD(err,iLtsyModel.CallSharedData().EmergencyCall().OpenNewCall(iLtsyModel.CallSharedData().VoiceLine()));
		if (err != KErrNone)
			{
			SetTestStepResult(EFail);
			INFO_PRINTF2(_L("The error was %d when open emergency call"),err);
			return TestStepResult();			
			}
		}	
	
	TRequestStatus iStatus;
	iLtsyModel.CallSharedData().EmergencyCall().DialEmergencyCall(iStatus, KLtsyEmergencyTelNum);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("Dial emergency call result = %d"),iStatus.Int());
	
	User::After(1000*4000);
	
	iLtsyModel.CallSharedData().EmergencyCall().Close();
	
	CloseCallOne();
	
	return TestStepResult();
	}	

//End of file
