// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Te_SimPacketRel99Rel5QOSPubSubNotifTest.h"

CSimPacketRel99Rel5QOSPubSubNotifTest::CSimPacketRel99Rel5QOSPubSubNotifTest() 
	{ 
	SetTestStepName(_L("Rel99Rel5QOSPubSubNotifTest"));
	}

TVerdict CSimPacketRel99Rel5QOSPubSubNotifTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Begin Rel99Rel5QOSPubSubNotifTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(21);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));
	TESTL(iPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));
	TName contextName;
	TName contextNameCompare;
	TESTL(iFirstPrimaryPacketContext.OpenNewContext(iPacketService, contextName)==KErrNone);
	contextNameCompare.Append(KSimPrimaryPacketContextName);
	contextNameCompare.AppendNum(1);
	TESTL(contextName.Compare(contextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Context object"));

	TRequestStatus reqStatus, notifyStatus, cancelStatus;

	// R99 test
	TName r99QosName;
	TESTL(iRel99Packetqos.OpenNewQoS(iFirstPrimaryPacketContext, r99QosName)==KErrNone);
	RPacketQoS::TQoSR99_R4Requested aR99QoSReqConfig;
	TPckg<RPacketQoS::TQoSR99_R4Requested> aR99QoSReqPckg(aR99QoSReqConfig);

	RPacketQoS::TQoSR99_R4Negotiated aR99NotifyQoS;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> aR99NotifyQoSPckg(aR99NotifyQoS);

	TInt req = 4;
	TInt min = 2; 

	//Profile data
	aR99QoSReqConfig.iReqTrafficClass = RPacketQoS::ETrafficClassStreaming; // 0x04
	aR99QoSReqConfig.iMinTrafficClass = RPacketQoS::ETrafficClassConversational; // 0x02
	aR99QoSReqConfig.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderNotRequired; // 0x04
	aR99QoSReqConfig.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;	// 0x02
	aR99QoSReqConfig.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired; // 0x08
	aR99QoSReqConfig.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired; // 0x04
	aR99QoSReqConfig.iReqMaxSDUSize = req; // 4	
	aR99QoSReqConfig.iMinAcceptableMaxSDUSize = min; // 2
	aR99QoSReqConfig.iReqMaxRate.iUplinkRate = min;	// between 0 - 1840
	aR99QoSReqConfig.iReqMaxRate.iDownlinkRate = min;
	aR99QoSReqConfig.iMinAcceptableMaxRate.iDownlinkRate = min-1; // between 0 - 1840
	aR99QoSReqConfig.iMinAcceptableMaxRate.iUplinkRate = req;
	aR99QoSReqConfig.iReqBER = RPacketQoS::EBEROnePerHundred;	// 0x04
	aR99QoSReqConfig.iMaxBER = RPacketQoS::EBERFivePerHundred;	// 0x02
	aR99QoSReqConfig.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioSevenPerThousand; // 0x08
	aR99QoSReqConfig.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundred;	// 0x04
	aR99QoSReqConfig.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority2; // 0x04
	aR99QoSReqConfig.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority1; // 0x02
	aR99QoSReqConfig.iReqTransferDelay = req;
	aR99QoSReqConfig.iMaxTransferDelay = min;
	aR99QoSReqConfig.iReqGuaranteedRate.iDownlinkRate = min;
	aR99QoSReqConfig.iReqGuaranteedRate.iUplinkRate = req;
	aR99QoSReqConfig.iMinGuaranteedRate.iDownlinkRate = min;
	aR99QoSReqConfig.iMinGuaranteedRate.iUplinkRate = req;
 
	// Set the qos
	iRel99Packetqos.SetProfileParameters(reqStatus, aR99QoSReqPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	// post the notification
	iRel99Packetqos.NotifyProfileChanged(notifyStatus, aR99NotifyQoSPckg);
	
	// Trig QoS change
	ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyNetworkQoSChange, 0);
	TESTL(ret == KErrNone);
	
	// wait for the notification
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);

	// validate profile. Expect data from QosR5NetworkNotification tag
	TEST(aR99NotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TEST(aR99NotifyQoS.iTrafficClass==RPacketQoS::ETrafficClassBackground);
	TEST(aR99NotifyQoS.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TEST(aR99NotifyQoS.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TEST(aR99NotifyQoS.iMaxSDUSize==req*2);
	TEST(aR99NotifyQoS.iMaxRate.iUplinkRate==min*2);
	TEST(aR99NotifyQoS.iMaxRate.iDownlinkRate==req*2);
	TEST(aR99NotifyQoS.iBER==RPacketQoS::EBERFivePerThousand);
	TEST(aR99NotifyQoS.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerThousand);
	TEST(aR99NotifyQoS.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority3);
	TEST(aR99NotifyQoS.iTransferDelay==min*2);
	TEST(aR99NotifyQoS.iGuaranteedRate.iUplinkRate==min*2);
	TEST(aR99NotifyQoS.iGuaranteedRate.iDownlinkRate==req*2);	

	// cleanup
	iRel99Packetqos.Close();

	// R5 test
	TName r5QosName;
	TESTL(iRel5Packetqos.OpenNewQoS(iFirstPrimaryPacketContext, r5QosName)==KErrNone);
	RPacketQoS::TQoSR5Requested aR5QoSReqConfig;
	TPckg<RPacketQoS::TQoSR5Requested> aR5QoSReqPckg(aR5QoSReqConfig);

	RPacketQoS::TQoSR5Negotiated aR5NotifyQoS;
	TPckg<RPacketQoS::TQoSR5Negotiated> aR5NotifyQoSPckg(aR5NotifyQoS);

	//Profile data
	aR5QoSReqConfig.iReqTrafficClass = RPacketQoS::ETrafficClassInteractive; // 0x08
	aR5QoSReqConfig.iMinTrafficClass = RPacketQoS::ETrafficClassBackground; // 0x10
	aR5QoSReqConfig.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;	// 0x02
	aR5QoSReqConfig.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderNotRequired; // 0x04
	aR5QoSReqConfig.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUNoDetection; // 0x02
	aR5QoSReqConfig.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryUnspecified; // 0x01
	aR5QoSReqConfig.iReqMaxSDUSize = req*2; // 8	
	aR5QoSReqConfig.iMinAcceptableMaxSDUSize = min*2; // 4
	aR5QoSReqConfig.iReqMaxRate.iUplinkRate = min*2;	// between 0 - 1840
	aR5QoSReqConfig.iReqMaxRate.iDownlinkRate = min*2;
	aR5QoSReqConfig.iMinAcceptableMaxRate.iDownlinkRate = (min-1)*2; // between 0 - 1840
	aR5QoSReqConfig.iMinAcceptableMaxRate.iUplinkRate = req*2;
	aR5QoSReqConfig.iReqBER = RPacketQoS::EBERFourPerThousand; // 0x10
	aR5QoSReqConfig.iMaxBER = RPacketQoS::EBERFivePerThousand; // 0x08
	aR5QoSReqConfig.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerTenThousand; // 0x20
	aR5QoSReqConfig.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerThousand; // 0x10
	aR5QoSReqConfig.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority3; // 0x08
	aR5QoSReqConfig.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority2; // 0x04
	aR5QoSReqConfig.iReqTransferDelay = req*2;
	aR5QoSReqConfig.iMaxTransferDelay = min*2;
	aR5QoSReqConfig.iReqGuaranteedRate.iDownlinkRate = min*2;
	aR5QoSReqConfig.iReqGuaranteedRate.iUplinkRate = req*2;
	aR5QoSReqConfig.iMinGuaranteedRate.iDownlinkRate = min*2;
	aR5QoSReqConfig.iMinGuaranteedRate.iUplinkRate = req*2;
	aR5QoSReqConfig.iSignallingIndication = ETrue;			 
	aR5QoSReqConfig.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorSpeech;
 
	// Set the qos
	iRel5Packetqos.SetProfileParameters(reqStatus, aR5QoSReqPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	// post the notification
	iRel5Packetqos.NotifyProfileChanged(notifyStatus, aR5NotifyQoSPckg);
	
	// Trig QoS change
	ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyNetworkQoSChange, 0);
	TESTL(ret == KErrNone);
	
	// wait for the notification
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);

	// validate profile. Expect data from QosR5NetworkNotification tag
	TEST(aR5NotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TEST(aR5NotifyQoS.iTrafficClass==RPacketQoS::ETrafficClassInteractive); //0x08
	TEST(aR5NotifyQoS.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired); // 0x04
	TEST(aR5NotifyQoS.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUDeliveryNotRequired); //0x08
	TEST(aR5NotifyQoS.iMaxSDUSize==req);
	TEST(aR5NotifyQoS.iMaxRate.iUplinkRate==min);
	TEST(aR5NotifyQoS.iMaxRate.iDownlinkRate==req);
	TEST(aR5NotifyQoS.iBER==RPacketQoS::EBERFivePerHundred); // 0x02
	TEST(aR5NotifyQoS.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerHundredThousand); //0x40
	TEST(aR5NotifyQoS.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority2); // 0x04
	TEST(aR5NotifyQoS.iTransferDelay==min);
	TEST(aR5NotifyQoS.iGuaranteedRate.iUplinkRate==min);
	TEST(aR5NotifyQoS.iGuaranteedRate.iDownlinkRate==req);	
	TEST(aR5NotifyQoS.iSignallingIndication);			 
	TEST(aR5NotifyQoS.iSourceStatisticsDescriptor == RPacketQoS::ESourceStatisticsDescriptorSpeech);

	// cleanup
	iRel5Packetqos.Close();

	// End of RPacketQos tests
	iFirstPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

