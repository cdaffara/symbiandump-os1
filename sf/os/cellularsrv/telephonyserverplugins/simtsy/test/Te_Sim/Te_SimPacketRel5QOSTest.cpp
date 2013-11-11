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

#include "Te_SimPacketRel5QOSTest.h"

CSimPacketRel5QOSTest::CSimPacketRel5QOSTest() 
	{ 
	SetTestStepName(_L("Rel5QOSTest"));
	}

TVerdict CSimPacketRel5QOSTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketRel5QOSTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

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

	// set Release5 Qos Profile Params
	TName r5QosName;
	TESTL(iRel5Packetqos.OpenNewQoS(iFirstPrimaryPacketContext, r5QosName)==KErrNone);
	TName r5QosNameCompare;
	r5QosNameCompare.Append(KSimPacketQosName);
	r5QosNameCompare.AppendNum(1);
	TESTL(r5QosName.Compare(r5QosNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Rel5 QoS object"));

	TName qosName;
	iFirstPrimaryPacketContext.GetProfileName(qosName);
	INFO_PRINTF1(_L("Retrieved Rel5 QoS object reference name"));

	RPacketQoS::TQoSR5Requested aR5QoSReqConfig;
	TPckg<RPacketQoS::TQoSR5Requested> aR5QoSReqPckg(aR5QoSReqConfig);

	RPacketQoS::TQoSR5Negotiated aR5NotifyQoS;
	TPckg<RPacketQoS::TQoSR5Negotiated> aR5NotifyQoSPckg(aR5NotifyQoS);

	TInt req = 4;
	TInt min = 2; 
	TInt sigOff = 0;
	TInt sigOn  = 1;

	//Profile data
	aR5QoSReqConfig.iReqTrafficClass = RPacketQoS::ETrafficClassStreaming; // 0x04
	aR5QoSReqConfig.iMinTrafficClass = RPacketQoS::ETrafficClassConversational; // 0x02
	aR5QoSReqConfig.iReqDeliveryOrderReqd = RPacketQoS::EDeliveryOrderNotRequired; // 0x04
	aR5QoSReqConfig.iMinDeliveryOrderReqd = RPacketQoS::EDeliveryOrderRequired;	// 0x02
	aR5QoSReqConfig.iReqDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryNotRequired; // 0x08
	aR5QoSReqConfig.iMinDeliverErroneousSDU = RPacketQoS::EErroneousSDUDeliveryRequired; // 0x04
	aR5QoSReqConfig.iReqMaxSDUSize = req; // 4	
	aR5QoSReqConfig.iMinAcceptableMaxSDUSize = min; // 2
	aR5QoSReqConfig.iReqMaxRate.iUplinkRate = min;	// between 0 - 1840
	aR5QoSReqConfig.iReqMaxRate.iDownlinkRate = min;
	aR5QoSReqConfig.iMinAcceptableMaxRate.iDownlinkRate = min-1; // between 0 - 1840
	aR5QoSReqConfig.iMinAcceptableMaxRate.iUplinkRate = req;
	aR5QoSReqConfig.iReqBER = RPacketQoS::EBEROnePerHundred;	// 0x04
	aR5QoSReqConfig.iMaxBER = RPacketQoS::EBERFivePerHundred;	// 0x02
	aR5QoSReqConfig.iReqSDUErrorRatio = RPacketQoS::ESDUErrorRatioSevenPerThousand; // 0x08
	aR5QoSReqConfig.iMaxSDUErrorRatio = RPacketQoS::ESDUErrorRatioOnePerHundred;	// 0x04
	aR5QoSReqConfig.iReqTrafficHandlingPriority = RPacketQoS::ETrafficPriority2; // 0x04
	aR5QoSReqConfig.iMinTrafficHandlingPriority = RPacketQoS::ETrafficPriority1; // 0x02
	aR5QoSReqConfig.iReqTransferDelay = req;
	aR5QoSReqConfig.iMaxTransferDelay = min;
	aR5QoSReqConfig.iReqGuaranteedRate.iDownlinkRate = min;
	aR5QoSReqConfig.iReqGuaranteedRate.iUplinkRate = req;
	aR5QoSReqConfig.iMinGuaranteedRate.iDownlinkRate = min;
	aR5QoSReqConfig.iMinGuaranteedRate.iUplinkRate = req;

	aR5QoSReqConfig.iSignallingIndication = sigOff;
	aR5QoSReqConfig.iSourceStatisticsDescriptor = RPacketQoS::ESourceStatisticsDescriptorUnknown;
 
	//post the notification
	iRel5Packetqos.NotifyProfileChanged(notifyStatus, aR5NotifyQoSPckg);
	//Set the qos
	iRel5Packetqos.SetProfileParameters(reqStatus, aR5QoSReqPckg);

	User::WaitForRequest(notifyStatus);

	TESTL(notifyStatus.Int()==KErrNone);

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Set Rel5 Profile Test passed"));

	TESTL(aR5NotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TESTL(aR5NotifyQoS.iTrafficClass==RPacketQoS::ETrafficClassConversational);
	TESTL(aR5NotifyQoS.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TESTL(aR5NotifyQoS.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TESTL(aR5NotifyQoS.iMaxSDUSize==min);
	TESTL(aR5NotifyQoS.iMaxRate.iUplinkRate==min);
	TESTL(aR5NotifyQoS.iMaxRate.iDownlinkRate==req);
	TESTL(aR5NotifyQoS.iBER==RPacketQoS::EBEROnePerHundred);
	TESTL(aR5NotifyQoS.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerTen);
	TESTL(aR5NotifyQoS.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority2);
	TESTL(aR5NotifyQoS.iTransferDelay==min);
	TESTL(aR5NotifyQoS.iGuaranteedRate.iUplinkRate==min);
	TESTL(aR5NotifyQoS.iGuaranteedRate.iDownlinkRate==req);

	TESTL(aR5NotifyQoS.iSignallingIndication==sigOff);
	TESTL(aR5NotifyQoS.iSourceStatisticsDescriptor==RPacketQoS::ESourceStatisticsDescriptorUnknown);

	// post a notification
	iRel5Packetqos.NotifyProfileChanged(cancelStatus, aR5NotifyQoSPckg);	
	iRel5Packetqos.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(cancelStatus);

	TESTL(cancelStatus.Int()==KErrCancel);
	INFO_PRINTF1(_L("Set Profile Test passed"));

	// need to call Notify first to set pending to true after 
	// cancel event which sets it to false	
	iRel5Packetqos.NotifyProfileChanged(notifyStatus, aR5NotifyQoSPckg);	
	User::WaitForRequest(notifyStatus);

	// Get QoS Rel5 Capabilities
	RPacketQoS::TQoSCapsR5 aQoSR5Caps;
	TPckg<RPacketQoS::TQoSCapsR5> aQoSR5CapsPckg(aQoSR5Caps);
	
	iRel5Packetqos.GetProfileCapabilities(reqStatus, aQoSR5CapsPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(aQoSR5Caps.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TESTL(aQoSR5Caps.iTrafficClass == RPacketQoS::ETrafficClassInteractive);
	TESTL(aQoSR5Caps.iDeliveryOrderReqd == RPacketQoS::EDeliveryOrderNotRequired);
	TESTL(aQoSR5Caps.iDeliverErroneousSDU == RPacketQoS::EErroneousSDUNoDetection);
	TESTL(aQoSR5Caps.iBER == RPacketQoS::EBEROnePerHundred);
	TESTL(aQoSR5Caps.iSDUErrorRatio == RPacketQoS::ESDUErrorRatioOnePerTen);
	TESTL(aQoSR5Caps.iTrafficHandlingPriority == RPacketQoS::ETrafficPriority2);

	TESTL(aQoSR5Caps.iSignallingIndication==sigOff);
	TESTL(aQoSR5Caps.iSourceStatisticsDescriptor==RPacketQoS::ESourceStatisticsDescriptorUnknown);
	INFO_PRINTF1(_L("Get QoS Profile  Capabilities Test passed"));

	//Get QoS Profile Params
	iRel5Packetqos.GetProfileParameters(cancelStatus, aR5NotifyQoSPckg);
	iRel5Packetqos.CancelAsyncRequest(EPacketQoSGetProfileParams);//no implementation-does nothing
	User::WaitForRequest(cancelStatus);
	TESTL(cancelStatus.Int()==KErrNone);
	User::After(1000000L);

	// test RPacketContext::Activate - before calling ModifyActiveContext
	StartNTRasSimulation();
	iFirstPrimaryPacketContext.Activate(reqStatus);
	User::WaitForRequest(reqStatus);	
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Activate test passed"));	
	
	// test RPacketContext::ModifyActiveContext
	iFirstPrimaryPacketContext.ModifyActiveContext(reqStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNotReady); 
	INFO_PRINTF1(_L("ModifyActiveContext Test Passed"));
	INFO_PRINTF1(_L("Notify Rel5 QoS Profile Test passed"));

	iRel5Packetqos.GetProfileParameters(reqStatus, aR5NotifyQoSPckg);
	User::WaitForRequest(reqStatus);

	TESTL(aR5NotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigRel5);
	TESTL(aR5NotifyQoS.iTrafficClass==RPacketQoS::ETrafficClassConversational);
	TESTL(aR5NotifyQoS.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TESTL(aR5NotifyQoS.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TESTL(aR5NotifyQoS.iMaxSDUSize==min);
	TESTL(aR5NotifyQoS.iMaxRate.iUplinkRate==min);
	TESTL(aR5NotifyQoS.iMaxRate.iDownlinkRate==req);
	TESTL(aR5NotifyQoS.iBER==RPacketQoS::EBEROnePerHundred);
	TESTL(aR5NotifyQoS.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerTen);
	TESTL(aR5NotifyQoS.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority2);
	TESTL(aR5NotifyQoS.iTransferDelay==min);
	TESTL(aR5NotifyQoS.iGuaranteedRate.iUplinkRate==min);
	TESTL(aR5NotifyQoS.iGuaranteedRate.iDownlinkRate==req);	

	TESTL(aR5NotifyQoS.iSignallingIndication==sigOn);
	TESTL(aR5NotifyQoS.iSourceStatisticsDescriptor==RPacketQoS::ESourceStatisticsDescriptorSpeech);

	// End of RPacketQos tests
	iRel5Packetqos.Close();
	iFirstPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

