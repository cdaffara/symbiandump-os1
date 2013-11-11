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

#include "Te_SimPacketRel99QOSTest.h"

CSimPacketRel99QOSTest::CSimPacketRel99QOSTest() 
	{ 
	SetTestStepName(_L("Rel99QOSTest"));
	}

TVerdict CSimPacketRel99QOSTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketRel99QOSTest"));

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

	// set Release99 Qos Profile Params
	TName r99QosName;
	TESTL(iRel99Packetqos.OpenNewQoS(iFirstPrimaryPacketContext, r99QosName)==KErrNone);
	TName r99QosNameCompare;
	r99QosNameCompare.Append(KSimPacketQosName);
	r99QosNameCompare.AppendNum(1);
	TESTL(r99QosName.Compare(r99QosNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Rel99 QoS object"));

	TName qosName;
	iFirstPrimaryPacketContext.GetProfileName(qosName);
	INFO_PRINTF1(_L("Retrieved Rel99 QoS object reference name"));

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
 
	//post the notification
	iRel99Packetqos.NotifyProfileChanged(notifyStatus, aR99NotifyQoSPckg);
	//Set the qos
	iRel99Packetqos.SetProfileParameters(reqStatus, aR99QoSReqPckg);

	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);

	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Set Rel99 Profile Test passed"));

	TESTL(aR99NotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TESTL(aR99NotifyQoS.iTrafficClass==RPacketQoS::ETrafficClassConversational);
	TESTL(aR99NotifyQoS.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TESTL(aR99NotifyQoS.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TESTL(aR99NotifyQoS.iMaxSDUSize==min);
	TESTL(aR99NotifyQoS.iMaxRate.iUplinkRate==min);
	TESTL(aR99NotifyQoS.iMaxRate.iDownlinkRate==req);
	TESTL(aR99NotifyQoS.iBER==RPacketQoS::EBEROnePerHundred);
	TESTL(aR99NotifyQoS.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerTen);
	TESTL(aR99NotifyQoS.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority2);
	TESTL(aR99NotifyQoS.iTransferDelay==min);
	TESTL(aR99NotifyQoS.iGuaranteedRate.iUplinkRate==min);
	TESTL(aR99NotifyQoS.iGuaranteedRate.iDownlinkRate==req);

	// post a notification
	iRel99Packetqos.NotifyProfileChanged(cancelStatus, aR99NotifyQoSPckg);	
	iRel99Packetqos.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(cancelStatus);

	TESTL(cancelStatus.Int()==KErrCancel);
	INFO_PRINTF1(_L("Set Profile Test passed"));

	// need to call Notify first to set pending to true after 
	// cancel event which sets it to false	
	iRel99Packetqos.NotifyProfileChanged(notifyStatus, aR99NotifyQoSPckg);	
	User::WaitForRequest(notifyStatus);

	// Get QoS Rel99 Capabilities
	RPacketQoS::TQoSCapsR99_R4 aQoSCaps;
	TPckg<RPacketQoS::TQoSCapsR99_R4> aQoSCapsPckg(aQoSCaps);

	iRel99Packetqos.GetProfileCapabilities(reqStatus, aQoSCapsPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	
	TESTL(aQoSCaps.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TESTL(aQoSCaps.iTrafficClass == RPacketQoS::ETrafficClassInteractive);
	TESTL(aQoSCaps.iDeliveryOrderReqd == RPacketQoS::EDeliveryOrderNotRequired);
	TESTL(aQoSCaps.iDeliverErroneousSDU == RPacketQoS::EErroneousSDUNoDetection);
	TESTL(aQoSCaps.iBER == RPacketQoS::EBEROnePerHundred);
	TESTL(aQoSCaps.iSDUErrorRatio == RPacketQoS::ESDUErrorRatioOnePerTen);
	TESTL(aQoSCaps.iTrafficHandlingPriority == RPacketQoS::ETrafficPriority2);
	INFO_PRINTF1(_L("Get QoS Profile  Capabilities Test passed"));

	//Get QoS Profile Params
	iRel99Packetqos.GetProfileParameters(cancelStatus, aR99NotifyQoSPckg);
	iRel99Packetqos.CancelAsyncRequest(EPacketQoSGetProfileParams);//no implementation-does nothing
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
	INFO_PRINTF1(_L("Notify Rel99 QoS Profile Test passed"));

	iRel99Packetqos.GetProfileParameters(reqStatus, aR99NotifyQoSPckg);
	User::WaitForRequest(reqStatus);

	TESTL(aR99NotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
	TESTL(aR99NotifyQoS.iTrafficClass==RPacketQoS::ETrafficClassConversational);
	TESTL(aR99NotifyQoS.iDeliveryOrderReqd==RPacketQoS::EDeliveryOrderNotRequired);
	TESTL(aR99NotifyQoS.iDeliverErroneousSDU==RPacketQoS::EErroneousSDUNoDetection);
	TESTL(aR99NotifyQoS.iMaxSDUSize==min);
	TESTL(aR99NotifyQoS.iMaxRate.iUplinkRate==min);
	TESTL(aR99NotifyQoS.iMaxRate.iDownlinkRate==req);
	TESTL(aR99NotifyQoS.iBER==RPacketQoS::EBEROnePerHundred);
	TESTL(aR99NotifyQoS.iSDUErrorRatio==RPacketQoS::ESDUErrorRatioOnePerTen);
	TESTL(aR99NotifyQoS.iTrafficHandlingPriority==RPacketQoS::ETrafficPriority2);
	TESTL(aR99NotifyQoS.iTransferDelay==min);
	TESTL(aR99NotifyQoS.iGuaranteedRate.iUplinkRate==min);
	TESTL(aR99NotifyQoS.iGuaranteedRate.iDownlinkRate==req);	

	INFO_PRINTF1(_L("Get Rel99 QoS Profile Test passed"));

	// End of RPacketQos tests
	iRel99Packetqos.Close();
	iFirstPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

