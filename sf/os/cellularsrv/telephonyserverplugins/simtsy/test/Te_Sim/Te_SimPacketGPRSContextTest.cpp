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

#include "Te_SimPacketGPRSContextTest.h"

CSimPacketGPRSContextTest::CSimPacketGPRSContextTest() 
	{ 
	SetTestStepName(_L("GPRSContextTest"));
	}

TVerdict CSimPacketGPRSContextTest::doTestStepL()
	{
	INFO_PRINTF1(_L("BeginPacketGPRSContextTest"));

 	CreateConfigFileL(_L("c:\\config3.txt"));
 	SetTestNumberL(6);

	TInt ret = iPhone.Open(iTelServer,KPhoneName); 
	INFO_PRINTF2(_L("Result: %d"),ret); 
	TESTL(ret == KErrNone); 
	INFO_PRINTF1(_L("Opened phone object"));

	TESTL(iGPRSPacketService.Open(iPhone)==KErrNone);
	INFO_PRINTF1(_L("Opened Packet object"));

	TRequestStatus reqStatus;
	TRequestStatus notifyStatus;

		// test RPacketContext::OpenNewContext
	TName gprsContextName;
	TName gprsContextNameCompare;
	TESTL(iGPRSPacketContext.OpenNewContext(iGPRSPacketService, gprsContextName)==KErrNone);
	gprsContextNameCompare.Append(KSimPrimaryPacketContextName);
	gprsContextNameCompare.AppendNum(1);
	TESTL(gprsContextName.Compare(gprsContextNameCompare)==KErrNone);
	INFO_PRINTF1(_L("Opened GPRS Context object"));

	// GPRS	
	RPacketContext::TContextConfigGPRS gprsContextConfig;
	TESTL(gprsContextConfig.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextConfigPckgGPRS(gprsContextConfig);

	RPacketContext::TContextConfigGPRS gprsContextNotify;
	TESTL(gprsContextNotify.ExtensionId()==TPacketDataConfigBase::KConfigGPRS);
	TPckg<RPacketContext::TContextConfigGPRS> contextNotifyPckgGPRS(gprsContextNotify);

	gprsContextConfig.iAccessPointName = DPCKTTSY_ACCESS_POINT1;
	gprsContextConfig.iAnonymousAccessReqd = DPCKTTSY_ANONYMOUS_ACCESS1;
	gprsContextConfig.iPdpAddress = DPCKTTSY_PDP_ADDRESS1;
	gprsContextConfig.iPdpCompression = DPCKTTSY_COMPRESSION1;
	gprsContextConfig.iPdpType = DPCKTTSY_PDP_TYPE1;
	gprsContextConfig.iUseEdge = DPCKTTSY_EGPRS_REQUIRED;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol = SIM_PACKET_CONTEXT_PROTOCOL1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iUsername = SIM_PACKET_CONTEXT_USERNAME1;
	gprsContextConfig.iProtocolConfigOption.iAuthInfo.iPassword = SIM_PACKET_CONTEXT_PASSWORD1;
	gprsContextConfig.iProtocolConfigOption.iChallenge = SIM_PACKET_CONTEXT_CHALLENGE1;
	gprsContextConfig.iProtocolConfigOption.iResponse = SIM_PACKET_CONTEXT_RESPONSE1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns = SIM_PACKET_CONTEXT_PRIMARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns = SIM_PACKET_CONTEXT_SECONDARYSERVER1;
	gprsContextConfig.iProtocolConfigOption.iId = SIM_PACKET_CONTEXT_ID1;

	iGPRSPacketContext.NotifyConfigChanged(notifyStatus, contextNotifyPckgGPRS);
	iGPRSPacketContext.SetConfig(reqStatus, contextConfigPckgGPRS);
	
	User::WaitForRequest(notifyStatus);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(notifyStatus.Int()==KErrNone);
	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.12"));

	INFO_PRINTF1(_L("SetConfig GPRS test passed"));

	TESTL(gprsContextNotify.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextNotify.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextNotify.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextNotify.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextNotify.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextNotify.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);

	INFO_PRINTF1(_L("NotifyConfigChanged GPRS Test Passed"));

	// now test the getting of DNS server names
	RPacketContext::TDnsInfoV2 dnsInfoV2;
	RPacketContext::TTDnsInfoV2Pckg dnsInfoV2pckg(dnsInfoV2);

	iGPRSPacketContext.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	User::WaitForRequest(reqStatus); 
	
	TESTL(dnsInfoV2.iPrimaryDns==SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(dnsInfoV2.iSecondaryDns==SIM_PACKET_CONTEXT_SECONDARYSERVER1);

	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("GetDnsInfo Test Passed")); 	

	// test cancelling of retrieval of DNS server names
	iGPRSPacketContext.GetDnsInfo(reqStatus, dnsInfoV2pckg);
	iGPRSPacketContext.CancelAsyncRequest(EPacketContextGetDNSInfo);
	User::WaitForRequest(reqStatus); 
	TESTL(reqStatus.Int()==KErrNone); 
	INFO_PRINTF1(_L("GetDnsInfoCancel Test Passed"));
	
	reqStatus = KRequestPending;
	iGPRSPacketContext.SetConfig(reqStatus, contextConfigPckgGPRS);
	iGPRSPacketContext.CancelAsyncRequest(EPacketContextSetConfig);
	User::WaitForRequest(reqStatus);

	TESTL(reqStatus.Int()==KErrNone);  //not supporting cancel of SetConfig

	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.15"));
	INFO_PRINTF1(_L("SetConfig Cancel GPRS test passed"));
	// GPRS tests finished
	
	// GetConfig - GPRS
	iGPRSPacketContext.GetConfig(reqStatus, contextConfigPckgGPRS);
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus.Int()==KErrNone);
	TESTL(gprsContextConfig.iAccessPointName==DPCKTTSY_ACCESS_POINT1);
	TESTL(gprsContextConfig.iAnonymousAccessReqd==DPCKTTSY_ANONYMOUS_ACCESS1);
	TESTL(gprsContextConfig.iPdpAddress==DPCKTTSY_PDP_ADDRESS1);
	TESTL(gprsContextConfig.iPdpCompression==DPCKTTSY_COMPRESSION1);
	TESTL(gprsContextConfig.iPdpType==DPCKTTSY_PDP_TYPE1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iAuthInfo.iProtocol == SIM_PACKET_CONTEXT_PROTOCOL1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iAuthInfo.iUsername == SIM_PACKET_CONTEXT_USERNAME1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iAuthInfo.iPassword == SIM_PACKET_CONTEXT_PASSWORD1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iChallenge == SIM_PACKET_CONTEXT_CHALLENGE1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iResponse == SIM_PACKET_CONTEXT_RESPONSE1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iPrimaryDns == SIM_PACKET_CONTEXT_PRIMARYSERVER1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iDnsAddresses.iSecondaryDns == SIM_PACKET_CONTEXT_SECONDARYSERVER1);
	TESTL(gprsContextConfig.iProtocolConfigOption.iId == SIM_PACKET_CONTEXT_ID1);

	CHECKPOINTL(reqStatus.Int(),KErrNone,CHP_CNTXT_CASE("E.18"));
	INFO_PRINTF1(_L("GetConfig test passed"));

	iGPRSPacketContext.Close();
	iGPRSPacketService.Close();
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);

	return TestStepResult();
	}

