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

/**
 @file
*/

#include "Te_SimPacketQoS.h"

void CSimPacketQoSTest::TestFixtureLC(TUint aTestNumber)
/**
 * Used to setup the test case 
 * Any test data or environment is configured here
 */ 
{
	//Setup TSY config to use config3.txt
	CreateConfigFileL(_L("c:\\config3.txt"));
	SetTestNumberL(aTestNumber);
	
	TInt ret = iPhone.Open(iTelServer,KPhoneName);
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	
	//Create the packet connection
	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));
	
	//Open a context
	TName contextName;
	TESTL(iPacketContext.OpenNewContext(iPacketService, contextName)==KErrNone);

	// set Release99 Qos Profile Params
	TName r99QosName;
	TESTL(iPacketQoS.OpenNewQoS(iPacketContext, r99QosName)==KErrNone);
	INFO_PRINTF1(_L("Opened Rel99 QoS object"));

	//Setting for QoS
	iR99QoSParameters.iReqTrafficClass = RPacketQoS::ETrafficClassStreaming; // 0x04
	iR99QoSParameters.iMinTrafficClass = RPacketQoS::ETrafficClassConversational; // 0x02
	iR99QoSParameters.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderNotRequired; // 0x04
	iR99QoSParameters.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;	// 0x02
	iR99QoSParameters.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired; // 0x08
	iR99QoSParameters.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired; // 0x04
	iR99QoSParameters.iReqMaxSDUSize = KReq; // 4	
	iR99QoSParameters.iMinAcceptableMaxSDUSize = KMin; // 2
	iR99QoSParameters.iReqMaxRate.iUplinkRate = KMin;	// between 0 - 1840
	iR99QoSParameters.iReqMaxRate.iDownlinkRate = KMin;
	iR99QoSParameters.iMinAcceptableMaxRate.iUplinkRate = KReq;
	iR99QoSParameters.iMinAcceptableMaxRate.iDownlinkRate = KMin-1; // between 0 - 1840	
	iR99QoSParameters.iReqBER = RPacketQoS::EBEROnePerHundred;	// 0x04
	iR99QoSParameters.iMaxBER = RPacketQoS::EBERFivePerHundred;	// 0x02
	iR99QoSParameters.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioSevenPerThousand; // 0x08
	iR99QoSParameters.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundred;	// 0x04
	iR99QoSParameters.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority2; // 0x04
	iR99QoSParameters.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority1; // 0x02
	iR99QoSParameters.iReqTransferDelay = KReq;
	iR99QoSParameters.iMaxTransferDelay = KMin;
	iR99QoSParameters.iReqGuaranteedRate.iUplinkRate = KReq;
	iR99QoSParameters.iReqGuaranteedRate.iDownlinkRate = KMin;
	iR99QoSParameters.iMinGuaranteedRate.iUplinkRate = KReq;
	iR99QoSParameters.iMinGuaranteedRate.iDownlinkRate = KMin;

	INFO_PRINTF1(_L("Populated Rel99 QoS parameters"));
}

//Use for tearing down the test case
void CSimPacketQoSTest::TearDownL()
/**
 * Used to setup the test case 
 * Any unloading or closing is here
 */ 
{
	iPacketQoS.Close();
	iPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
}


CSetQoSDelayOnFirstSet::CSetQoSDelayOnFirstSet() 
	{ 
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SetQoSDelayOnFirstSet"));
	}

 TVerdict CSetQoSDelayOnFirstSet::doTestStepL()
/**

*/
{	
	// This test has been disabled whilst a submission goes in to MCL
	/*
	TestFixtureLC(12);
	TRequestStatus reqStatus;
	TPckg<RPacketQoS::TQoSR99_R4Requested> r99QoSRequestPckg(iR99QoSParameters);

	//Get a time stamp
	TTime startTime;
	startTime.UniversalTime();

	//Set the QoS profiles
	iPacketQoS.SetProfileParameters(reqStatus, r99QoSRequestPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//Get a time stamp
	TTime endTime;
	endTime.UniversalTime();
	TTimeIntervalSeconds secondsElapsed;
	endTime.SecondsFrom(startTime, secondsElapsed);
	TESTL(secondsElapsed.Int()==5);
	
	TearDownL();
	*/
	return TestStepResult();
}

