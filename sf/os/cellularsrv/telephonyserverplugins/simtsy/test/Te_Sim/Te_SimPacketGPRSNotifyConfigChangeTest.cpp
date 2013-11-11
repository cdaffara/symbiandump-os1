// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Te_SimPacketGPRSNotifyConfigChangeChangeTest.cpp
//



/**
 @file
*/

#include "Te_SimPacketGPRSNotifyConfigChangeTest.h"

CSimPacketGPRSNotifyConfigChangeTest::CSimPacketGPRSNotifyConfigChangeTest()
	{ 
	SetTestStepName(_L("GPRSNotifyConfigChangeTest"));
	}

TVerdict CSimPacketGPRSNotifyConfigChangeTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketGPRSNotifyConfigChangeTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TRequestStatus gprsReqStatus;
	TRequestStatus gprsNotifyStatus;
	
	TRequestStatus gprsNotifyStatus2;

	// Open phone
	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret);
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	// Open GPRS packet service
	TESTL(iGPRSPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	// Open GPRS context
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iGPRSPacketContext.OpenNewContext(iGPRSPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	//Opena second context from the first
	RPacketContext GPRSPacketContext2;
	TESTL(GPRSPacketContext2.OpenExistingContext(iGPRSPacketService, gprsContextName) == KErrNone);
	
	// Configure GPRS context.
	RPacketContext::TContextConfigGPRS gprsContextConfig;
	TESTL(gprsContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS(gprsContextConfig);
	gprsContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	gprsContextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	gprsContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	gprsContextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	gprsContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	gprsContextConfig.iUseEdge = ETrue;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	gprsContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	gprsContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

		//set up notification package objects (only on heap to avoid chkstk problems)

	//first gprs notify object
	RPacketContext::TContextConfigGPRS* gprsContextNotify = new(ELeave) RPacketContext::TContextConfigGPRS;
	CleanupStack::PushL(gprsContextNotify);
	TESTL(gprsContextNotify->ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckgGPRS(*gprsContextNotify);
	//second gprs notify object 
	RPacketContext::TContextConfigGPRS* gprsContextNotify2 = new(ELeave) RPacketContext::TContextConfigGPRS;
	CleanupStack::PushL(gprsContextNotify2);
	TESTL(gprsContextNotify2->ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckgGPRS2(*gprsContextNotify2);
	
	// Test

	iGPRSPacketContext.NotifyConfigChanged(gprsNotifyStatus, contextNotifyPckgGPRS);
	GPRSPacketContext2.NotifyConfigChanged(gprsNotifyStatus2, contextNotifyPckgGPRS2);
	iGPRSPacketContext.SetConfig(gprsReqStatus, contextConfigPckgGPRS);
	

	User::WaitForRequest(gprsNotifyStatus);
	User::WaitForRequest(gprsNotifyStatus2);
	

	User::WaitForRequest(gprsReqStatus);
	

	//Check notify packages have correct parameters

	
	//1st gprs object
	TESTL(gprsContextNotify->iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextNotify->iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextNotify->iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextNotify->iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextNotify->iPdpType==DPCKTTSY_PDP_TYPE1);

	//2nd gprs object
	TESTL(gprsContextNotify2->iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextNotify2->iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextNotify2->iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextNotify2->iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextNotify2->iPdpType==DPCKTTSY_PDP_TYPE1);

	CleanupStack::PopAndDestroy(2); 
	
	// Close
	GPRSPacketContext2.Close();
	iGPRSPacketContext.Close();
	iGPRSPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

