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

#include "testltsycallcontroldialemergencycall.h"

//const define
_LIT16(KLtsyEmergencyNum, "58848158");


CTestLtsyCallControlDialEmergencyCall::CTestLtsyCallControlDialEmergencyCall(CTestLtsyModel& aModel)
	: CTestLtsyCallControlBase(aModel)
	{
	
	}


CTestLtsyCallControlDialEmergencyCall::~CTestLtsyCallControlDialEmergencyCall()
	{
	
	}

TVerdict CTestLtsyCallControlDialEmergencyCall::doTestStepL()
	{
	INFO_PRINTF1(_L("starting CTestLtsyCallControlDialEmergencyCall::doTestStepL()"));
	
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
	iLtsyModel.CallSharedData().EmergencyCall().DialEmergencyCall(iStatus, KLtsyEmergencyNum);
	User::WaitForRequest(iStatus);
	
	INFO_PRINTF2(_L("Dial emergency call result = %d"),iStatus.Int());
	
	User::After(1000*4000);
	
	iLtsyModel.CallSharedData().EmergencyCall().Close();
	
	return TestStepResult();
	}

//End of file