CSetQoSDelayOnThirdSet::CSetQoSDelayOnThirdSet() 
	{ 
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SetQoSDelayOnThirdSet"));
	}

 TVerdict CSetQoSDelayOnThirdSet::doTestStepL()
/**

*/
{	
	TestFixtureLC(13);
	TRequestStatus reqStatus;
	TPckg<RPacketQoS::TQoSR99_R4Requested> r99QoSRequestPckg(iR99QoSParameters);

	//Set the qos
	iPacketQoS.SetProfileParameters(reqStatus, r99QoSRequestPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	//Set the qos again (2)
	iR99QoSParameters.iMinGuaranteedRate.iDownlinkRate = KMin -1;
	iPacketQoS.SetProfileParameters(reqStatus, r99QoSRequestPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	//Get a time stamp
	TTime startTime;
	startTime.UniversalTime();
	//Set the qos again (3)	
	TRequestStatus requestWhileDelay;
	iR99QoSParameters.iMinGuaranteedRate.iDownlinkRate = KMin;
	iR99QoSParameters.iReqGuaranteedRate.iDownlinkRate = KMin-1;
	iPacketQoS.SetProfileParameters(requestWhileDelay, r99QoSRequestPckg);
	User::WaitForRequest(requestWhileDelay);
	TESTL(requestWhileDelay.Int()==KErrNone);
	//Get another time stamp
	TTime endTime;
	endTime.UniversalTime();
	TTimeIntervalSeconds secondsElapsed;
	endTime.SecondsFrom(startTime, secondsElapsed);
	TESTL(secondsElapsed.Int()==7);

	TearDownL();

	return TestStepResult();
}

CSetQoSDelayUnset::CSetQoSDelayUnset() 
	{ 
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SetQoSDelayUnset"));
	}

 TVerdict CSetQoSDelayUnset::doTestStepL()
/**

*/
{	
	TestFixtureLC(14);
	TRequestStatus reqStatus;
	TPckg<RPacketQoS::TQoSR99_R4Requested> r99QoSRequestPckg(iR99QoSParameters);

	//Get a time stamp
	TTime startTime;
	startTime.UniversalTime();

	//Set the QoS profiles
	iPacketQoS.SetProfileParameters(reqStatus, r99QoSRequestPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//Get a time stamp
	TTime endTime;
	endTime.UniversalTime();
	TTimeIntervalSeconds secondsElapsed;
	endTime.SecondsFrom(startTime, secondsElapsed);
	TESTL(secondsElapsed.Int()==0);
	
	TearDownL();

	return TestStepResult();
}

CSetQoSNegativeDelay::CSetQoSNegativeDelay() 
	{ 
	// store the name of this test case
	// this is the name that is used by the script file
	SetTestStepName(_L("SetQoSNegativeDelay"));
	}

 TVerdict CSetQoSNegativeDelay::doTestStepL()
/**

*/
{	
	TestFixtureLC(15);
	TRequestStatus reqStatus;
	TPckg<RPacketQoS::TQoSR99_R4Requested> r99QoSRequestPckg(iR99QoSParameters);

	//Get a time stamp
	TTime startTime;
	startTime.UniversalTime();

	//Set the QoS profiles
	iPacketQoS.SetProfileParameters(reqStatus, r99QoSRequestPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	//Get a time stamp
	TTime endTime;
	endTime.UniversalTime();
	TTimeIntervalSeconds secondsElapsed;
	endTime.SecondsFrom(startTime, secondsElapsed);
	TESTL(secondsElapsed.Int()==0);
	
	TearDownL();

	return TestStepResult();
}


