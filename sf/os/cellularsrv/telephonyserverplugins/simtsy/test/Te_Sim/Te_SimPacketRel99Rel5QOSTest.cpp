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
// Te_SimPacketRel4Rel5QOSTest.cpp
// 
//

/**
 @file
*/

#include "Te_SimPacketRel99Rel5QOSTest.h"

CSimPacketRel99Rel5QOSTest::CSimPacketRel99Rel5QOSTest() 
	{ 
	SetTestStepName(_L("Rel99Rel5QOSTest"));
	}

TVerdict CSimPacketRel99Rel5QOSTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketR4R5QOSTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(20);

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

	TRequestStatus reqStatus, notifyR99Status, notifyR5Status;

	// open Release99 Qos Profile Params
	TName qosName;
	TESTL(iRel99Packetqos.OpenNewQoS(iFirstPrimaryPacketContext, qosName)==KErrNone);
	INFO_PRINTF1(_L("Opened Rel99 QoS object"));

	// open Release5 Qos Profile Params
	TESTL(iRel5Packetqos.OpenExistingQoS(iFirstPrimaryPacketContext, qosName)==KErrNone);
	INFO_PRINTF1(_L("Opened R5 QoS object"));

	// Set Profile on Rel99 QoS, check that getting profile on Rel5 Qos gives Rel99 neg values
	// check notification on both release
	RPacketQoS::TQoSR99_R4Negotiated aR99QoSNegNotifFromR99ProfileConfig;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> aR99QoSNegNotifFromR99ProfilePckg(aR99QoSNegNotifFromR99ProfileConfig);
	iRel99Packetqos.NotifyProfileChanged(notifyR99Status,aR99QoSNegNotifFromR99ProfilePckg);

	RPacketQoS::TQoSR5Negotiated aR5QoSNegNotifFromR99ProfileConfig;
	TPckg<RPacketQoS::TQoSR5Negotiated> aR5QoSNegNotifFromR99ProfilePckg(aR5QoSNegNotifFromR99ProfileConfig);
	iRel5Packetqos.NotifyProfileChanged(notifyR5Status,aR5QoSNegNotifFromR99ProfilePckg);

	// set the profile
	TInt req = 4;
	TInt min = 2; 	
	RPacketQoS::TQoSR99_R4Requested aR99QoSReqConfig;
	TPckg<RPacketQoS::TQoSR99_R4Requested> aR99QoSReqPckg(aR99QoSReqConfig);
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
	
	iRel99Packetqos.SetProfileParameters(reqStatus, aR99QoSReqPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	// wait for notifications on both QoS
	User::WaitForRequest(notifyR99Status);
	User::WaitForRequest(notifyR5Status);

	// GetProfileParameters
	RPacketQoS::TQoSR5Negotiated aR5QoSNegConfig;
	TPckg<RPacketQoS::TQoSR5Negotiated> aR5QoSNegPckg(aR5QoSNegConfig);
	iRel5Packetqos.GetProfileParameters(reqStatus,aR5QoSNegPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	// validate profile returned by GetProfileParameters. Expect data from QosProfileNegR99 tag
	TEST(aR5QoSNegConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TEST(aR5QoSNegConfig.iTrafficClass==RPacketQoS::ETrafficClassConversational);
	TEST(aR5QoSNegConfig.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TEST(aR5QoSNegConfig.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TEST(aR5QoSNegConfig.iMaxSDUSize==min);
	TEST(aR5QoSNegConfig.iMaxRate.iUplinkRate==min);
	TEST(aR5QoSNegConfig.iMaxRate.iDownlinkRate==req);
	TEST(aR5QoSNegConfig.iBER==RPacketQoS::EBEROnePerHundred);
	TEST(aR5QoSNegConfig.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerTen);
	TEST(aR5QoSNegConfig.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority2);
	TEST(aR5QoSNegConfig.iTransferDelay==min);
	TEST(aR5QoSNegConfig.iGuaranteedRate.iUplinkRate==min);
	TEST(aR5QoSNegConfig.iGuaranteedRate.iDownlinkRate==req);	
	TEST(aR5QoSNegConfig.iSignallingIndication==EFalse);	
	TEST(aR5QoSNegConfig.iSourceStatisticsDescriptor==RPacketQoS::ESourceStatisticsDescriptorUnknown);	

	// check profile returned in the R5 notification
	TEST(aR5QoSNegNotifFromR99ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TestEquals(aR5QoSNegConfig, aR5QoSNegNotifFromR99ProfileConfig);
	// check profile returned in the R99 notification
	TEST(aR99QoSNegNotifFromR99ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TestEquals(aR5QoSNegConfig, aR99QoSNegNotifFromR99ProfileConfig);

	// wait for next notification, tigged by QosNetworkNotification 
	iRel99Packetqos.NotifyProfileChanged(notifyR99Status,aR99QoSNegNotifFromR99ProfilePckg);
	iRel5Packetqos.NotifyProfileChanged(notifyR5Status,aR5QoSNegNotifFromR99ProfilePckg);
	User::WaitForRequest(notifyR99Status);
	User::WaitForRequest(notifyR5Status);

	// GetProfileParameters
	iRel5Packetqos.GetProfileParameters(reqStatus,aR5QoSNegPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	// validate profile returned by GetProfileParameters. Expect data from QosNetworkNotification tag
	TEST(aR5QoSNegConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TEST(aR5QoSNegConfig.iTrafficClass==RPacketQoS::ETrafficClassBackground);
	TEST(aR5QoSNegConfig.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TEST(aR5QoSNegConfig.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TEST(aR5QoSNegConfig.iMaxSDUSize==req*2);
	TEST(aR5QoSNegConfig.iMaxRate.iUplinkRate==min*2);
	TEST(aR5QoSNegConfig.iMaxRate.iDownlinkRate==req*2);
	TEST(aR5QoSNegConfig.iBER==RPacketQoS::EBERFivePerThousand);
	TEST(aR5QoSNegConfig.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerThousand);
	TEST(aR5QoSNegConfig.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority3);
	TEST(aR5QoSNegConfig.iTransferDelay==min*2);
	TEST(aR5QoSNegConfig.iGuaranteedRate.iUplinkRate==min*2);
	TEST(aR5QoSNegConfig.iGuaranteedRate.iDownlinkRate==req*2);	
	TEST(aR5QoSNegConfig.iSignallingIndication==EFalse);	
	TEST(aR5QoSNegConfig.iSourceStatisticsDescriptor==RPacketQoS::ESourceStatisticsDescriptorUnknown);	

	// check profile returned in the R5 notification
	TEST(aR5QoSNegNotifFromR99ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TestEquals(aR5QoSNegConfig, aR5QoSNegNotifFromR99ProfileConfig);
	// check profile returned in the R99 notification
	TEST(aR99QoSNegNotifFromR99ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TestEquals(aR5QoSNegConfig, aR99QoSNegNotifFromR99ProfileConfig);

	// cleanup
	iRel99Packetqos.NotifyProfileChanged(notifyR99Status,aR99QoSNegNotifFromR99ProfilePckg);
	iRel99Packetqos.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(notifyR99Status);

	iRel5Packetqos.NotifyProfileChanged(notifyR5Status,aR5QoSNegNotifFromR99ProfilePckg);
	iRel5Packetqos.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(notifyR5Status);
		
	iRel5Packetqos.Close();
	iRel99Packetqos.Close();

	// Set Profile on Rel5 QoS, check that getting profile on Rel99 Qos gives Rel5 neg values
	// check notification on both release
	TESTL(iRel99Packetqos.OpenNewQoS(iFirstPrimaryPacketContext, qosName)==KErrNone);
	TESTL(iRel5Packetqos.OpenExistingQoS(iFirstPrimaryPacketContext, qosName)==KErrNone);

	RPacketQoS::TQoSR5Negotiated aR5QoSNegNotifFromR5ProfileConfig;
	TPckg<RPacketQoS::TQoSR5Negotiated> aR5QoSNegNotifFromR5ProfilePckg(aR5QoSNegNotifFromR5ProfileConfig);
	iRel5Packetqos.NotifyProfileChanged(notifyR5Status,aR5QoSNegNotifFromR5ProfilePckg);

	RPacketQoS::TQoSR99_R4Negotiated aR99QoSNegNotifFromR5ProfileConfig;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> aR99QoSNegNotifFromR5ProfilePckg(aR99QoSNegNotifFromR5ProfileConfig);
	iRel99Packetqos.NotifyProfileChanged(notifyR99Status,aR99QoSNegNotifFromR5ProfilePckg);

	// set the profile
	RPacketQoS::TQoSR5Requested aR5QoSReqConfig;
	TPckg<RPacketQoS::TQoSR5Requested> aR5QoSReqPckg(aR5QoSReqConfig);
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
	
	iRel5Packetqos.SetProfileParameters(reqStatus, aR5QoSReqPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	// wait for notifications on both QoS
	User::WaitForRequest(notifyR5Status);
	User::WaitForRequest(notifyR99Status);

	// GetProfileParameters
	RPacketQoS::TQoSR99_R4Negotiated aR99QoSNegConfig;
	TPckg<RPacketQoS::TQoSR99_R4Negotiated> aR99QoSNegPckg(aR99QoSNegConfig);
	iRel99Packetqos.GetProfileParameters(reqStatus,aR99QoSNegPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	// validate profile returned by GetProfileParameters. Expect data from QosProfileNegR99 tag
	TEST(aR99QoSNegConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TEST(aR99QoSNegConfig.iTrafficClass==RPacketQoS::ETrafficClassBackground);
	TEST(aR99QoSNegConfig.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TEST(aR99QoSNegConfig.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TEST(aR99QoSNegConfig.iMaxSDUSize==req*2);
	TEST(aR99QoSNegConfig.iMaxRate.iUplinkRate==min*2);
	TEST(aR99QoSNegConfig.iMaxRate.iDownlinkRate==req*2);
	TEST(aR99QoSNegConfig.iBER==RPacketQoS::EBERFivePerThousand);
	TEST(aR99QoSNegConfig.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerThousand);
	TEST(aR99QoSNegConfig.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority3);
	TEST(aR99QoSNegConfig.iTransferDelay==min*2);
	TEST(aR99QoSNegConfig.iGuaranteedRate.iUplinkRate==min*2);
	TEST(aR99QoSNegConfig.iGuaranteedRate.iDownlinkRate==req*2);	

	// check profile returned in the R99 notification
	TEST(aR99QoSNegNotifFromR5ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TestEquals(aR99QoSNegConfig, aR99QoSNegNotifFromR5ProfileConfig);
	// check profile returned in the R5 notification
	TEST(aR5QoSNegNotifFromR5ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TestEquals(aR99QoSNegConfig, aR5QoSNegNotifFromR5ProfileConfig);

	// wait for next notification, tigged by QosNetworkNotification 
	iRel5Packetqos.NotifyProfileChanged(notifyR5Status,aR5QoSNegNotifFromR5ProfilePckg);
	iRel99Packetqos.NotifyProfileChanged(notifyR99Status,aR99QoSNegNotifFromR5ProfilePckg);
	User::WaitForRequest(notifyR5Status);
	User::WaitForRequest(notifyR99Status);

	// GetProfileParameters
	iRel99Packetqos.GetProfileParameters(reqStatus,aR99QoSNegPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);

	// validate profile returned by GetProfileParameters. Expect data from QosR5NetworkNotification tag
	TEST(aR99QoSNegConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TEST(aR99QoSNegConfig.iTrafficClass==RPacketQoS::ETrafficClassInteractive); //0x08
	TEST(aR99QoSNegConfig.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired); // 0x04
	TEST(aR99QoSNegConfig.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUDeliveryNotRequired); //0x08
	TEST(aR99QoSNegConfig.iMaxSDUSize==req);
	TEST(aR99QoSNegConfig.iMaxRate.iUplinkRate==min);
	TEST(aR99QoSNegConfig.iMaxRate.iDownlinkRate==req);
	TEST(aR99QoSNegConfig.iBER==RPacketQoS::EBERFivePerHundred); // 0x02
	TEST(aR99QoSNegConfig.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerHundredThousand); //0x40
	TEST(aR99QoSNegConfig.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority2); // 0x04
	TEST(aR99QoSNegConfig.iTransferDelay==min);
	TEST(aR99QoSNegConfig.iGuaranteedRate.iUplinkRate==min);
	TEST(aR99QoSNegConfig.iGuaranteedRate.iDownlinkRate==req);	

	// check profile returned in the R5 notification
	TEST(aR5QoSNegNotifFromR99ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TestEquals(aR99QoSNegConfig, aR5QoSNegNotifFromR5ProfileConfig);
	// check profile returned in the R99 notification
	TEST(aR99QoSNegNotifFromR99ProfileConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TestEquals(aR99QoSNegConfig, aR99QoSNegNotifFromR5ProfileConfig);

	// cleanup
	iRel99Packetqos.NotifyProfileChanged(notifyR99Status,aR99QoSNegNotifFromR5ProfilePckg);
	iRel99Packetqos.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(notifyR99Status);

	iRel5Packetqos.NotifyProfileChanged(notifyR5Status,aR5QoSNegNotifFromR5ProfilePckg);
	iRel5Packetqos.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(notifyR5Status);
		

	// End of RPacketQos tests
	iRel5Packetqos.Close();
	iRel99Packetqos.Close();
	iFirstPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}


void CSimPacketRel99Rel5QOSTest::TestEquals(const RPacketQoS::TQoSR99_R4Negotiated& aProfile1, const RPacketQoS::TQoSR99_R4Negotiated& aProfile2)
	{
	TEST(aProfile1.iTrafficClass==aProfile2.iTrafficClass);
	TEST(aProfile1.iDeliveryOrderReqd==aProfile2.iDeliveryOrderReqd);
	TEST(aProfile1.iDeliverErroneousSDU==aProfile2.iDeliverErroneousSDU);
	TEST(aProfile1.iMaxSDUSize==aProfile2.iMaxSDUSize);
	TEST(aProfile1.iMaxRate.iUplinkRate==aProfile2.iMaxRate.iUplinkRate);
	TEST(aProfile1.iMaxRate.iDownlinkRate==aProfile2.iMaxRate.iDownlinkRate);
	TEST(aProfile1.iBER==aProfile2.iBER);
	TEST(aProfile1.iSDUErrorRatio==aProfile2.iSDUErrorRatio);
	TEST(aProfile1.iTrafficHandlingPriority==aProfile2.iTrafficHandlingPriority);
	TEST(aProfile1.iTransferDelay==aProfile2.iTransferDelay);
	TEST(aProfile1.iGuaranteedRate.iUplinkRate==aProfile2.iGuaranteedRate.iUplinkRate);
	TEST(aProfile1.iGuaranteedRate.iDownlinkRate==aProfile2.iGuaranteedRate.iDownlinkRate);	
	}
