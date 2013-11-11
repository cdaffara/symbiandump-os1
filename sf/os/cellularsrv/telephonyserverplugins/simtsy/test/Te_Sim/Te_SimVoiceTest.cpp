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

#include "Te_SimVoiceTest.h"

CSimVoiceTest::CSimVoiceTest() 
	{ 
	SetTestStepName(_L("VoiceTest"));
	}

TVerdict CSimVoiceTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginVoiceTest"));

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

	iLine.Close();
	iCall.Close();

	TESTL(iLine.Open(iPhone,KVoiceLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName2;
	TESTL(iCall.OpenNewCall(iLine,callName2)==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));

	INFO_PRINTF1(_L("Test Idle Call State"));
	RMobileCall::TMobileCallStatus callStatus;
	TESTL(iCall.GetMobileCallStatus(callStatus)==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusIdle);

	INFO_PRINTF1(_L("Test Idle Line State"));
	RMobileCall::TMobileCallStatus lineStatus;
	TESTL(iLine.GetMobileLineStatus(lineStatus)==KErrNone);
	TESTL(lineStatus==RMobileCall::EStatusIdle);

	INFO_PRINTF1(_L("Test Call and Line Caps"));
	RCall::TCaps callCaps;
	TESTL(iCall.GetCaps(callCaps)==KErrNone);
	TESTL(callCaps.iFlags==KCallCaps);
	RLine::TCaps lineCaps;
	TESTL(iLine.GetCaps(lineCaps)==KErrNone);
	TESTL(lineCaps.iFlags==KLineCaps);
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);
	TESTL(iCall.GetMobileCallCaps(mobileCallCapsPckg)==KErrNone);
	TESTL(mobileCallCaps.iCallControlCaps==KCallCaps);
	TESTL(mobileCallCaps.iCallEventCaps==0);

	TRequestStatus reqStatus0, reqStatus1, reqStatus2, reqStatus3, reqStatus4;
	iCall.NotifyMobileCallStatusChange(reqStatus0,callStatus);
	iLine.NotifyMobileLineStatusChange(reqStatus1,lineStatus);

	INFO_PRINTF2(_L("Dialling %S "),&KTelephoneNumber);
	iCall.Dial(reqStatus2,KTelephoneNumber);

	User::WaitForRequest(reqStatus0);		// Wait for the Call Status Change notification
	TESTL(reqStatus0==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusDialling);
	INFO_PRINTF1(_L("...Call State is Dialling"));
	CHECKPOINTL(reqStatus0.Int(),KErrNone,CHP_VOICE_CASE("A.1"));

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

	iCall.NotifyCapsChange(reqStatus3,callCaps);
	iCall.NotifyMobileCallCapsChange(reqStatus4,mobileCallCapsPckg);

	iCall.NotifyMobileCallStatusChange(reqStatus0,callStatus);
	iLine.NotifyMobileLineStatusChange(reqStatus1,lineStatus);

	User::WaitForRequest(reqStatus2);
	TESTL(reqStatus2==KErrNone);
	User::WaitForRequest(reqStatus0);
	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus0==KErrNone);
	TESTL(reqStatus1==KErrNone);
	TESTL(lineStatus==RMobileCall::EStatusConnected);
	TESTL(callStatus==RMobileCall::EStatusConnected);
	INFO_PRINTF1(_L("Connected"));

	INFO_PRINTF1(_L("Test Caps have changed dynamically"));
	User::WaitForRequest(reqStatus3);
	User::WaitForRequest(reqStatus4);
	TESTL(callCaps.iFlags & RCall::KCapsVoice);
	TESTL(callCaps.iFlags & RCall::KCapsHangUp);
	TESTL(mobileCallCaps.iCallEventCaps==0);

	INFO_PRINTF1(_L("Test Call Hold"));
	iCall.Hold(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);
	TESTL(iCall.GetMobileCallStatus(callStatus)==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusHold);
	CHECKPOINTL(reqStatus0.Int(),KErrNone,CHP_VOICE_CASE("A.3"));

	INFO_PRINTF1(_L("Test Call Resume"));
	iCall.Resume(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);
	TESTL(iCall.GetMobileCallStatus(callStatus)==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusConnected);
	CHECKPOINTL(reqStatus0.Int(),KErrNone,CHP_VOICE_CASE("A.122"));

	INFO_PRINTF1(_L("Test Call Swap"));
	iCall.Swap(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);
	TESTL(iCall.GetMobileCallStatus(callStatus)==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusHold);
	iCall.Swap(reqStatus0);
	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);
	TESTL(iCall.GetMobileCallStatus(callStatus)==KErrNone);
	TESTL(callStatus==RMobileCall::EStatusConnected);

	RCall::THookStatus lineHookStatus;
	RCall::THookStatus callHookStatus;
	iCall.NotifyHookChange(reqStatus0,callHookStatus);
	iLine.NotifyHookChange(reqStatus1,lineHookStatus);
	INFO_PRINTF1(_L("Hanging up..."));
	TESTL(iCall.HangUp()==KErrNone);
	User::WaitForRequest(reqStatus0);
	User::WaitForRequest(reqStatus1);
	TESTL(callHookStatus==RCall::EHookStatusOn);
	TESTL(lineHookStatus==RCall::EHookStatusOn);
	CHECKPOINTL(reqStatus0.Int(),KErrNone,CHP_VOICE_CASE("A.2"));
	INFO_PRINTF1(_L("Disconnected"));

	TRequestStatus reqStatus = KErrNotReady;
	TName incomingCallName;
	TName callAdded;
	iLine.NotifyIncomingCall(reqStatus, incomingCallName);
	iLine.NotifyCallAdded(reqStatus0,callAdded);
	iLine.NotifyCapsChange(reqStatus1,lineCaps);

	TInt state = 999;
	INFO_PRINTF1(_L("Testing the Publish & Subscribe..."));
	RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyIncomingVoiceCall, state);

	User::WaitForRequest(reqStatus);
	if(reqStatus!=KErrNone)
		{
		INFO_PRINTF1(_L("Incoming voice call test failed..."));
		TESTL(reqStatus==KErrNone);		// Force a failure...
		}

	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus==KErrNone);
	TESTL(callAdded==incomingCallName);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_VOICE_CASE("A.4"));

	User::WaitForRequest(reqStatus1);
	TESTL(reqStatus1==KErrNone);
	TESTL(lineCaps.iFlags==RLine::KCapsVoice);

	TESTL(iCallAnswer.OpenExistingCall(iLine,incomingCallName)==KErrNone);

	INFO_PRINTF1(_L("Answering incoming call..."));
	ret = iCallAnswer.AnswerIncomingCall();
	CHECKPOINTL(ret,KErrNone,CHP_VOICE_CASE("A.5"));
	INFO_PRINTF1(_L("Connected"));

	INFO_PRINTF1(_L("Hanging up..."));
	TESTL(iCallAnswer.HangUp()==KErrNone);
	INFO_PRINTF1(_L("Disconnected"));

	INFO_PRINTF1(_L("Closing objects..."));

	iCallAnswer.Close();

	iCall.Close();
	iLine.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

