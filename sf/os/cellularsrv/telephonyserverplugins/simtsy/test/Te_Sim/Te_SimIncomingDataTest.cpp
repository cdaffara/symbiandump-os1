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

#include "Te_SimIncomingDataTest.h"

CSimIncomingDataTest::CSimIncomingDataTest() 
	{ 
	SetTestStepName(_L("IncomingDataTest"));
	}

TVerdict CSimIncomingDataTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginIncomingDataTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

	StartNTRasSimulation();
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iPhone.Initialise()==KErrNone);
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iLine.Open(iPhone,KDataLineName)==KErrNone);
	INFO_PRINTF1(_L("Opened Line object"));

	TName callName;
	TESTL(iCall.OpenNewCall(iLine,callName)==KErrNone);
	INFO_PRINTF1(_L("Opened Call object"));

	INFO_PRINTF1(_L("Get DataCall Capabilities"));
	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
	TESTL(iCall.GetMobileDataCallCaps(callCapsPckg)==KErrNone);
	 
	INFO_PRINTF1(_L("Get DataCall RLP Range"));
	TRequestStatus stat0;
	RMobileCall::TMobileDataRLPRangesV1 rlp;
	RMobileCall::TMobileDataRLPRangesV1Pckg rlpPckg(rlp);
	iCall.GetMobileDataCallRLPRange(stat0,1,rlpPckg);
	User::WaitForRequest(stat0);
	TESTL(stat0==KErrNone);
	
	INFO_PRINTF1(_L("Get DataCall Hscsd information"));
	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);
	TESTL(iCall.GetCurrentHscsdInfo(infoPckg)==KErrNone);

	TRequestStatus reqStatus0, reqStatus1;
	RMobileCall::TMobileCallStatus lineStatus;
	iLine.NotifyMobileLineStatusChange(reqStatus0,lineStatus);

	TName incomingCallName;
	iLine.NotifyIncomingCall(reqStatus1,incomingCallName);

	TInt state = 999;
	INFO_PRINTF1(_L("Testing the Publish & Subscribe..."));
	RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyIncomingDataCall, state);
	
	User::WaitForRequest(reqStatus1);
	if(reqStatus1!=KErrNone)
		{
		INFO_PRINTF1(_L("Incoming data call test failed..."));
		TESTL(reqStatus1==KErrNone);		// Force a failure...
		}
	CHECKPOINTL(reqStatus1,KErrNone,CHP_DATA_CASE("B.4"));

	User::WaitForRequest(reqStatus0);
	TESTL(reqStatus0==KErrNone);
	TESTL(lineStatus==RMobileCall::EStatusRinging);

	TESTL(iCallAnswer.OpenExistingCall(iLine,incomingCallName)==KErrNone);

	INFO_PRINTF1(_L("Answering incoming call..."));
	RMobileCall::TMobileHscsdCallParamsV1 mobileCallParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg mobileCallParamsPckg(mobileCallParams);

	mobileCallParams.iSpeakerControl=RCall::EMonitorSpeakerControlOnExceptDuringDialling;
	mobileCallParams.iSpeakerVolume=RCall::EMonitorSpeakerVolumeUnknown;
	mobileCallParams.iWaitForDialTone=RCall::EDialToneNoWait;
	mobileCallParams.iInterval=5678;
	mobileCallParams.iAutoRedial=ETrue;
	mobileCallParams.iCug.iCugIndex=99;
	mobileCallParams.iIdRestrict=RMobileCall::ESendMyId;
	mobileCallParams.iWantedAiur=RMobileCall::EAiurBps9600;
	mobileCallParams.iWantedRxTimeSlots=5;
	mobileCallParams.iMaxTimeSlots=30;
	mobileCallParams.iCodings=RMobileCall::ETchCoding48;

	TESTL(iCallAnswer.AnswerIncomingCall(mobileCallParamsPckg)==KErrNone);
	INFO_PRINTF1(_L("Connected"));

	RCall::TStatus callStatus;
	TESTL(iCallAnswer.GetStatus(callStatus)==KErrNone);
	TESTL(callStatus==RCall::EStatusConnected);

	INFO_PRINTF1(_L("Checking Call Params"));	
	RMobileCall::TMobileHscsdCallParamsV1 mobileCallParams2;
	RMobileCall::TMobileHscsdCallParamsV1Pckg mobileCallParams2Pckg(mobileCallParams2);
	TESTL(iCallAnswer.GetCallParams(mobileCallParams2Pckg)==KErrNone);
	TESTL(mobileCallParams2.iSpeakerControl==RCall::EMonitorSpeakerControlOnExceptDuringDialling);
	TESTL(mobileCallParams2.iSpeakerVolume==RCall::EMonitorSpeakerVolumeUnknown);
	TESTL(mobileCallParams2.iWaitForDialTone==RCall::EDialToneNoWait);
	TESTL(mobileCallParams2.iInterval==5678);

	INFO_PRINTF1(_L("Notification for Hscsd info changes"));
	iCall.NotifyHscsdInfoChange(reqStatus1,infoPckg);
	 
	INFO_PRINTF1(_L("Set DataCall Hscsd Dynamic Parameters"));
	iCall.SetDynamicHscsdParams(reqStatus0,RMobileCall::EAiurBps57600, 15);
	User::WaitForRequest(reqStatus0);	
	TESTL(reqStatus0==KErrNone);
	User::WaitForRequest(reqStatus1);	
	TESTL(reqStatus1==KErrNone);
	INFO_PRINTF1(_L("Completed notification..."));

	INFO_PRINTF1(_L("Hanging up..."));
	TESTL(iCallAnswer.HangUp()==KErrNone);
	INFO_PRINTF1(_L("Disconnected"));

	TESTL(iCallAnswer.GetStatus(callStatus)==KErrNone);
	TESTL(callStatus==RCall::EStatusIdle);

	INFO_PRINTF1(_L("Closing objects..."));

	iCallAnswer.Close();
	iCall.Close();
	iLine.Close();
   	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

