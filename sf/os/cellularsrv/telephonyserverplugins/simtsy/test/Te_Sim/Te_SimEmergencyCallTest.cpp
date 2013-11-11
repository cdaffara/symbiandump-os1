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

#include "Te_SimEmergencyCallTest.h"

CSimEmergencyCallTest::CSimEmergencyCallTest() 
	{ 
	SetTestStepName(_L("EmergencyCallTest"));
	}

TVerdict CSimEmergencyCallTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginEmergencyCallTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iLine.Open(iPhone,KVoiceLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName;
	TESTL(iCall.OpenNewCall(iLine,callName)==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));

	INFO_PRINTF1(_L("Test Idle Call State"));
	RMobileCall::TMobileCallStatus callStatus;
	TESTL(iCall.GetMobileCallStatus(callStatus)==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusIdle);

	INFO_PRINTF1(_L("Test Idle Line State"));
	RMobileCall::TMobileCallStatus lineStatus;
	TESTL(iLine.GetMobileLineStatus(lineStatus)==KErrNone);
	TESTL(lineStatus==RMobileCall::EStatusIdle);

	TRequestStatus reqStatus0, reqStatus1, reqStatus2;
	iCall.NotifyMobileCallStatusChange(reqStatus0,callStatus);
	iLine.NotifyMobileLineStatusChange(reqStatus1,lineStatus);

	INFO_PRINTF2(_L("Dialling Emergency Call with number %S "),&KTelephoneNumber);
	iCall.DialEmergencyCall(reqStatus2,KTelephoneNumber);

	User::WaitForRequest(reqStatus0);		// Wait for the Call Status Change notification
	TESTL(reqStatus0==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusDialling);
	INFO_PRINTF1(_L("...Call State is Dialling"));

	User::WaitForRequest(reqStatus1);		// Wait for the Line Status Change notification
	TESTL(reqStatus1==KErrNone);
	TESTL(lineStatus==RMobileCall::EStatusDialling);
	INFO_PRINTF1(_L("...Line State is Dialling"));

	iCall.NotifyMobileCallStatusChange(reqStatus0,callStatus);
	iLine.NotifyMobileLineStatusChange(reqStatus1,lineStatus);

	User::WaitForRequest(reqStatus0);		// Wait for the Call Status Change notification
	TESTL(reqStatus0==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusConnecting);
	INFO_PRINTF1(_L("...Call State is Connecting"));

	User::WaitForRequest(reqStatus1);		// Wait for the Line Status Change notification
	TESTL(reqStatus1==KErrNone);
	TESTL(lineStatus==RMobileCall::EStatusConnecting);
	INFO_PRINTF1(_L("...Line State is Connecting"));

	iCall.NotifyMobileCallStatusChange(reqStatus0,callStatus);
	iLine.NotifyMobileLineStatusChange(reqStatus1,lineStatus);

	User::WaitForRequest(reqStatus2);
	TESTL(reqStatus2==KErrNone);
	CHECKPOINTL(reqStatus2,KErrNone,CHP_VOICE_CASE("A.6"));	

	User::WaitForRequest(reqStatus0);
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus0==KErrNone);
	TESTL(reqStatus1==KErrNone);
	TESTL(lineStatus==RMobileCall::EStatusConnected);
	TESTL(callStatus==RMobileCall::EStatusConnected);
	INFO_PRINTF1(_L("Connected"));

	INFO_PRINTF1(_L("Hanging up..."));
	TESTL(iCall.HangUp()==KErrNone);
	INFO_PRINTF1(_L("Disconnected"));

	iCall.Close();
	iLine.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

