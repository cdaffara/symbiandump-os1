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

#include "Te_SimPacketGPRSRel99NotifyProfileChangeTest.h"

CSimPacketGPRSRel99NotifyProfileChangeTest::CSimPacketGPRSRel99NotifyProfileChangeTest() 
	{ 
	SetTestStepName(_L("GPRSRel99NotifyProfileChangeTest"));
	}

TVerdict CSimPacketGPRSRel99NotifyProfileChangeTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketGPRSRel99NotifyProfileChangeTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TRequestStatus gprsReqStatus, rel99ReqStatus;
	TRequestStatus gprsNotifyStatus, rel99NotifyStatus;

	// Open phone
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	// Open GPRS packet service
	TESTL(iGPRSPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	// Open GPRS context
	TName contextName;
	TName contextNameCompare;
	TESTL(iGPRSPacketContext.OpenNewContext(iGPRSPacketService, contextName)==KErrNone);
	contextNameCompare.Append(KSimPrimaryPacketContextName);
	contextNameCompare.AppendNum(1);
	TESTL(contextName.Compare(contextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened Context object"));

	// Open GPRS PacketQoS
	TName gprsQosName;
	TESTL(iGPRSPacketqos.OpenNewQoS(iGPRSPacketContext, gprsQosName)==KErrNone);

	TName gprsQosNameCompare;
	gprsQosNameCompare.Append(KSimPacketQosName);
	gprsQosNameCompare.AppendNum(1);
	TESTL(gprsQosName.Compare(gprsQosNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS QoS object"));
	
	// Open UMTS Rel99 packet service - opened from same session RTelServer object as GPRS packet service
	TESTL(iRel99PacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Rel99 Packet object"));

	// Open UMTS Rel99 context
	TESTL(iRel99PacketContext.OpenExistingContext(iRel99PacketService, contextName) == KErrNone);

	// Open Rel99 PacketQoS
	TESTL(iRel99Packetqos.OpenExistingQoS(iRel99PacketContext, gprsQosName)==KErrNone);
	INFO_PRINTF1(_L("Opened Rel99 QoS object"));

	//Configure GPRS parameters
	//
	//
	// Set GPRS QoS Profile Params
	RPacketQoS::TQoSGPRSRequested aGPRSQoSReqConfig;
	TPckg<RPacketQoS::TQoSGPRSRequested> aGPRSQoSReqPckg(aGPRSQoSReqConfig);

	// post a GPRS notification
 	RPacketQoS::TQoSGPRSNegotiated aGPRSNotifyQoS;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> aGPRSNotifyQoSPckg(aGPRSNotifyQoS);

	//GPRS Profile data
	aGPRSQoSReqConfig.iMinDelay = RPacketQoS::EDelayClass1;
	aGPRSQoSReqConfig.iMinMeanThroughput = RPacketQoS::EMeanThroughput200;
	aGPRSQoSReqConfig.iMinPeakThroughput = RPacketQoS::EPeakThroughput16000;
	aGPRSQoSReqConfig.iMinPrecedence = RPacketQoS::EPriorityLowPrecedence;
	aGPRSQoSReqConfig.iMinReliability = RPacketQoS::EReliabilityClass1;
	aGPRSQoSReqConfig.iReqDelay = RPacketQoS::EDelayClass2;
	aGPRSQoSReqConfig.iReqMeanThroughput = RPacketQoS::EMeanThroughput2000;
	aGPRSQoSReqConfig.iReqPeakThroughput = RPacketQoS::EPeakThroughput64000;
	aGPRSQoSReqConfig.iReqPrecedence = RPacketQoS::EPriorityMediumPrecedence;
	aGPRSQoSReqConfig.iReqReliability = RPacketQoS::EReliabilityClass2;
	//
	//


	//Configure Rel99 QoS Profile Params
	//
	//
	RPacketQoS::TQoSR99_R4Requested aR99QoSReqConfig;
	TESTL(aR99QoSReqConfig.ExtensionId()==TPacketDataConfigBase::KConfigRel99Rel4);
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
	//
	//

	//post the notifications
	iRel99Packetqos.NotifyProfileChanged(rel99NotifyStatus, aR99NotifyQoSPckg);
	iGPRSPacketqos.NotifyProfileChanged(gprsNotifyStatus, aGPRSNotifyQoSPckg);
	
	//Set the profile
	iRel99Packetqos.SetProfileParameters(rel99ReqStatus, aR99QoSReqPckg);
	iGPRSPacketqos.SetProfileParameters(gprsReqStatus, aGPRSQoSReqPckg);	

	User::WaitForRequest(rel99NotifyStatus);
	User::WaitForRequest(gprsNotifyStatus);
	

	//User::WaitForRequest(rel99ReqStatus);
	User::WaitForRequest(rel99ReqStatus);
	User::WaitForRequest(gprsReqStatus);
	
	//Test the notification packages are correct

	//Rel99
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

	//Gprs
	TESTL(aGPRSNotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TESTL(aGPRSNotifyQoS.iDelay == RPacketQoS::EDelayClass2);
	TESTL(aGPRSNotifyQoS.iDelay == RPacketQoS::EDelayClass2);
	TESTL(aGPRSNotifyQoS.iMeanThroughput == RPacketQoS::EMeanThroughput2000);
	TESTL(aGPRSNotifyQoS.iPeakThroughput == RPacketQoS::EPeakThroughput64000);
	TESTL(aGPRSNotifyQoS.iPrecedence == RPacketQoS::EPriorityMediumPrecedence);
	TESTL(aGPRSNotifyQoS.iReliability == RPacketQoS::EReliabilityClass2);
	
	// Close
	iGPRSPacketqos.Close();
	iGPRSPacketContext.Close();
	iGPRSPacketService.Close();
	iRel99Packetqos.Close();
	iRel99PacketContext.Close();
	iRel99PacketService.Close();
	iPhone.Close();
	
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

