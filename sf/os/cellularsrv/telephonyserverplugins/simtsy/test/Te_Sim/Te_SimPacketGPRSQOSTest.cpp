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

#include "Te_SimPacketGPRSQOSTest.h"

CSimPacketGPRSQOSTest::CSimPacketGPRSQOSTest() 
	{ 
	SetTestStepName(_L("GPRSQOSTest"));
	}

TVerdict CSimPacketGPRSQOSTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketGPRSQOSTest"));

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

	TRequestStatus reqStatus;
	TRequestStatus notifyStatus;
	TName gprsQosName;
	TESTL(iGPRSPacketqos.OpenNewQoS(iFirstPrimaryPacketContext, gprsQosName)==KErrNone);
	TName gprsQosNameCompare;
	gprsQosNameCompare.Append(KSimPacketQosName);
	gprsQosNameCompare.AppendNum(1);
	TESTL(gprsQosName.Compare(gprsQosNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS QoS object"));
	
	TName qosname;
	iFirstPrimaryPacketContext.GetProfileName(qosname);
	INFO_PRINTF1(_L("Retrieved GPRS QoS object reference name"));

	// Set QoS Profile Params
	RPacketQoS::TQoSGPRSRequested aGPRSQoSReqConfig;
	TPckg<RPacketQoS::TQoSGPRSRequested> aGPRSQoSReqPckg(aGPRSQoSReqConfig);

	// post a notification
 	RPacketQoS::TQoSGPRSNegotiated aGPRSNotifyQoS;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> aGPRSNotifyQoSPckg(aGPRSNotifyQoS);

	//Profile data
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

	//post the notification
	iGPRSPacketqos.NotifyProfileChanged(notifyStatus, aGPRSNotifyQoSPckg);
	//Set the config
	iGPRSPacketqos.SetProfileParameters(reqStatus, aGPRSQoSReqPckg);
	
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);

	TESTL(aGPRSNotifyQoS.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TESTL(aGPRSNotifyQoS.iDelay == RPacketQoS::EDelayClass2);
	TESTL(aGPRSNotifyQoS.iDelay == RPacketQoS::EDelayClass2);
	TESTL(aGPRSNotifyQoS.iMeanThroughput == RPacketQoS::EMeanThroughput2000);
	TESTL(aGPRSNotifyQoS.iPeakThroughput == RPacketQoS::EPeakThroughput64000);
	TESTL(aGPRSNotifyQoS.iPrecedence == RPacketQoS::EPriorityMediumPrecedence);
	TESTL(aGPRSNotifyQoS.iReliability == RPacketQoS::EReliabilityClass2);	
	
	// post a notification
	TRequestStatus cancelStatus;
	iGPRSPacketqos.NotifyProfileChanged(cancelStatus, aGPRSNotifyQoSPckg);	
	iGPRSPacketqos.CancelAsyncRequest(EPacketQoSNotifyProfileChanged);
	User::WaitForRequest(cancelStatus);

	TESTL(cancelStatus.Int()==KErrCancel);
	INFO_PRINTF1(_L("Set Profile Test passed"));

	// need to call Notify first to set pending to true after 
	// cancel event which sets it to false	
	iGPRSPacketqos.NotifyProfileChanged(notifyStatus, aGPRSNotifyQoSPckg);	
	iGPRSPacketqos.SetProfileParameters(reqStatus, aGPRSQoSReqPckg);
	User::WaitForRequest(notifyStatus);
	TESTL(notifyStatus.Int()==KErrNone);
	User::WaitForRequest(reqStatus);	
	TESTL(reqStatus.Int()==KErrNone);

	// test RPacketContext::Activate - before calling ModifyActiveContext
	StartNTRasSimulation();
	iFirstPrimaryPacketContext.Activate(reqStatus);
	User::WaitForRequest(reqStatus);	
	TESTL(reqStatus.Int()==KErrNone);
	INFO_PRINTF1(_L("Activate test passed"));	

	// test RPacketContext::ModifyActiveContext - it shouldn't work unless
	// a profile/tft has changed since activation
	TRequestStatus modifyStatus;
	iFirstPrimaryPacketContext.ModifyActiveContext(modifyStatus);
	User::WaitForRequest(modifyStatus);
	TESTL(modifyStatus.Int()==KErrNotReady); 
	iGPRSPacketqos.SetProfileParameters(reqStatus, aGPRSQoSReqPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone); 
	iFirstPrimaryPacketContext.ModifyActiveContext(modifyStatus);
	User::WaitForRequest(modifyStatus);
	TESTL(modifyStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("ModifyActiveContext Test Passed"));
	INFO_PRINTF1(_L("Notify GPRS QoS Profile Test passed"));
	
	// Get QoS GPRS Capabilities
	RPacketQoS::TQoSCapsGPRS aQoSCaps;
	TPckg<RPacketQoS::TQoSCapsGPRS> aQoSCapsPckg(aQoSCaps);
	
	iGPRSPacketqos.GetProfileCapabilities(reqStatus, aQoSCapsPckg);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(aQoSCaps.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TESTL(aQoSCaps.iDelay == RPacketQoS::EDelayClass2);
	TESTL(aQoSCaps.iMean == RPacketQoS::EMeanThroughput100);
	TESTL(aQoSCaps.iPeak == RPacketQoS::EPeakThroughput64000);
	TESTL(aQoSCaps.iPrecedence == RPacketQoS::EPriorityMediumPrecedence);
	TESTL(aQoSCaps.iReliability == RPacketQoS::EReliabilityClass2);
	INFO_PRINTF1(_L("Get QoS Profile  Capabilities Test passed"));

	//Get QoS Profile Params
	RPacketQoS::TQoSGPRSNegotiated aGPRSQoSNegConfig;
	TPckg<RPacketQoS::TQoSGPRSNegotiated> aGPRSQoSNegPckg(aGPRSQoSNegConfig);

	iGPRSPacketqos.GetProfileParameters(cancelStatus, aGPRSQoSNegPckg);
	iGPRSPacketqos.CancelAsyncRequest(EPacketQoSGetProfileParams);//no implementation-does nothing
	User::WaitForRequest(cancelStatus);
	TESTL(cancelStatus.Int()==KErrNone);

	TESTL(aGPRSQoSNegConfig.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TESTL(aGPRSQoSNegConfig.iDelay==RPacketQoS::EDelayClass2);
	TESTL(aGPRSQoSNegConfig.iMeanThroughput==RPacketQoS::EMeanThroughput100);
	TESTL(aGPRSQoSNegConfig.iPeakThroughput==RPacketQoS::EPeakThroughput64000);
	TESTL(aGPRSQoSNegConfig.iPrecedence==RPacketQoS::EPriorityMediumPrecedence);
	TESTL(aGPRSQoSNegConfig.iReliability==RPacketQoS::EReliabilityClass2);
	INFO_PRINTF1(_L("Get QoS Profile Test passed"));

	// End of RPacketQos tests
	iGPRSPacketqos.Close();
	iFirstPrimaryPacketContext.Close();
	iPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

