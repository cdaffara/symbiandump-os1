// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "TE_EtelMMTestStepBase.h"
#include "TE_mmline.h"
#include "testdef.h"

CTestLine::CTestLine()
/** Each test step initialises it's own name
*/
	{
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("TestLine"));
	}

enum TVerdict CTestLine::doTestStepL()
//
// Test Line Functions
//
    {
	iTestCount=1;
	INFO_PRINTF1(_L(""));
	INFO_PRINTF1(_L("Test Mobile Line Functionality"));
//	LOGTEXT(_L8("Test Mobile Line Functionality"));
	
	RMobilePhone mmPhone;
	TInt ret=mmPhone.Open(iTelServer,DMMTSY_PHONE_NAME);
	TEST(ret==KErrNone);

	RMobileLine mmLine;
	ret=mmLine.Open(mmPhone,DMMTSY_LINE_VOICE_NAME);
    TEST(ret==KErrNone);

	// Test for Primary or Auxillary voice line

	// line caps are part of Etel core api but as we have extended them 
	// in etelmm so we test the extension here 
	RLine::TCaps lineCaps = {DMMTSY_MOBILE_LINE_AUX_VOICE};
	lineCaps.iFlags ^= DMMTSY_MOBILE_LINE_AUX_VOICE; // make sure AUX voice bit is clear						

	// Get Caps will set the AUX voice bit 
	TEST(KErrNone==mmLine.GetCaps(lineCaps));

	if(lineCaps.iFlags & DMMTSY_MOBILE_LINE_AUX_VOICE)
		{
		INFO_PRINTF2(_L("Test %d - Line is an auxiliary voice line"), iTestCount) ;
		INFO_PRINTF2(_L("Test %d - Test for Primary or Auxillary voice line - passed"), iTestCount++);
		}
	else
		{
		INFO_PRINTF2(_L("Test %d - Line is a primary voice line"), iTestCount) ;
		INFO_PRINTF2(_L("Test %d - Test for Primary or Auxillary voice line - FAILED"), iTestCount++);
		}

	// Get Line Status
	TRequestStatus reqStatus;

	RMobileCall::TMobileCallStatus lineStatus;
	TEST(KErrNone==mmLine.GetMobileLineStatus(lineStatus));
	TEST(lineStatus==DMMTSY_CALL_STATUS1);
	INFO_PRINTF2(_L("Test %d - RMobileLine::GetMobileLineStatus (sync) passed"), iTestCount++);

	// Notify Change of Line Status
	// asynchronous

	mmLine.NotifyMobileLineStatusChange(reqStatus,lineStatus);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone);
	TEST(lineStatus==DMMTSY_CALL_STATUS2);

	// asynchronous & cancel

	mmLine.NotifyMobileLineStatusChange(reqStatus,lineStatus);
	mmLine.CancelAsyncRequest(EMobileLineNotifyMobileLineStatusChange);
	User::WaitForRequest(reqStatus);
	TEST(reqStatus.Int()==KErrNone || reqStatus.Int()==KErrCancel);
	if (reqStatus.Int()==KErrCancel)
		INFO_PRINTF2(_L("Test %d - RMobileLine::NotifyMobileLineStatusChange (async & cancel (Cancelled Request)) passed"), iTestCount++);
	else
		{
		TEST(lineStatus==DMMTSY_CALL_STATUS2);
		INFO_PRINTF2(_L("Test %d - RMobileLine::NotifyMobileLineStatusChange (async & cancel (Request Not Cancelled)) passed"), iTestCount++);
		}

	mmLine.Close();
	mmPhone.Close();
	INFO_PRINTF1(_L(""));

//	INFO_PRINTF2(_L("OK: RMobileLine's MobileLineStatus"));

	return TestStepResult();
	}
