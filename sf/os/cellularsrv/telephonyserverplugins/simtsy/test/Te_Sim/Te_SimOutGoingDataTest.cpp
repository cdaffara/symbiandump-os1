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

#include "Te_SimOutGoingDataTest.h"

CSimOutGoingDataTest::CSimOutGoingDataTest() 
	{ 
	SetTestStepName(_L("OutGoingDataTest"));
	}

TVerdict CSimOutGoingDataTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginOutGoingDataTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(0);

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

// Test a Dial with Core Call Params...
	INFO_PRINTF1(_L("Get Bearer Information"));
	RCall::TBearerService bearerServiceInfo;
	TESTL(iCall.GetBearerServiceInfo(bearerServiceInfo)==KErrEtelCallNotActive);

 	INFO_PRINTF1(_L("Get DataCall Capabilities"));
 	RMobileCall::TMobileCallDataCapsV1 callCaps;
 	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
 	ret=iCall.GetMobileDataCallCaps(callCapsPckg);
 	CHECKPOINTL(ret,KErrNone,CHP_DATA_CASE("B.7"));
 
 	INFO_PRINTF1(_L("Get DataCall RLP Range"));
 	TRequestStatus stat0;
 	RMobileCall::TMobileDataRLPRangesV1 rlp;
 	RMobileCall::TMobileDataRLPRangesV1Pckg rlpPckg(rlp);
 	iCall.GetMobileDataCallRLPRange(stat0,1,rlpPckg);
 	User::WaitForRequest(stat0);
 	CHECKPOINTL(stat0,KErrNone,CHP_DATA_CASE("B.6"));
 
 	INFO_PRINTF1(_L("Get DataCall Hscsd information"));
 	RMobileCall::TMobileCallHscsdInfoV1 info;
 	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);
 	ret=iCall.GetCurrentHscsdInfo(infoPckg);
 	CHECKPOINTL(ret,KErrNone,CHP_DATA_CASE("B.9"));
 
 	INFO_PRINTF1(_L("Get Hscsd Call Params"));
 	RMobileCall::TMobileHscsdCallParamsV1 callParams;
 	RMobileCall::TMobileHscsdCallParamsV1Pckg callParamsPckg(callParams);
 
 	TESTL(iCall.GetCallParams(callParamsPckg)==KErrEtelCallNotActive);

	StartNTRasSimulation();
 	INFO_PRINTF2(_L("Dialling %S "),&KTelephoneNumber);
 	TRequestStatus reqStatus0;
 	callParams.iSpeakerControl=RCall::EMonitorSpeakerControlAlwaysOn;
 	callParams.iSpeakerVolume=RCall::EMonitorSpeakerVolumeHigh;
 	callParams.iWaitForDialTone=RCall::EDialToneWait;
 	callParams.iInterval=1234;
 	callParams.iWantedAiur=RMobileCall::EAiurBps9600;
 	callParams.iWantedRxTimeSlots=5;
 	callParams.iMaxTimeSlots=30;
 	callParams.iCodings=RMobileCall::ETchCoding48;

 	iCall.Dial(reqStatus0,callParamsPckg,KTelephoneNumber);
 	User::WaitForRequest(reqStatus0);		// Wait for the Call Status Change notification
 	TESTL(reqStatus0==KErrNone);
	INFO_PRINTF2(_L("Result: %d"),reqStatus0.Int());
 	CHECKPOINTL(reqStatus0,KErrNone,CHP_DATA_CASE("B.1"));
 
 	INFO_PRINTF1(_L("Get Bearer Information again..."));
 	TESTL(iCall.GetBearerServiceInfo(bearerServiceInfo)==KErrNone);
 	TESTL(bearerServiceInfo.iBearerCaps==RCall::KBearerCapsCompressionV42bis);
 	TESTL(bearerServiceInfo.iBearerSpeed==RCall::EBearerData19200);
 
 	INFO_PRINTF1(_L("Check call state..."));
 	RMobileCall::TMobileCallStatus state;
 	TESTL(iCall.GetMobileCallStatus(state)==KErrNone);
 	TESTL(state==RMobileCall::EStatusConnected);
 
 	INFO_PRINTF1(_L("Check call params..."));
 	TESTL(iCall.GetCallParams(callParamsPckg)==KErrNone);
 	TESTL(callParams.iSpeakerControl==RCall::EMonitorSpeakerControlAlwaysOn);
 	TESTL(callParams.iSpeakerVolume==RCall::EMonitorSpeakerVolumeHigh);
 	TESTL(callParams.iWaitForDialTone==RCall::EDialToneWait);
 	TESTL(callParams.iInterval==1234);
 
 	INFO_PRINTF1(_L("Notification for Hscsd info changes"));
 	TRequestStatus reqStatus1;
 	iCall.NotifyHscsdInfoChange(reqStatus1,infoPckg);
 
 	INFO_PRINTF1(_L("Set DataCall Hscsd Dynamic Parameters"));
 	iCall.SetDynamicHscsdParams(reqStatus0,RMobileCall::EAiurBps57600, 15);
 	User::WaitForRequest(reqStatus0);
 	CHECKPOINTL(reqStatus0,KErrNone,CHP_DATA_CASE("B.8"));
 	User::WaitForRequest(reqStatus1);
 	CHECKPOINTL(reqStatus1,KErrNone,CHP_DATA_CASE("B.10"));
 	INFO_PRINTF1(_L("Completed notification..."));
 
 	INFO_PRINTF1(_L("Hanging up call"));
 
 	TESTL(iCall.HangUp()==KErrNone);
 
 	INFO_PRINTF1(_L("Check call state..."));
 	TESTL(iCall.GetMobileCallStatus(state)==KErrNone);
 	TESTL(state==RMobileCall::EStatusIdle);
 
 // Start another session, this time using Multimode Call Params, so re-start the
 // NTRasSimulator
 	StartNTRasSimulation();
 
 	INFO_PRINTF2(_L("Dialling %S "),&KTelephoneNumber);
 	RMobileCall::TMobileCallParamsV1 mobileCallParams;
 	RMobileCall::TMobileCallParamsV1Pckg mobileCallParamsPckg(mobileCallParams);
 	mobileCallParams.iSpeakerControl=RCall::EMonitorSpeakerControlOnExceptDuringDialling;
 	mobileCallParams.iSpeakerVolume=RCall::EMonitorSpeakerVolumeUnknown;
 	mobileCallParams.iWaitForDialTone=RCall::EDialToneNoWait;
 	mobileCallParams.iInterval=5678;
 	mobileCallParams.iAutoRedial=ETrue;
 	mobileCallParams.iCug.iCugIndex=99;
 	mobileCallParams.iIdRestrict=RMobileCall::ESendMyId;
 	iCall.Dial(reqStatus0,mobileCallParamsPckg,KTelephoneNumber);
 	User::WaitForRequest(reqStatus0);		// Wait for the Call Status Change notification
 	TESTL(reqStatus0==KErrNone);
 	CHECKPOINTL(reqStatus0,KErrNone,CHP_DATA_CASE("B.2"));
 
 	INFO_PRINTF1(_L("Check call state..."));
 	TESTL(iCall.GetMobileCallStatus(state)==KErrNone);
 	TESTL(state==RMobileCall::EStatusConnected);
 
 	INFO_PRINTF1(_L("Check call params..."));
 	TESTL(iCall.GetCallParams(mobileCallParamsPckg)==KErrNone);
 	TESTL(mobileCallParams.iSpeakerControl==RCall::EMonitorSpeakerControlOnExceptDuringDialling);
 	TESTL(mobileCallParams.iSpeakerVolume==RCall::EMonitorSpeakerVolumeUnknown);
 	TESTL(mobileCallParams.iWaitForDialTone==RCall::EDialToneNoWait);
 	TESTL(mobileCallParams.iInterval==5678);
 	TESTL(mobileCallParams.iAutoRedial/*==ETrue*/);
 	TESTL(mobileCallParams.iCug.iCugIndex==99);
 	TESTL(mobileCallParams.iIdRestrict==RMobileCall::ESendMyId);
 
 	TESTL(iCall.HangUp()==KErrNone);
 
 	INFO_PRINTF1(_L("Check call state..."));
 	TESTL(iCall.GetMobileCallStatus(state)==KErrNone);
 	TESTL(state==RMobileCall::EStatusIdle);
 	CHECKPOINTL(state,RMobileCall::EStatusIdle,CHP_DATA_CASE("B.3"));
 
 	iCall.Close();
 	iLine.Close();
 	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

